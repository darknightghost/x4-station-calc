#pragma once

#include <memory>

#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <common/generic_string.h>
#include <common/multi_threading.h>
#include <save/save.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/module_item.h>
#include <ui/main_window/editor_widget/warning_widget.h>
#include <ui/main_window/info_widget/info_widget.h>
#include <ui/main_window/main_window.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QWidget {
    Q_OBJECT;

  private:
    class Operation;
    template<typename T, typename... Args>
    class OperationBase;
    class AddGroupOperation;
    class AddModuleOperation;
    class ChangeModuleAmountOperation;
    class RemoveOperation;
    class RenameGroupOperation;

  private:
    /**
     * @brief   Module information.
     */
    struct ModuleInfo {
        ModuleItem *      moduleItem;   ///< Item.
        ModuleItemWidget *moduleWidget; ///< Widget.
    };

    /**
     * @brief   Group information.
     */
    struct GroupInfo {
        GroupItem *      groupItem;   ///< Group item.
        GroupItemWidget *groupWidget; ///< Group widget.
        QMap<ModuleItem *, ::std::shared_ptr<ModuleInfo>>
            moduleInfos; ///< Module informations.
        QMap<QString, ::std::shared_ptr<ModuleInfo>>
            moduleMacroMap; ///< Macro map.
    };

  private:
    StationModulesWidget *m_stationModulesWidget; ///< Station modules widget.
    InfoWidget *          m_infoWidget;           ///< Info widget.

    ::std::shared_ptr<Save>  m_save;            ///< Save file.
    int                      m_savedUndoCount;  ///< Undo stack size when saved.
    MainWindow::FileActions *m_fileActions;     ///< File actions.
    MainWindow::EditActions *m_editActions;     ///< Edit actions.
    BackgroundTask *         m_backgroundTasks; ///< Background tasks.

    QVBoxLayout *m_layout; ///< Layout.

    QVector<WarningWidget *> m_widgetsWarningInfos; ///< Warning informations.
    QTreeWidget *            m_treeEditor;          ///< Editor.

    // Operation stack.
    QVector<::std::shared_ptr<Operation>> m_undoStack; ///< Undo stack.
    QVector<::std::shared_ptr<Operation>> m_redoStack; ///< Redo stack.

    // Items
    QTreeWidgetItem *m_itemGroups; ///< Station module groups.
    QMap<GroupItem *, ::std::shared_ptr<GroupInfo>>
        m_groupItems; ///< Group items.

    QTreeWidgetItem *m_itemSummary; ///< Summary.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	save			        Save file.
     * @param[in]	fileActions		        File actions.
     * @param[in]	editActions		        Edit actions.
     * @param[in]	infoWidget              Info widget.
     * @param[in]	stationModulesWidget    Station modules widget.
     * @param[in]	parent			        Parent.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
                 MainWindow::FileActions *fileActions,
                 MainWindow::EditActions *editActions,
                 InfoWidget *             infoWidget,
                 StationModulesWidget *   stationModulesWidget,
                 QMdiSubWindow *          parent);

    /**
     * @brief	Destructors.
     */
    virtual ~EditorWidget();

  public:
    /**
     * @brief	Close save file.
     *
     * @return	\c true if the file closed, otherwise returns false.
     */
    bool closeSave();

    /**
     * @brief	Load groups.
     */
    void loadGroups();

  signals:
    /**
     * @brief       Emit when enable status of button "Add to Station" should
     *              be changed,
     *
     * @param[in]   status      Enable status.
     */
    void addToStationStatusChaged(bool status);

  private slots:
    /**
     * @brief		Do operation.
     *
     * @param[in]	operation		Operation.
     */
    void doOperation(::std::shared_ptr<Operation> operation);

    /**
     * @brief       Update window title.
     */
    void updateTitle();

    /**
     * @brief       Update button "Save" status.
     */
    void updateSaveStatus();

    /**
     * @brief       Update button "Add to Station" status.
     */
    void updateAddToStationStatus();

    /**
     * @brief       Update undo/redo action status.
     */
    void updateUndoRedoStatus();

    /**
     * @brief       Update cut/copy/remove action status.
     */
    void updateCutCopyRemoveStatus();

    /**
     * @brief       Update paste action status.
     */
    void updatePasteStatus();

  private:
    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    /**
     * @brief	    Add module.
     *
     * @param[in]   macros       Macros of the modules.
     */
    void addModules(const QStringList &macros);

    /**
     * @brief		Create new group.
     */
    void newGroup();

    /**
     * @brief		Undo.
     */
    void undo();

    /**
     * @brief		Redo.
     */
    void redo();

    /**
     * @brief		Cut.
     */
    void cut();

    /**
     * @brief		Copy.
     */
    void copy();

    /**
     * @brief		Paste.
     */
    void paste();

    /**
     * @brief		Remove.
     */
    void remove();

    /**
     * @brief		Remove group item.
     *
     * @param[in]   item    Item.
     */
    void removeGroupItem(GroupItem *item);

    /**
     * @brief		Remove module item.
     *
     * @param[in]   item    Item.
     */
    void removeModuleItem(ModuleItem *item);

    /**
     * @brief		Save.
     */
    void save();

    /**
     * @brief		Save as.
     */
    void saveAs();

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

  private slots:
    /**
     * @brief	Clear all warning informations.
     */
    void clearInfo();

    /**
     * @brief		Show error information.
     *
     * @param[in]	id		String id.
     */
    void addWarning(QString id);

    /**
     * @brief		Called when item changed.
     *
     * @param[in]	item	Item.
     * @param[in]	column	Column.
     */
    void onItemChanged(QTreeWidgetItem *item, int column);

    /**
     * @brief		Called when item double clicked.
     *
     * @param[in]	item	Item.
     * @param[in]	column	Column.
     */
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

    /**
     * @brief	    On amount changed.
     *
     * @param[in]   oldAmount       Old amount.
     * @param[in]   newAmount       New amount.
     * @param[in]	moduleItem	    Module item.
     */
    void onChangeAmount(quint64     oldAmount,
                        quint64     newAmount,
                        ModuleItem *moduleItem);

  public:
    /**
     * @brief       Active editor widget.
     */
    void active();
};

#include <ui/main_window/editor_widget/operation.h>
