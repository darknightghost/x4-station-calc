#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <string>

#include <QtCore/QObject>

#include <interfaces/i_singleton.h>

/**
 * @brief   Global informations.
 */
class Global : public ISingleton<Global, int &, char **&, int &> {
    SIGNLETON_OBJECT(Global, int &, char **&, int &)
  private:
    /**
     * @brief   Argument info.
     */
    struct ArgInfo {
        ::std::string           shortName; ///< Short name.
        ::std::string           longName;  ///< Long name.
        ::std::string           help;      ///< Help.
        ::std::function<bool()> handler;   ///< Handler.
    };

  private:
    ::std::map<char, ArgInfo> m_argMap;  ///< Arguments.
    QString                   m_execDir; ///< Path of current executable file.
    QString                   m_configPath;    ///< Path of config file.
    bool                      m_hasFileToOpen; ///< Has file to open.
    QString                   m_fileToOpen;    ///< File to open.

  protected:
    /**
     * @brief       Constructor.
     *
     * @param[in]   argc        argc in main.
     * @param[in]   argv        argv in main.
     * @param[out]  exitCode    Exitcode when error occured.
     */
    Global(int &argc, char **&argv, int &exitCode);

  public:
    /**
     * @brief       Get the direcotry of current executable file.
     *
     * @return		Path of the file.
     */
    const QString &execDir() const;

    /**
     * @brief       Get path of config file.
     *
     * @return		Path of the file.
     */
    const QString &configPath() const;

    /**
     * @brief       Check if there is a file to open.
     *
     * @return		True if has a file to open, otherwise returns false.
     */
    bool hasFileToOpen() const;

    /**
     * @brief       Get path of file to open.
     *
     * @return		Path of the file to open.
     */
    const QString &fileToOpen() const;

    /**
     * @brief   Destructor.
     */
    virtual ~Global();

  private:
    /**
     * @brief       Show help.
     *
     * @param[in]   arg0    The first argument in argv,
     */
    void showHelp(const char *arg0);

    /**
     * @brief       Parse arguments.
     *
     * @param[in]   argc        argc in main.
     * @param[in]   argv        argv in main.
     * @param[out]  exitCode    Exitcode when error occured.
     *
     * @return      True if the constructor should continue executing,
     *              otherwise returns false.
     */
    bool parseArgs(int &argc, char **&argv, int &exitCode);
};
