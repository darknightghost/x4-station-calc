#if defined(OS_WINDOWS)

    #include <atomic>

    #include <Windows.h>
    #include <tlhelp32.h>

    #include <common/auto_release.h>

class CrashHandler {
  private:
    using SetUnhandledExceptionFilterFuncType
        = LPTOP_LEVEL_EXCEPTION_FILTER (*)(LPTOP_LEVEL_EXCEPTION_FILTER);

  private:
    static CrashHandler _instance; ///< Instance.
    SetUnhandledExceptionFilterFuncType
        m_realSetUnhandledExceptionFilter; ///< Real
                                           ///< SetUnhandledExceptionFilter().
    ::std::atomic<LPTOP_LEVEL_EXCEPTION_FILTER>
        m_topLevelExceptionFiler; ///< Top level exception filter.

  public:
    /**
     * @brief	Constructor.
     */
    CrashHandler();

    /**
     * @brief	Destructor.
     */
    virtual ~CrashHandler();

  private:
    /**
     * @brief		Exception handler.
     *
     * @param[in]	exception		Exception pointser.
     */
    static LONG WINAPI onCrash(struct _EXCEPTION_POINTERS *exceptions);

    /**
     * @brief		Fake SetUnhandledExceptionFilter().
     *
     * @param[in]	exception		Exception pointser.
     */
    static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

    /**
     * @@brief		Enable EAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableEATHook();

    /**
     * @@brief		Disable EAT hook.
     */
    void disableEATHook();

    /**
     * @@brief		Enable IAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableIATHook();
};

CrashHandler CrashHandler::_instance; ///< Instance.

/**
 * @brief	Constructor.
 */
CrashHandler::CrashHandler() :
    m_realSetUnhandledExceptionFilter(nullptr),
    m_topLevelExceptionFiler(nullptr)
{
    // Enable EAT hook.
    if (! this->enableEATHook()) {
        ::MessageBoxA(NULL,
                      "Failed to make EAT hook, the crash handler is disabled.",
                      "EAT Hook Failed", MB_OK | MB_ICONERROR);
        return;
    }

    // Enable IAT hook.
    if (! this->enableIATHook()) {
        ::MessageBoxA(NULL,
                      "Failed to make IAT hook, the crash handler is disabled.",
                      "IAT Hook Failed", MB_OK | MB_ICONERROR);
        this->disableEATHook();
        return;
    }

    // Register crash handler.
}

/**
 * @brief	Destructor.
 */
CrashHandler::~CrashHandler() {}

/**
 * @brief		Exception handler.
 */
LONG CrashHandler::onCrash(struct _EXCEPTION_POINTERS *exceptions)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

/**
 * @brief		Fake SetUnhandledExceptionFilter().
 *
 * @param[in]	exception		Exception pointser.
 */
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
    CrashHandler::fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    return _instance.m_topLevelExceptionFiler.exchange(
        lpTopLevelExceptionFilter);
}

/**
 * @@brief		Enable EAT hook.
 */
bool CrashHandler::enableEATHook()
{
    return true;
}

/**
 * @@brief		Disable EAT hook.
 */
void CrashHandler::disableEATHook() {}

/**
 * @@brief		Enable IAT hook.
 */
bool CrashHandler::enableIATHook()
{
    // Traverse loaded modules.
    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (hnd == INVALID_HANDLE_VALUE) {
        return false;
    }
    AutoRelease<HANDLE> releaseSnapshot(snapshot, [](HANDLE &hnd) -> void {
        ::CloseHandle(hnd);
    });

    MODULEENTRY32 entery;
    for (BOOL hasNext = ::Module32First(snapshot, &entery); hasNext;
         hasNext      = ::Module32Next(snapshot, &entery)) {
        // Find DOS header.
        PIMAGE_DOS_HEADER dosHeader
            = reinterpret_cast<PIMAGE_DOS_HEADER>(entery.modBaseAddr);

        if (dosHeader == NULL) {
            return false;
        }

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return false;
        }

        // Find NT header.
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
            reinterpret_cast<uint8_t *>(dosHeader) + dosHeader->e_lfanew);

        if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
            return false;
        }
    }

    return true;
}

#endif
