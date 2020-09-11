#pragma once

#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>

#include <ui/main_window/info_widget/info_widget.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>
#include <update_checker.h>

/**
 * @brief	Main window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    /**
     * @brief		Actions in file menu.
     */
    struct FileActions {
        QAction *actionFileNew;    ///< Menu "File->New".
        QAction *actionFileOpen;   ///< Menu "File->Open".
        QAction *actionFileSave;   ///< Menu "File->Save".
        QAction *actionFileSaveAs; ///< Menu "File->Save As".
        QAction *actionFileClose;  ///< Menu "File->Close".
        QAction *actionFileExit;   ///< Menu "File->Exit".
    };

    /**
     * @brief		Actions in edit menu.
     */
    struct EditActions {
        QAction *actionEditNewGroup; ///< Menu "Edit->New Group".
        QAction *actionEditUndo;     ///< Menu "Edit->Undo".
        QAction *actionEditRedo;     ///< Menu "Edit->Redo".
        QAction *actionEditCut;      ///< Menu "Edit->Cut".
        QAction *actionEditCopy;     ///< Menu "Edit->Copy".
        QAction *actionEditPaste;    ///< Menu "Edit->Paste".
        QAction *actionEditRemove;   ///< Menu "Edit->Remove".
    };

  private:
    // Menu
    QMenuBar *m_mainMenu; ///< Main menu.

    // File menu
    QMenu *     m_menuFile;    ///< Menu "File".
    QToolBar *  m_toolbarFile; ///< Toolbar "File".
    FileActions m_fileActions; ///< File actions.

    // Edit menu
    QMenu *     m_menuEdit;    ///< Menu "Edit".
    QToolBar *  m_toolbarEdit; ///< Toolbar "Edit".
    EditActions m_editActions; ///< Edit actions.

    // Setting menu
    QMenu *  m_menuSettings;           ///< Menu "Settings".
    QMenu *  m_menuSettingsLanguage;   ///< Menu "Settings->Language".
    QAction *m_actionSetttingGamePath; ///< Menu "Settings->Game Path".

    // View menu
    QMenu *  m_menuView;                 ///< Menu "View".
    QAction *m_actionViewStationModules; ///< Menu "View->Station Modules".
    QAction *m_actionViewInfo;           ///< Menu "View->Info".

    // Help menu
    QMenu *  m_menuHelp;        ///< Menu "Help".
    QAction *m_helpAbout;       ///< Menu "Help->About".
    QAction *m_helpCheckUpdate; ///< Menu "Help->Check Update".

    // Dock widgets
    // Station module widget.
    StationModulesWidget *m_stationModulesWidget; ///< Station modules widget.

    // Info widget.
    InfoWidget *m_infoWidget; ///< Information widget.

    // Central widget
    QMdiArea *m_centralWidget; ///< Central widget.

    // Update checker.
    UpdateChecker *m_updateChecker; ///< Update checker.

  public:
    /**
     * @brief		Constructor of main window.
     */
    MainWindow();

    /**
     * @brief		Destructor of main window.
     */
    virtual ~MainWindow();

  private:
    /**
     * @brief	Initialize Menu.
     */
    void initMenuToolBar();

    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  private slots:
    /**
     * @brief		Open file.
     *
     * @param[in]	path		Path of file.
     */
    void open(QString path);

    /**
     * @brief		Create new file.
     */
    void newAction();

    /**
     * @brief		Open file.
     */
    void openAction();

    /**
     * @brief		Active window.
     */
    void active();

    /**
     * @brief       Editor window activated.
     *
     * @param[in]   window      Window actived.
     */
    void editorActived(QMdiSubWindow *window);

    /**
     * @brief		Ask game path.
     */
    void askGamePath();

    /**
     * @brief		Open about dialog.
     */
    void onAboutDialog();

  protected slots:
    /**
     * @brief		Change language.
     */
    void onLanguageChanged();
};
