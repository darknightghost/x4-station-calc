#if defined(OS_WINDOWS)

    #include <atomic>

    #include <Windows.h>

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
     * @@brief		Enable IAT hook.
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
    // Enable IAT hook.
    if (! this->enableIATHook()) {
        ::MessageBoxA(NULL,
                      "Failed to make IAT hook, the crash handler is disabled.",
                      "IAT Hook Failed", MB_OK | MB_ICONERROR);
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
 * @@brief		Enable IAT hook.
 */
bool CrashHandler::enableIATHook()
{
    // Find DOS header.
    PIMAGE_DOS_HEADER dosHeader
        = reinterpret_cast<PIMAGE_DOS_HEADER>(::GetModuleHandle(NULL));

    if (dosHeader == NULL) {
        return false;
    }

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    // Find NT header.
    PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<uint8_t *>(dosHeader) + dosHeader->e_lfanew);

    if (ntHeader->->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    return true;
}

#endif
