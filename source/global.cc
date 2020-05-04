#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QApplication>

#include <getopt.h>

#include <global.h>

/**
 * @brief       Constructor.
 */
Global::Global(int &argc, char **&argv, int &exitCode)
{
    exitCode = 0;
    // Argument table
    m_argMap['h'] = {"h", "help", "Show this help.", [&]() -> bool {
                         exitCode = 0;
                         this->showHelp(argv[0]);
                         return false;
                     }};

    if (! this->parseArgs(argc, argv, exitCode)) {
        return;
    }

    if (m_hasFileToOpen) {
        qDebug() << "Has file to open : True.";
        qDebug() << "File to open : " << m_fileToOpen << ".";

    } else {
        qDebug() << "Has file to open : False.";
    }

    // Path of current file.
    m_execDir = QDir(QApplication::applicationDirPath()).absolutePath();
    qDebug() << "Tool dir : " << m_execDir << ".";

    // Path of config file
    QDir configDir(QStandardPaths::writableLocation(
        QStandardPaths::StandardLocation::AppLocalDataLocation));
    if (! configDir.exists()) {
        configDir.mkpath(".");
    }
    m_configPath = configDir.absoluteFilePath(".config");
    qDebug() << "Path of config file : " << m_configPath;

    this->setGood();
}

/**
 * @brief       Get the direcotry of current executable file.
 */
const QString &Global::execDir() const
{
    return m_execDir;
}

/**
 * @brief       Get path of config file.
 */
const QString &Global::configPath() const
{
    return m_configPath;
}

/**
 * @brief       Check if there is a file to open.
 */
bool Global::hasFileToOpen() const
{
    return m_hasFileToOpen;
}

/**
 * @brief       Get path of file to open.
 */
const QString &Global::fileToOpen() const
{
    return m_fileToOpen;
}

/**
 * @brief   Destructor.
 */
Global::~Global() {}

/**
 * @brief       Show help.
 */
void Global::showHelp(const char *arg0)
{
#define OPTION_WIDTH 24
#define MAX_WIDTH    80
    ::std::ostringstream ss;

    // Usage
    ss << "Usage: " << ::std::endl;
    ss << "    " << arg0 << " [OPTIONS] [FILE]" << ::std::endl;
    ss << ::std::endl;
    ss << "Calculator of stations in X4:Foundations." << ::std::endl;
    ss << ::std::endl;

    // Options
    ss << "Options: " << ::std::endl;

    for (auto iter = m_argMap.begin(); iter != m_argMap.end(); iter++) {
        // Option
        ::std::ostringstream shortStr, longStr;
        shortStr << "-" << iter->first;
        longStr << "--" << iter->second.longName;

        if (iter->second.shortName.length() > 1) {
            ::std::string upperStr = iter->second.longName;
            ::std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(),
                             ::toupper);
            shortStr << " " << upperStr;
            longStr << " " << upperStr;
        }

        // Strip lines
        ::std::vector<::std::string> optionLines, helpLines;

        // Strip options lines
        if (shortStr.str().size() + longStr.str().size() + 3 > OPTION_WIDTH) {
            optionLines.push_back(shortStr.str());
            optionLines.push_back(longStr.str());

        } else {
            shortStr << ", " << longStr.str();
            optionLines.push_back(shortStr.str());
        }

        // Strip help lines
        int  helpWidth = MAX_WIDTH - 4 * 2 - OPTION_WIDTH;
        auto wordBegin = iter->second.help.begin();

        while (wordBegin < iter->second.help.end()) {
            auto wordEnd = wordBegin + 1;

            if (wordEnd == iter->second.help.end()) {
                helpLines.push_back(iter->second.help.substr(
                    wordBegin - iter->second.help.begin(),
                    wordEnd - wordBegin));
                break;
            }

            auto nextEnd = wordEnd;

            while (nextEnd - wordBegin < helpWidth) {
                wordEnd = nextEnd;

                if (nextEnd == iter->second.help.end()) {
                    break;
                }

                // Get next end
                // Jump space
                while (nextEnd != iter->second.help.end()
                       && (*nextEnd == ' ' || *nextEnd == '\t')) {
                    nextEnd++;
                }

                // Jump word
                while (nextEnd != iter->second.help.end() && *nextEnd != ' '
                       && *nextEnd != '\t') {
                    nextEnd++;
                }
            }

            helpLines.push_back(iter->second.help.substr(
                wordBegin - iter->second.help.begin(), wordEnd - wordBegin));

            // Jump space
            while (wordEnd != iter->second.help.end()
                   && (*wordEnd == ' ' || *wordEnd == '\t')) {
                wordEnd++;
            }

            wordBegin = wordEnd;
        }

        // Print lines
        size_t maxLineNum = optionLines.size();

        if (maxLineNum < helpLines.size()) {
            maxLineNum = helpLines.size();
        }

        for (size_t i = 0; i < maxLineNum; i++) {
            ss << "    ";
            // Option
            size_t optionRemain = OPTION_WIDTH;

            if (i < optionLines.size()) {
                optionRemain -= optionLines[i].size();
                ss << optionLines[i];
            }

            for (size_t j = 0; j < optionRemain; j++) {
                ss << " ";
            }

            ss << "    ";

            // Help
            if (i < helpLines.size()) {
                ss << helpLines[i];
            }

            ss << ::std::endl;
        }
    }

    ::std::cout << ss.str();
}

/**
 * @brief       Parse arguments.
 */
bool Global::parseArgs(int &argc, char **&argv, int &exitCode)
{
    // Make description.
    ::std::ostringstream             shortArgs;
    ::std::unique_ptr<struct option> longArgs(
        new struct option[m_argMap.size() + 1]);
    int count = 0;

    for (auto iter = m_argMap.begin(); iter != m_argMap.end(); iter++) {
        shortArgs << iter->second.shortName;
        longArgs.get()[count] = {iter->second.longName.c_str(),
                                 [](const ::std::string &shortName) -> int {
                                     if (shortName.length() > 1) {
                                         return required_argument;

                                     } else {
                                         return no_argument;
                                     }
                                 }(iter->second.shortName),
                                 NULL, iter->first};
        count++;
        ;
    }

    longArgs.get()[count] = {NULL, 0, NULL, 0};
    int optIndex          = 0;

    // Parse arguments
    for (char c   = ::getopt_long(argc, argv, shortArgs.str().c_str(),
                                longArgs.get(), &optIndex);
         c > 0; c = ::getopt_long(argc, argv, shortArgs.str().c_str(),
                                  longArgs.get(), &optIndex)) {
        auto iter = m_argMap.find(c);

        if (iter == m_argMap.end()) {
            this->showHelp(argv[0]);
            exitCode = 1;
            return false;

        } else {
            if (! iter->second.handler()) {
                return false;
            }
        }
    }

    if (argc == optind + 1) {
        m_hasFileToOpen = true;
        m_fileToOpen    = QDir(argv[optind]).absolutePath();

    } else if (argc > optind + 1) {
        m_hasFileToOpen = false;
        ::std::cerr << "Too many files to open.";
        return false;

    } else {
        m_hasFileToOpen = false;
    }

    return true;
}
