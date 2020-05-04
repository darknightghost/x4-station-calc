#include <QtCore/QDebug>

#include <config.h>
#include <locale/string_table.h>
#include <open_file_listener.h>
#include <ui/main_window/main_window.h>

/**
 * @brief		Constructor of main window.
 */
MainWindow::MainWindow() : QMainWindow(nullptr)
{
    // Listen file open events.
    this->connect(OpenFileListener::instance().get(),
                  &OpenFileListener::openFile, this, &MainWindow::open,
                  Qt::ConnectionType::QueuedConnection);
    this->connect(OpenFileListener::instance().get(), &OpenFileListener::active,
                  this, &MainWindow::active,
                  Qt::ConnectionType::QueuedConnection);
    OpenFileListener::instance()->unblock();

    // Initialize menus
    this->initMenu();

    // Set text
    this->onLanguageChanged();
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &MainWindow::onLanguageChanged);
}

/**
 * @brief		Destructor of main window.
 */
MainWindow::~MainWindow() {}

/**
 * @brief	Initialize Menu.
 */
void MainWindow::initMenu()
{
    // Main menu.
    m_mainMenu = new QMenuBar();
    this->setMenuBar(m_mainMenu);

    // File menu
    // Menu "File".
    m_menuFile = new QMenu();
    m_mainMenu->addMenu(m_menuFile);

    // Menu "File->New".
    m_acionFileNew = new QAction();
    m_menuFile->addAction(m_acionFileNew);

    // Menu "File->Open".
    m_acionFileOpen = new QAction();
    m_menuFile->addAction(m_acionFileOpen);
    m_menuFile->addSeparator();

    // Menu "File->Save".
    m_acionFileSave = new QAction();
    m_menuFile->addAction(m_acionFileSave);

    // Menu "File->Save As".
    m_acionFileSaveAs = new QAction();
    m_menuFile->addAction(m_acionFileSaveAs);
    m_menuFile->addSeparator();

    // Menu "File->Close".
    m_acionFileClose = new QAction();
    m_menuFile->addAction(m_acionFileClose);
    m_menuFile->addSeparator();

    // Menu "File->Exit".
    m_acionFileExit = new QAction();
    m_menuFile->addAction(m_acionFileExit);
    this->connect(m_acionFileExit, &QAction::toggled, this, &MainWindow::close);

    // Edit menu
    // Menu "Edit".
    m_menuEdit = new QMenu();
    m_mainMenu->addMenu(m_menuEdit);

    // Menu "Edit->New Group".
    m_acionEditNewGroup = new QAction();
    m_menuEdit->addAction(m_acionEditNewGroup);
    m_menuEdit->addSeparator();

    // Menu "Edit->Undo".
    m_acionEditUndo = new QAction();
    m_menuEdit->addAction(m_acionEditUndo);

    // Menu "Edit->Redo".
    m_acionEditRedo = new QAction();
    m_menuEdit->addAction(m_acionEditRedo);
    m_menuEdit->addSeparator();

    // Menu "Edit->Cut".
    m_acionEditCut = new QAction();
    m_menuEdit->addAction(m_acionEditCut);

    // Menu "Edit->Copy".
    m_acionEditCopy = new QAction();
    m_menuEdit->addAction(m_acionEditCopy);

    // Menu "Edit->Paste".
    m_acionEditPaste = new QAction();
    m_menuEdit->addAction(m_acionEditPaste);
    m_menuEdit->addSeparator();

    // Menu "Edit->Remove".
    m_acionEditRemove = new QAction();
    m_menuEdit->addAction(m_acionEditRemove);

    // Setting menu
    // Menu "Settings".
    m_menuSettings = new QMenu();
    m_mainMenu->addMenu(m_menuSettings);

    // Menu "Settings->Language".
    m_menuSettingsLanguage = new QMenu();
    m_menuSettings->addMenu(m_menuSettingsLanguage);
    m_menuSettings->addSeparator();

    // Menu "Settings->Game Path".
    m_actionSetttingGamePath = new QAction();
    m_menuSettings->addAction(m_actionSetttingGamePath);

    // View menu
    // Menu "View".
    m_menuView = new QMenu();
    m_mainMenu->addMenu(m_menuView);

    // Menu "View->Station Modules".
    m_actionViewStationModules = new QAction();
    m_menuView->addAction(m_actionViewStationModules);

    // Menu "View->Info".
    m_actionViewInfo = new QAction();
    m_menuView->addAction(m_actionViewInfo);

    // Help menu
    // Menu "Help".
    m_menuHelp = new QMenu();
    m_mainMenu->addMenu(m_menuHelp);

    // Menu "Help->About".
    m_helpAbout = new QAction();
    m_menuHelp->addAction(m_helpAbout);
    m_menuHelp->addSeparator();

    // Menu "Help->Check Update".
    m_helpCheckUpdate = new QAction();
    m_menuHelp->addAction(m_helpCheckUpdate);
}

/**
 * @brief		Open file.
 *
 * @param[in]	path		Path of file.
 */
void MainWindow::open(QString path)
{
    qDebug() << path;
}

/**
 * @brief		Active window.
 */
void MainWindow::active()
{
    this->activateWindow();
}

/**
 * @brief		Change language.
 */
void MainWindow::onLanguageChanged()
{
    // Title
    this->setWindowTitle(STR("STR_TITLE"));

    // Menu
    // File menu
    // Menu "File".
    m_menuFile->setTitle(STR("STR_MENU_FILE"));

    // Menu "File->New".
    m_acionFileNew->setText(STR("STR_MENU_FILE_NEW"));

    // Menu "File->Open".
    m_acionFileOpen->setText(STR("STR_MENU_FILE_OPEN"));

    // Menu "File->Save".
    m_acionFileSave->setText(STR("STR_MENU_FILE_SAVE"));

    // Menu "File->Save As".
    m_acionFileSaveAs->setText(STR("STR_MENU_FILE_SAVE_AS"));

    // Menu "File->Close".
    m_acionFileClose->setText(STR("STR_MENU_FILE_CLOSE"));

    // Menu "File->Exit".
    m_acionFileExit->setText(STR("STR_MENU_FILE_EXIT"));

    // Edit menu
    // Menu "Edit".
    m_menuEdit->setTitle(STR("STR_MENU_EDIT"));

    // Menu "Edit->New Group".
    m_acionEditNewGroup->setText(STR("STR_MENU_EDIT_NEW_GROUP"));

    // Menu "Edit->Undo".
    m_acionEditUndo->setText(STR("STR_MENU_EDIT_UNDO"));

    // Menu "Edit->Redo".
    m_acionEditRedo->setText(STR("STR_MENU_EDIT_REDO"));

    // Menu "Edit->Cut".
    m_acionEditCut->setText(STR("STR_MENU_EDIT_CUT"));

    // Menu "Edit->Copy".
    m_acionEditCopy->setText(STR("STR_MENU_EDIT_COPY"));

    // Menu "Edit->Paste".
    m_acionEditPaste->setText(STR("STR_MENU_EDIT_PASTE"));

    // Menu "Edit->Remove".
    m_acionEditRemove->setText(STR("STR_MENU_EDIT_REMOVE"));

    // Setting menu
    // Menu "Settings".
    m_menuSettings->setTitle(STR("STR_MENU_SETTINGS"));

    // Menu "Settings->Language".
    m_menuSettingsLanguage->setTitle(STR("STR_MENU_SETTINGS_LANGUAGE"));

    // Menu "Settings->Game Path".
    m_actionSetttingGamePath->setText(STR("STR_MENU_SETTINGS_GAME_PATH"));

    // View menu
    // Menu "View".
    m_menuView->setTitle(STR("STR_MENU_VIEW"));

    // Menu "View->Station Modules".
    m_actionViewStationModules->setText(STR("STR_MENU_VIEW_STATION_MODULES"));

    // Menu "View->Info".
    m_actionViewInfo->setText(STR("STR_MENU_VIEW_INFO"));

    // Help menu
    // Menu "Help".
    m_menuHelp->setTitle(STR("STR_MENU_HELP"));

    // Menu "Help->About".
    m_helpAbout->setText(STR("STR_MENU_HELP_ABOUT"));

    // Menu "Help->Check Update".
    m_helpCheckUpdate->setText(STR("STR_MENU_HELP_CHECK_UPDATE"));
}
