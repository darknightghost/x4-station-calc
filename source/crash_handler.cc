#if defined(OS_WINDOWS)

    #include <atomic>
    #include <cstring>

    #include <Windows.h>
    #include <tlhelp32.h>

    #include <Memoryapi.h>

    #include <common/auto_release.h>

class CrashHandler {
  private:
    using SetUnhandledExceptionFilterFuncType
        = LPTOP_LEVEL_EXCEPTION_FILTER (*)(LPTOP_LEVEL_EXCEPTION_FILTER);

  private:
    static CrashHandler _instance; ///< Instance.
    WCHAR
    m_dumpFilePath[MAX_PATH + 1]; ///< Buffer of the path of the dump file.
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
     * @param[in]	exceptionInfo       Exception information.
     */
    static LONG WINAPI onCrash(struct _EXCEPTION_POINTERS *exceptionInfo);

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
     * @@brief		Enable IAT hook.
     *
     * @return      On success, the method returns \c true. Otherwise returns
     *              \c false.
     */
    bool enableIATHook();

    /**
     * @brief		Save the dump file.
     *
     * @param[in]	exceptionInfo       Exception information.
     */
    void saveDump(struct _EXCEPTION_POINTERS *exceptionInfo);
};

CrashHandler CrashHandler::_instance; ///< Instance.

/**
 * @brief	Constructor.
 */
CrashHandler::CrashHandler() :
    m_dumpFilePath(L"x4-station-calc.dmp"),
    m_realSetUnhandledExceptionFilter(nullptr),
    m_topLevelExceptionFiler(nullptr)
{
    m_realSetUnhandledExceptionFilter
        = reinterpret_cast<SetUnhandledExceptionFilterFuncType>(
            ::GetProcAddress(GetModuleHandle("KERNEL32.dll"),
                             "SetUnhandledExceptionFilter"));
    if (m_realSetUnhandledExceptionFilter == nullptr) {
        ::MessageBoxW(NULL, L"Failed to find SetUnhandledExceptionFilter().",
                      L"Unknow Error", MB_OK | MB_ICONERROR);
    }

    // Enable EAT hook.
    if (! this->enableEATHook()) {
        ::MessageBoxW(
            NULL, L"Failed to make EAT hook, the crash handler is disabled.",
            L"EAT Hook Failed", MB_OK | MB_ICONERROR);
    }

    // Enable IAT hook.
    if (! this->enableIATHook()) {
        ::MessageBoxW(
            NULL, L"Failed to make IAT hook, the crash handler is disabled.",
            L"IAT Hook Failed", MB_OK | MB_ICONERROR);
    }

    // Register crash handler.
    m_topLevelExceptionFiler
        = m_realSetUnhandledExceptionFilter(&CrashHandler::onCrash);
}

/**
 * @brief	Destructor.
 */
CrashHandler::~CrashHandler() {}

/**
 * @brief		Exception handler.
 */
LONG CrashHandler::onCrash(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    // Ask user to save a dump file.
    if (::MessageBoxW(
            NULL,
            L"X4 Station Calculator has been crashed, would you like to "
            "save a core dump?",
            L"Crash", MB_YESNO | MB_ICONERROR)
        == IDYES) {
        _instance.saveDump(exceptionInfo);
    }

    // Call default exception handler.
    if (_instance.m_topLevelExceptionFiler != nullptr) {
        return _instance.m_topLevelExceptionFiler.load()(exceptionInfo);
    }
    return EXCEPTION_EXECUTE_HANDLER;
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
    uint8_t *moduleBaseAddr
        = reinterpret_cast<uint8_t *>(::GetModuleHandleA("KERNEL32.dll"));

    // Find DOS header.
    PIMAGE_DOS_HEADER dosHeader
        = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddr);

    if (dosHeader == NULL) {
        return false;
    }

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    // Find NT header.
    PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        moduleBaseAddr + dosHeader->e_lfanew);

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    // Find optional header.
    PIMAGE_OPTIONAL_HEADER optionalHeader = &(ntHeader->OptionalHeader);

    // Find exprot descriptor.
    PIMAGE_EXPORT_DIRECTORY exportDirectory
        = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
            moduleBaseAddr
            + optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                  .VirtualAddress);
    PDWORD addressOfNames = reinterpret_cast<PDWORD>(
        moduleBaseAddr + exportDirectory->AddressOfNames);

    // Search for symbol name.
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        LPCTSTR symbolName
            = reinterpret_cast<LPCTSTR>(moduleBaseAddr + addressOfNames[i]);

        if (::strcmp(symbolName, "SetUnhandledExceptionFilter") == 0) {
            // Find RVA.
            PWORD addressOfNameOrdinals = reinterpret_cast<PWORD>(
                moduleBaseAddr + exportDirectory->AddressOfNameOrdinals);
            PDWORD addressOfFunctions = reinterpret_cast<PDWORD>(
                moduleBaseAddr + exportDirectory->AddressOfFunctions);
            PDWORD targetRVA = addressOfFunctions + addressOfNameOrdinals[i];

            // Get real address.
            m_realSetUnhandledExceptionFilter
                = reinterpret_cast<SetUnhandledExceptionFilterFuncType>(
                    moduleBaseAddr + *targetRVA);

            DWORD  oldMemAttr = 0;
            SIZE_T written    = 0;

            // Make memory writable.
            ::VirtualProtect(targetRVA, sizeof(*targetRVA), PAGE_READWRITE,
                             &oldMemAttr);

            // Set hook.
            DWORD newRVA = static_cast<DWORD>(
                reinterpret_cast<UINT_PTR>(
                    &CrashHandler::fakeSetUnhandledExceptionFilter)
                - reinterpret_cast<UINT_PTR>(moduleBaseAddr));
            ::WriteProcessMemory(::GetCurrentProcess(), targetRVA, &newRVA,
                                 sizeof(newRVA), &written);

            // Resume memory attribute.
            ::VirtualProtect(targetRVA, sizeof(*targetRVA), oldMemAttr,
                             &oldMemAttr);

            return true;
        }
    }

    return false;
}

/**
 * @@brief		Enable IAT hook.
 */
bool CrashHandler::enableIATHook()
{
    // Traverse loaded modules.
    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    AutoRelease<HANDLE> releaseSnapshot(snapshot, [](HANDLE &hnd) -> void {
        ::CloseHandle(hnd);
    });

    MODULEENTRY32 entery;
    for (BOOL hasNext = ::Module32First(snapshot, &entery); hasNext;
         hasNext      = ::Module32Next(snapshot, &entery)) {
        uint8_t *moduleBaseAddr
            = reinterpret_cast<uint8_t *>(entery.modBaseAddr);
        // Find DOS header.
        PIMAGE_DOS_HEADER dosHeader
            = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBaseAddr);

        if (dosHeader == NULL) {
            return false;
        }

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return false;
        }

        // Find NT header.
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
            moduleBaseAddr + dosHeader->e_lfanew);

        if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
            return false;
        }

        // Find optional header.
        PIMAGE_OPTIONAL_HEADER optionalHeader = &(ntHeader->OptionalHeader);

        // Find import descriptors.
        for (PIMAGE_IMPORT_DESCRIPTOR importDescriptor
             = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
                 moduleBaseAddr
                 + optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                       .VirtualAddress);
             importDescriptor->Name != 0; ++importDescriptor) {
            LPCTSTR dllName = reinterpret_cast<LPCTSTR>(
                moduleBaseAddr + importDescriptor->Name);

            if (::_stricmp(dllName, "KERNEL32.dll") == 0) {
                PIMAGE_THUNK_DATA originalThunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->OriginalFirstThunk);
                PIMAGE_THUNK_DATA thunkData
                    = reinterpret_cast<PIMAGE_THUNK_DATA>(
                        moduleBaseAddr + importDescriptor->FirstThunk);
                for (; originalThunkData->u1.Ordinal != 0;
                     ++originalThunkData, ++thunkData) {
                    if (! (originalThunkData->u1.Ordinal
                           & IMAGE_ORDINAL_FLAG)) {
                        PIMAGE_IMPORT_BY_NAME importByName
                            = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                moduleBaseAddr + originalThunkData->u1.Ordinal);
                        if (::strcmp(importByName->Name,
                                     "SetUnhandledExceptionFilter")
                            == 0) {
                            DWORD  oldMemAttr    = 0;
                            SIZE_T written       = 0;
                            void * targetAddress = &(thunkData->u1.Ordinal);
                            // Make memory writable.
                            ::VirtualProtect(targetAddress, sizeof(void *),
                                             PAGE_READWRITE, &oldMemAttr);

                            // Set hook.
                            void *hookFunc
                                = &CrashHandler::
                                      fakeSetUnhandledExceptionFilter;
                            ::WriteProcessMemory(::GetCurrentProcess(),
                                                 targetAddress, &hookFunc,
                                                 sizeof(hookFunc), &written);

                            // Resume memory attribute.
                            ::VirtualProtect(targetAddress, sizeof(void *),
                                             oldMemAttr, &oldMemAttr);
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @@brief		Save the dump file.
 */
void CrashHandler::saveDump(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    // Get path.
    OPENFILENAMEW saveInfo;
    ::memset(&saveInfo, 0, sizeof(saveInfo));
    saveInfo.lStructSize     = sizeof(saveInfo);
    saveInfo.lpstrFilter     = L"Dump Files\0*.dmp\0";
    saveInfo.nFilterIndex    = 0;
    saveInfo.lpstrFile       = m_dumpFilePath;
    saveInfo.nMaxFile        = sizeof(m_dumpFilePath) / sizeof(WCHAR);
    saveInfo.lpstrFileTitle  = NULL;
    saveInfo.nMaxFileTitle   = NULL;
    saveInfo.lpstrInitialDir = NULL;
    saveInfo.lpstrTitle      = L"Select a Path to Save the Dump File";
    saveInfo.Flags           = OFN_OVERWRITEPROMPT;

    if (! ::GetSaveFileNameW(&saveInfo)) {
        return;
    }

    ::MessageBoxW(NULL, m_dumpFilePath, L"dump", MB_OK);
}

#endif
