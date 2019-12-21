#include <cstdlib>

#include <QtWidgets/QApplication>

#include <common.h>
#include <config.h>
#include <global.h>
#include <locale/string_table.h>
#include <ui/language_setting_dialog.h>

int firstRun()
{
    /// Select language
    LanguageSettingDialog langDlg;
    if (langDlg.exec() != QDialog::Accepted) {
        return 1;
    }

    /// Show license
    return 1;
}

int main(int argc, char *argv[])
{
    int          exitCode;
    int          fakeArgc   = 1;
    char *       fakeArgv[] = {argv[0], NULL};
    QApplication app(fakeArgc, fakeArgv);

    /// Initialize
    if (Global::initialize(argc, argv, exitCode) == nullptr) {
        return exitCode;
    }

    if (Config::initialize() == nullptr) {
        return 1;
    }

    if (StringTable::initialize() == nullptr) {
        return 1;
    }

    /// Check if it is the first time to run.
    if (Config::instance()->getBool("/firstRun", true)) {
        exitCode = firstRun();
        if (exitCode != 0) {
            return exitCode;
        }
        Config::instance()->setBool("/firstRun", false);
    }

    /// Show splash and load data.

    /// Show main window.

    return 0;
}
