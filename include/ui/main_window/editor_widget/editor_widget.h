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
#include <ui/main_window/editor_widget/operation.h>
#include <ui/main_window/editor_widget/warning_widget.h>
#include <ui/main_window/info_widget/info_widget.h>
#include <ui/main_window/main_window.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QWidget {
    Q_OBJECT;
    friend class RenameGroupOperation;

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
        QVector<::std::shared_ptr<ModuleInfo>>
            moduleInfos; ///< Module informations.
        QMap<QString, ::std::shared_ptr<ModuleInfo>>
            moduleMacroMap; ///< Macro map.
    };

  private:
    StationModulesWidget *m_stationModulesWidget; ///< Station modules widget.
    InfoWidget *          m_infoWidget;           ///< Info widget.

    ::std::shared_ptr<Save>  m_save;            ///< Save file.
    int                      m_savedUndoCount;  ///< Undo stack size when saved.
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
     * @param[in]	editActions		        Edit actions.
     * @param[in]	infoWidget              Info widget.
     * @param[in]	stationModulesWidget    Station modules widget.
     * @param[in]	parent			        Parent.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
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
     * @brief	Check save file.
     */
    void checkSave();

    /**
     * @brief	Load groups.
     */
    void loadGroups();

  private:
    /**
     * @brief		Do operation.
     *
     * @param[in]	operation		Operation.
     */
    void doOperation(::std::shared_ptr<Operation> operation);

    /**
     * @brief       Update newGroup action statis.
     */
    void updateNewGroup();

    /**
     * @brief       Update undo/redo action statis.
     */
    void updateUndoRedoStatus();

    /**
     * @brief       Update cut/copy action statis.
     */
    void updateCutCopyStatus();

    /**
     * @brief       Update paste action statis.
     */
    void updatePasteStatus();

    /**
     * @brief       Update remove action statis.
     */
    void updateRemoveStatus();

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
     * @param[in]   macro       Macro of the module.
     */
    void addModule(const QString &macro);

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

  public:
    /**
     * @brief       Active editor widget.
     */
    void active();
};
