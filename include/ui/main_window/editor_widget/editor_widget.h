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
#include <ui/main_window/editor_widget/operation.h>
#include <ui/main_window/editor_widget/warning_widget.h>
#include <ui/main_window/main_window.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QWidget {
    Q_OBJECT;

  private:
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
    QMap<GroupItem *, GroupItemWidget *> m_groupItems; ///< Group items.

    QTreeWidgetItem *m_itemSummary; ///< Summary.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	save			Save file.
     * @param[in]	editActions		Edit actions.
     * @param[in]	parent			Parent.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
                 MainWindow::EditActions *editActions,
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

  public:
    /**
     * @brief       Active editor widget.
     */
    void active();

    /**
     * @brief       Get group item by index.
     *
     * @param[in]   index   Index of the item.
     *
     * @return      Group item.
     */
    GroupItem *getGroupItemByIndex(int index);
};
