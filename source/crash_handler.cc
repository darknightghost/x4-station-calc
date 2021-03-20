#if defined(OS_WINDOWS)

    #include <Dbghelp.h>
    #include <windows.h>

class CrashHandler {
  private:
    static CrashHandler _instance; ///< Instance.

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
     * @@brief		Disable \c SetUnhandledExceptionFilter().
     */
    void disableSetUnhandledExceptionFilter();
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
