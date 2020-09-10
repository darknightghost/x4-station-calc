#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>
#include <QtGui/QMoveEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QWindowStateChangeEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <locale/string_table.h>
#include <open_file_listener.h>
#include <ui/about_dialog.h>
#include <ui/main_window/editor_widget/editor_widget.h>
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

    // Set icon
    this->setWindowIcon(QIcon(":/Icons/MainIcon.png"));

    // Set style sheet
    QFile styleFile(":/StyleSheet/main_window.qss");
    styleFile.open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::Text);
    this->setStyleSheet(styleFile.readAll());

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
    m_stationModulesWidget->setAddToStationStatus(false);

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
    this->connect(m_stationModulesWidget,
                  &StationModulesWidget::stationModuleClicked, m_infoWidget,
                  &InfoWidget::showStationModuleInfo);

    // Restore InfoWidget
    m_infoWidget->restoreGeometry(QByteArray::fromHex(
        Config::instance()
            ->getString("/MainWindow/InfoWidget/geometry",
                        m_infoWidget->saveGeometry().toHex())
            .toLocal8Bit()));
    this->restoreDockWidget(m_infoWidget);

    // Central widget
    m_centralWidget = new QMdiArea(this);
    m_centralWidget->setViewMode(QMdiArea::TabbedView);
    m_centralWidget->setTabPosition(QTabWidget::TabPosition::North);
    m_centralWidget->setTabsClosable(true);
    m_centralWidget->setTabsMovable(true);
    m_centralWidget->setDocumentMode(true);
    this->setCentralWidget(m_centralWidget);
    this->connect(m_centralWidget, &QMdiArea::subWindowActivated, this,
                  &MainWindow::editorActived);

    // Set text.
    this->onLanguageChanged();
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &MainWindow::onLanguageChanged);

    // Open file listener.
    this->connect(OpenFileListener::instance().get(),
                  &OpenFileListener::openFile, this, &MainWindow::open);
    this->connect(OpenFileListener::instance().get(), &OpenFileListener::active,
                  this, &MainWindow::active);
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
    m_fileActions.actionFileNew = new QAction(this);
    m_fileActions.actionFileNew->setIcon(QIcon(":/Icons/FileNew.png"));
    m_fileActions.actionFileNew->setShortcut(QKeySequence::New);
    m_menuFile->addAction(m_fileActions.actionFileNew);
    m_toolbarFile->addAction(m_fileActions.actionFileNew);
    this->connect(m_fileActions.actionFileNew, &QAction::triggered, this,
                  &MainWindow::newAction);

    // Menu "File->Open".
    m_fileActions.actionFileOpen = new QAction(this);
    m_fileActions.actionFileOpen->setIcon(QIcon(":/Icons/FileOpen.png"));
    m_fileActions.actionFileOpen->setShortcut(QKeySequence::Open);
    m_menuFile->addAction(m_fileActions.actionFileOpen);
    m_toolbarFile->addAction(m_fileActions.actionFileOpen);
    this->connect(m_fileActions.actionFileOpen, &QAction::triggered, this,
                  &MainWindow::openAction);

    m_menuFile->addSeparator();
    m_toolbarFile->addSeparator();

    // Menu "File->Save".
    m_fileActions.actionFileSave = new QAction(this);
    m_fileActions.actionFileSave->setIcon(QIcon(":/Icons/FileSave.png"));
    m_fileActions.actionFileSave->setShortcut(QKeySequence::Save);
    m_fileActions.actionFileSave->setEnabled(false);
    m_menuFile->addAction(m_fileActions.actionFileSave);
    m_toolbarFile->addAction(m_fileActions.actionFileSave);

    // Menu "File->Save As".
    m_fileActions.actionFileSaveAs = new QAction(this);
    m_fileActions.actionFileSaveAs->setIcon(QIcon(":/Icons/FileSaveAs.png"));
    m_fileActions.actionFileSaveAs->setShortcut(QKeySequence::SaveAs);
    m_fileActions.actionFileSaveAs->setEnabled(false);
    m_menuFile->addAction(m_fileActions.actionFileSaveAs);
    m_toolbarFile->addAction(m_fileActions.actionFileSaveAs);

    m_menuFile->addSeparator();

    // Menu "File->Close".
    m_fileActions.actionFileClose = new QAction(this);
    m_fileActions.actionFileClose->setIcon(QIcon(":/Icons/FileClose.png"));
    m_fileActions.actionFileClose->setShortcut(QKeySequence::Close);
    m_fileActions.actionFileClose->setEnabled(false);
    m_menuFile->addAction(m_fileActions.actionFileClose);

    m_menuFile->addSeparator();

    // Menu "File->Exit".
    m_fileActions.actionFileExit = new QAction(this);
    m_fileActions.actionFileExit->setIcon(QIcon(":/Icons/FileExit.png"));
    m_fileActions.actionFileExit->setShortcut(QKeySequence::Quit);
    m_menuFile->addAction(m_fileActions.actionFileExit);
    this->connect(m_fileActions.actionFileExit, &QAction::triggered, this,
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
    m_editActions.actionEditNewGroup = new QAction(this);
    m_editActions.actionEditNewGroup->setIcon(
        QIcon(":/Icons/EditNewGroup.png"));
    m_editActions.actionEditNewGroup->setShortcut(QKeySequence("Ctrl+G"));
    m_editActions.actionEditNewGroup->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditNewGroup);
    m_toolbarEdit->addAction(m_editActions.actionEditNewGroup);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Undo".
    m_editActions.actionEditUndo = new QAction(this);
    m_editActions.actionEditUndo->setIcon(QIcon(":/Icons/EditUndo.png"));
    m_editActions.actionEditUndo->setShortcut(QKeySequence::Undo);
    m_editActions.actionEditUndo->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditUndo);
    m_toolbarEdit->addAction(m_editActions.actionEditUndo);

    // Menu "Edit->Redo".
    m_editActions.actionEditRedo = new QAction(this);
    m_editActions.actionEditRedo->setIcon(QIcon(":/Icons/EditRedo.png"));
    m_editActions.actionEditRedo->setShortcut(QKeySequence::Redo);
    m_editActions.actionEditRedo->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditRedo);
    m_toolbarEdit->addAction(m_editActions.actionEditRedo);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Cut".
    m_editActions.actionEditCut = new QAction(this);
    m_editActions.actionEditCut->setIcon(QIcon(":/Icons/EditCut.png"));
    m_editActions.actionEditCut->setShortcut(QKeySequence::Cut);
    m_editActions.actionEditCut->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditCut);
    m_toolbarEdit->addAction(m_editActions.actionEditCut);

    // Menu "Edit->Copy".
    m_editActions.actionEditCopy = new QAction(this);
    m_editActions.actionEditCopy->setIcon(QIcon(":/Icons/EditCopy.png"));
    m_editActions.actionEditCopy->setShortcut(QKeySequence::Copy);
    m_editActions.actionEditCopy->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditCopy);
    m_toolbarEdit->addAction(m_editActions.actionEditCopy);

    // Menu "Edit->Paste".
    m_editActions.actionEditPaste = new QAction(this);
    m_editActions.actionEditPaste->setIcon(QIcon(":/Icons/EditPaste.png"));
    m_editActions.actionEditPaste->setShortcut(QKeySequence::Paste);
    m_editActions.actionEditPaste->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditPaste);
    m_toolbarEdit->addAction(m_editActions.actionEditPaste);

    m_menuEdit->addSeparator();
    m_toolbarEdit->addSeparator();

    // Menu "Edit->Remove".
    m_editActions.actionEditRemove = new QAction(this);
    m_editActions.actionEditRemove->setIcon(QIcon(":/Icons/EditRemove.png"));
    m_editActions.actionEditRemove->setShortcut(QKeySequence("Ctrl+R"));
    m_editActions.actionEditRemove->setEnabled(false);
    m_menuEdit->addAction(m_editActions.actionEditRemove);
    m_toolbarEdit->addAction(m_editActions.actionEditRemove);

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
    this->connect(m_actionSetttingGamePath, &QAction::triggered, this,
                  &MainWindow::askGamePath);

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
    this->connect(m_helpAbout, &QAction::triggered, this,
                  &MainWindow::onAboutDialog);
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
    // Close all opend files.
    for (auto &subWindow : m_centralWidget->subWindowList()) {
        EditorWidget *editorWidget
            = dynamic_cast<EditorWidget *>(subWindow->widget());
        Q_ASSERT(editorWidget != nullptr);

        if (editorWidget->closeSave()) {
            editorWidget->close();
            subWindow->close();

        } else {
            event->ignore();
            return;
        }
    }

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
    EditorWidget *editorWidget = EditorWidget::getEditorWidgetByPath(path);
    if (editorWidget == nullptr) {
        ::std::shared_ptr<Save> save = Save::load(path);
        if (save == nullptr) {
            QMessageBox::critical(this, STR("STR_ERROR"),
                                  STR("STR_FAILED_OPEN_FILE").arg(path));

        } else {
            QMdiSubWindow *container = new QMdiSubWindow();
            m_centralWidget->addSubWindow(container);
            editorWidget = new EditorWidget(save, &m_fileActions,
                                            &m_editActions, m_infoWidget,
                                            m_stationModulesWidget, container);
            this->connect(editorWidget, &EditorWidget::addToStationStatusChaged,
                          m_stationModulesWidget,
                          &StationModulesWidget::setAddToStationStatus);
            this->connect(editorWidget, &EditorWidget::filterByProduct,
                          m_stationModulesWidget,
                          &StationModulesWidget::onFilterByProduct);
            this->connect(editorWidget, &EditorWidget::filterByResource,
                          m_stationModulesWidget,
                          &StationModulesWidget::onFilterByResource);
            m_centralWidget->setActiveSubWindow(container);
            editorWidget->show();
        }
    } else {
        QMdiSubWindow *container
            = dynamic_cast<QMdiSubWindow *>(editorWidget->parent());
        Q_ASSERT(container != nullptr);
        m_centralWidget->setActiveSubWindow(container);
        editorWidget->show();
    }
}

/**
 * @brief		Create new file.
 */
void MainWindow::newAction()
{
    ::std::shared_ptr<Save> save      = Save::create();
    QMdiSubWindow *         container = new QMdiSubWindow();
    m_centralWidget->addSubWindow(container);
    EditorWidget *editorWidget
        = new EditorWidget(save, &m_fileActions, &m_editActions, m_infoWidget,
                           m_stationModulesWidget, container);
    this->connect(editorWidget, &EditorWidget::addToStationStatusChaged,
                  m_stationModulesWidget,
                  &StationModulesWidget::setAddToStationStatus);
    this->connect(editorWidget, &EditorWidget::filterByProduct,
                  m_stationModulesWidget,
                  &StationModulesWidget::onFilterByProduct);
    this->connect(editorWidget, &EditorWidget::filterByResource,
                  m_stationModulesWidget,
                  &StationModulesWidget::onFilterByResource);
    m_centralWidget->setActiveSubWindow(container);
    editorWidget->show();
}

/**
 * @brief		Open file.
 */
void MainWindow::openAction()
{
    QString path = QFileDialog::getOpenFileName(
        this, STR("STR_OPEN_STATION"),
        Config::instance()->getString("/openPath", QDir::homePath()),
        STR("STR_SAVE_FILE_FILTER"));
    if (path != "") {
        QString dir = QDir(path).absolutePath();
        dir         = dir.left(dir.lastIndexOf("/"));
        Config::instance()->setString("/openPath", dir);
        this->open(path);
    }
}

/**
 * @brief		Active window.
 */
void MainWindow::active()
{
    this->activateWindow();
}

/**
 * @brief       Editor window activated.
 */
void MainWindow::editorActived(QMdiSubWindow *window)
{
    if (window == nullptr) {
        m_stationModulesWidget->setAddToStationStatus(false);
        m_fileActions.actionFileSave->setEnabled(false);
        m_fileActions.actionFileSaveAs->setEnabled(false);
        m_fileActions.actionFileClose->setEnabled(false);
        m_editActions.actionEditNewGroup->setEnabled(false);

    } else {
        static_cast<EditorWidget *>(window->widget())->active();
    }
}

/**
 * @brief		Ask game path.
 */
void MainWindow::askGamePath()
{
    while (true) {
        QFileDialog fileDialog(nullptr, STR("STR_TITLE_SELECT_GAME_PATH"),
                               Config::instance()->getString("/gamePath", ""),
                               "*");
        fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::FileMode::Directory);
        fileDialog.setFilter(QDir::Filter::Dirs | QDir::Filter::Hidden
                             | QDir::Filter::System);
        if (fileDialog.exec() != QDialog::DialogCode::Accepted
            || fileDialog.selectedFiles().empty()) {
            return;
        }
        QString str = QDir(fileDialog.selectedFiles()[0]).absolutePath();
        qDebug() << "Selected:" << str;

        if (GameData::instance()->checkGamePath(str)) {
            Config::instance()->setString("/gamePath", str);
            QMessageBox::information(this, STR("STR_INFO"),
                                     STR("STR_INFO_EFFECT_NEXT_LAUNCH"));
            return;
        } else {
            QMessageBox::critical(this, STR("STR_ERROR"),
                                  STR("STR_INFO_ILLEGAL_GAME_PATH"));
        }
    }
}

/**
 * @brief		Open about dialog.
 */
void MainWindow::onAboutDialog()
{
    AboutDialog aboutDialog;

    aboutDialog.exec();
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
    m_fileActions.actionFileNew->setText(STR("STR_MENU_FILE_NEW"));

    // Menu "File->Open".
    m_fileActions.actionFileOpen->setText(STR("STR_MENU_FILE_OPEN"));

    // Menu "File->Save".
    m_fileActions.actionFileSave->setText(STR("STR_MENU_FILE_SAVE"));

    // Menu "File->Save As".
    m_fileActions.actionFileSaveAs->setText(STR("STR_MENU_FILE_SAVE_AS"));

    // Menu "File->Close".
    m_fileActions.actionFileClose->setText(STR("STR_MENU_FILE_CLOSE"));

    // Menu "File->Exit".
    m_fileActions.actionFileExit->setText(STR("STR_MENU_FILE_EXIT"));

    // Edit menu
    // Menu "Edit".
    m_menuEdit->setTitle(STR("STR_MENU_EDIT"));

    // Toolbar "Edit".
    m_toolbarEdit->setWindowTitle(STR("STR_TOOLBAR_EDIT"));

    // Menu "Edit->New Group".
    m_editActions.actionEditNewGroup->setText(STR("STR_MENU_EDIT_NEW_GROUP"));

    // Menu "Edit->Undo".
    m_editActions.actionEditUndo->setText(STR("STR_MENU_EDIT_UNDO"));

    // Menu "Edit->Redo".
    m_editActions.actionEditRedo->setText(STR("STR_MENU_EDIT_REDO"));

    // Menu "Edit->Cut".
    m_editActions.actionEditCut->setText(STR("STR_MENU_EDIT_CUT"));

    // Menu "Edit->Copy".
    m_editActions.actionEditCopy->setText(STR("STR_MENU_EDIT_COPY"));

    // Menu "Edit->Paste".
    m_editActions.actionEditPaste->setText(STR("STR_MENU_EDIT_PASTE"));

    // Menu "Edit->Remove".
    m_editActions.actionEditRemove->setText(STR("STR_MENU_EDIT_REMOVE"));

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
