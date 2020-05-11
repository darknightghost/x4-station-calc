#include <QtCore/QDebug>
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>
#include <QtGui/QMoveEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QWindowStateChangeEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <config.h>
#include <locale/string_table.h>
#include <open_file_listener.h>
#include <ui/main_window/language_menu.h>
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

    // Initialize windows size and position.
    QRect windowRect = QApplication::desktop()->geometry();
    windowRect.setSize(windowRect.size() / 2);
    windowRect.setX(windowRect.width() / 4);
    windowRect.setY(windowRect.height() / 4);
    this->setGeometry(windowRect);
    this->restoreGeometry(QByteArray::fromHex(
        Config::instance()
            ->getString("/MainWindow/geometry", this->saveGeometry().toHex())
            .toLocal8Bit()));

    // Window status.
    this->restoreState(QByteArray::fromHex(
        Config::instance()
            ->getString("/MainWindow/status", this->saveState().toHex())
            .toLocal8Bit()));

    // Initialize menus.
    this->initMenuToolBar();

    // Station modules widget
    m_stationModulesWidget
        = new StationModulesWidget(m_actionViewStationModules, this);
    this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,
                        m_stationModulesWidget, Qt::Orientation::Vertical);
    m_stationModulesWidget->setObjectName("stationModulesWidget");

    // Restore StationModulesWidget
    m_stationModulesWidget->restoreGeometry(QByteArray::fromHex(
        Config::instance()
            ->getString("/MainWindow/StationModulesWidget/geometry",
                        m_stationModulesWidget->saveGeometry().toHex())
            .toLocal8Bit()));
    this->restoreDockWidget(m_stationModulesWidget);

    // Information widget
    m_infoWidget = new InfoWidget(m_actionViewInfo, this);
    this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, m_infoWidget,
                        Qt::Orientation::Vertical);
    m_infoWidget->setObjectName("infoWidget");

    // Restore InfoWidget
    m_infoWidget->restoreGeometry(QByteArray::fromHex(
        Config::instance()
            ->getString("/MainWindow/InfoWidget/geometry",
                        m_infoWidget->saveGeometry().toHex())
            .toLocal8Bit()));
    this->restoreDockWidget(m_infoWidget);

    // Central widget
    m_centralWidget = new QTabWidget(this);
    this->setCentralWidget(m_centralWidget);

    // Set text.
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
void MainWindow::initMenuToolBar()
{
    // Main menu.
    m_mainMenu = new QMenuBar(this);
    this->setMenuBar(m_mainMenu);

    // File menu
    // Menu "File".
    m_menuFile = new QMenu(this);
    m_mainMenu->addMenu(m_menuFile);
    m_toolbarFile = new QToolBar(this);
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, m_toolbarFile);
    m_toolbarFile->setFloatable(false);
    m_toolbarFile->setObjectName("toolbarFile");

    // Menu "File->New".
    m_acionFileNew = new QAction(this);
    m_acionFileNew->setIcon(QIcon(":/Icons/FileNew.png"));
    m_acionFileNew->setShortcut(QKeySequence::New);
    m_menuFile->addAction(m_acionFileNew);
    m_toolbarFile->addAction(m_acionFileNew);

    // Menu "File->Open".
    m_acionFileOpen = new QAction(this);
    m_acionFileOpen->setIcon(QIcon(":/Icons/FileOpen.png"));
    m_acionFileOpen->setShortcut(QKeySequence::Open);
    m_menuFile->addAction(m_acionFileOpen);
    m_toolbarFile->addAction(m_acionFileOpen);

    m_menuFile->addSeparator();
    m_toolbarFile->addSeparator();

    // Menu "File->Save".
    m_acionFileSave = new QAction(this);
    m_acionFileSave->setIcon(QIcon(":/Icons/FileSave.png"));
    m_acionFileSave->setShortcut(QKeySequence::Save);
    m_menuFile->addAction(m_acionFileSave);
    m_toolbarFile->addAction(m_acionFileSave);

    // Menu "File->Save As".
    m_acionFileSaveAs = new QAction(this);
    m_acionFileSaveAs->setIcon(QIcon(":/Icons/FileSaveAs.png"));
    m_acionFileSaveAs->setShortcut(QKeySequence::SaveAs);
    m_menuFile->addAction(m_acionFileSaveAs);
    m_toolbarFile->addAction(m_acionFileSaveAs);

    m_menuFile->addSeparator();

    // Menu "File->Close".
    m_acionFileClose = new QAction(this);
    m_acionFileClose->setIcon(QIcon(":/Icons/FileClose.png"));
    m_acionFileClose->setShortcut(QKeySequence::Close);
    m_menuFile->addAction(m_acionFileClose);

    m_menuFile->addSeparator();

    // Menu "File->Exit".
    m_acionFileExit = new QAction(this);
    m_acionFileExit->setIcon(QIcon(":/Icons/FileExit.png"));
    m_acionFileExit->setShortcut(QKeySequence::Quit);
    m_menuFile->addAction(m_acionFileExit);
    this->connect(m_acionFileExit, &QAction::triggered, this,
                  &MainWindow::close);

    // Edit menu
    // Menu "Edit".
    m_menuEdit = new QMenu(this);
    m_mainMenu->addMenu(m_menuEdit);
    m_toolbarEdit = new QToolBar(this);
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, m_toolbarEdit);
    m_toolbarEdit->setFloatable(false);
    m_toolbarEdit->setObjectName("toolbarEdit");

    // Menu "Edit->New Group".
    m_acionEditNewGroup = new QAction(this);
    m_acionEditNewGroup->setIcon(QIcon(":/Icons/EditNewGroup.png"));
    m_acionEditNewGroup->setShortcut(QKeySequence("Ctrl+G"));
    m_menuEdit->addAction(m_acionEditNewGroup);
    m_toolbarEdit->addAction(m_acionEditNewGroup);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Undo".
    m_acionEditUndo = new QAction(this);
    m_acionEditUndo->setIcon(QIcon(":/Icons/EditUndo.png"));
    m_acionEditUndo->setShortcut(QKeySequence::Undo);
    m_menuEdit->addAction(m_acionEditUndo);
    m_toolbarEdit->addAction(m_acionEditUndo);

    // Menu "Edit->Redo".
    m_acionEditRedo = new QAction(this);
    m_acionEditRedo->setIcon(QIcon(":/Icons/EditRedo.png"));
    m_acionEditRedo->setShortcut(QKeySequence::Redo);
    m_menuEdit->addAction(m_acionEditRedo);
    m_toolbarEdit->addAction(m_acionEditRedo);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Cut".
    m_acionEditCut = new QAction(this);
    m_acionEditCut->setIcon(QIcon(":/Icons/EditCut.png"));
    m_acionEditCut->setShortcut(QKeySequence::Cut);
    m_menuEdit->addAction(m_acionEditCut);
    m_toolbarEdit->addAction(m_acionEditCut);

    // Menu "Edit->Copy".
    m_acionEditCopy = new QAction(this);
    m_acionEditCopy->setIcon(QIcon(":/Icons/EditCopy.png"));
    m_acionEditCopy->setShortcut(QKeySequence::Copy);
    m_menuEdit->addAction(m_acionEditCopy);
    m_toolbarEdit->addAction(m_acionEditCopy);

    // Menu "Edit->Paste".
    m_acionEditPaste = new QAction(this);
    m_acionEditPaste->setIcon(QIcon(":/Icons/EditPaste.png"));
    m_acionEditPaste->setShortcut(QKeySequence::Paste);
    m_menuEdit->addAction(m_acionEditPaste);
    m_toolbarEdit->addAction(m_acionEditPaste);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Remove".
    m_acionEditRemove = new QAction(this);
    m_acionEditRemove->setIcon(QIcon(":/Icons/EditRemove.png"));
    m_acionEditRemove->setShortcut(QKeySequence("Ctrl+R"));
    m_menuEdit->addAction(m_acionEditRemove);
    m_toolbarEdit->addAction(m_acionEditRemove);

    // Setting menu
    // Menu "Settings".
    m_menuSettings = new QMenu(this);
    m_mainMenu->addMenu(m_menuSettings);

    // Menu "Settings->Language".
    m_menuSettingsLanguage = new LanguageMenu(m_menuSettings);
    m_menuSettings->addMenu(m_menuSettingsLanguage);
    m_menuSettings->addSeparator();

    // Menu "Settings->Game Path".
    m_actionSetttingGamePath = new QAction(this);
    m_menuSettings->addAction(m_actionSetttingGamePath);

    // View menu
    // Menu "View".
    m_menuView = new QMenu(this);
    m_mainMenu->addMenu(m_menuView);

    // Menu "View->Station Modules".
    m_actionViewStationModules = new QAction(this);
    m_menuView->addAction(m_actionViewStationModules);

    // Menu "View->Info".
    m_actionViewInfo = new QAction(this);
    m_menuView->addAction(m_actionViewInfo);

    // Help menu
    // Menu "Help".
    m_menuHelp = new QMenu(this);
    m_mainMenu->addMenu(m_menuHelp);

    // Menu "Help->About".
    m_helpAbout = new QAction(this);
    m_menuHelp->addAction(m_helpAbout);
    m_menuHelp->addSeparator();

    // Menu "Help->Check Update".
    m_helpCheckUpdate = new QAction(this);
    m_menuHelp->addAction(m_helpCheckUpdate);
}

/**
 * @brief		Close event.
 *
 * @param[in]	event		Event.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save status.
    Config::instance()->setString("/MainWindow/geometry",
                                  this->saveGeometry().toHex());
    Config::instance()->setString("/MainWindow/status",
                                  this->saveState().toHex());
    Config::instance()->setString(
        "/MainWindow/StationModulesWidget/geometry",
        m_stationModulesWidget->saveGeometry().toHex());
    Config::instance()->setString("/MainWindow/InfoWidget/geometry",
                                  m_infoWidget->saveGeometry().toHex());

    // Station module widget
    m_stationModulesWidget->enableClose();
    m_stationModulesWidget->close();

    // Info widget
    m_infoWidget->enableClose();
    m_infoWidget->close();

    event->accept();
}

/**
 * @brief		Open file.
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

    // Toolbar "File".
    m_toolbarFile->setWindowTitle(STR("STR_TOOLBAR_FILE"));

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

    // Toolbar "Edit".
    m_toolbarEdit->setWindowTitle(STR("STR_TOOLBAR_EDIT"));

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
