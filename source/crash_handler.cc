#if defined(OS_WINDOWS)

    #include <Dbghelp.h>
    #include <Windows.h>

class CrashHandler {
  private:
    using SetUnhandledExceptionFilterFuncType
        = WINAPI LPTOP_LEVEL_EXCEPTION_FILTER (*)(LPTOP_LEVEL_EXCEPTION_FILTER);

  private:
    static CrashHandler _instance; ///< Instance.
    SetUnhandledExceptionFilterFuncType
        m_realSetUnhandledExceptionFilter; ///< Real
                                           ///< SetUnhandledExceptionFilter().

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
    static WINAPI LONG onCrash(struct _EXCEPTION_POINTERS *exceptions);

    /**
     * @brief		Fake SetUnhandledExceptionFilter().
     *
     * @param[in]	exception		Exception pointser.
     */
    static WINAPI LPTOP_LEVEL_EXCEPTION_FILTER fakeSetUnhandledExceptionFilter(
        LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

    /**
     * @@brief		Enable IAT hook.
     */
    bool enableIATHook();
};

CrashHandler::_instance(); ///< Instance.

/**
 * @brief	Constructor.
 */
CrashHandler::CrashHandler()
{
    // Register crash handler.
    ::SetUnhandledExceptionFilter(&CrashHandler::onCrash);
    disableSetUnhandledExceptionFilter();
}

/**
 * @brief	Destructor.
 */
~CrashHandler() {}

/**
 * @brief		Exception handler.
 */
LONG CrashHandler::onCrash(struct _EXCEPTION_POINTERS *exceptions)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

/**
 * @@brief		Disable \c SetUnhandledExceptionFilter().
 */
void CrashHandler::disableSetUnhandledExceptionFilter() {}

#endif
