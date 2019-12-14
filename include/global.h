#pragma once

#include <functional>
#include <map>
#include <string>
#include <sstream>

#include <QtCore/QObject>

#include <design_model/singleton.h>

/**
 * @brief   Global informations.
 */
class Global : public Singleton<Global, int&, char**&, int&> {
        SIGNLETON_OBJECT(Global, int&, char**&, int&)
    private:
        /**
         * @brief   Argument info.
         */
        struct ArgInfo {
            ::std::string               shortName;  //< Short name.
            ::std::string               longName;   //< Long name.
            ::std::string               help;       //< Help.
            ::std::function<bool()>     handler;    //< Handler.
        };

    private:
        ::std::map<char, ArgInfo>   m_argMap;   //< Arguments.
        QString             m_execDir;      //< Path of current executable file.
        QString             m_configPath;   //< Path of config file.

    protected:
        /**
         * @brief       Constructor.
         *
         * @param[in]   argc        argc in main.
         * @param[in]   argv        argv in main.
         * @param[out]  exitCode    Exitcode when error occured.
         */
        Global(int& argc, char**& argv, int& exitCode);

    public:
        /**
         * @brief       Get the direcotry of current executable file.
         *
         * @preturn     Path of the file.
         */
        const QString&      execDir();

        /**
         * @brief       Get path of config file.
         *
         * @preturn     Path of the file.
         */
        const QString&      configPath();

        /**
         * @brief   Destructor.
         */
        virtual     ~Global();

    private:
        /**
         * @brief       Show help.
         *
         * @param[in]   arg0    The first argument in argv,
         */
        void        showHelp(const char* arg0);

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
        bool    parseArgs(int& argc, char**& argv, int& exitCode);


};
