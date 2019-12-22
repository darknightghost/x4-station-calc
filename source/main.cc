#include <cstdlib>

#include <QtCore/QTextCodec>
#include <QtWidgets/QApplication>

#include <common.h>
#include <config.h>
#include <game_data/game_data.h>
#include <global.h>
#include <locale/string_table.h>
#include <ui/language_setting_dialog.h>
#include <ui/license_dialog.h>
#include <ui/splash/splash_widget.h>

int firstRun()
{
    /// Select language
    LanguageSettingDialog langDlg;
    if (langDlg.exec() != QDialog::DialogCode::Accepted) {
        return 1;
    }

    /// Show license
    LicenseDialog licenseDlg;
    if (licenseDlg.exec() != QDialog::DialogCode::Accepted) {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /// Force UTF-8.
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    int          exitCode;
    int          fakeArgc   = 1;
    char *       fakeArgv[] = {argv[0], NULL};
    QApplication app(fakeArgc, fakeArgv);

    /// Initialize.
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
    SplashWidget splash;
    splash.exec([&]() -> int {
        /// Load game data.
        if (GameData::initialize(::std::bind(&SplashWidget::setText, &splash,
                                             ::std::placeholders::_1))
            == nullptr) {
            return 1;
        } else {
            return 0;
        }
    });

    /// Show main window.

    return 0;
}
