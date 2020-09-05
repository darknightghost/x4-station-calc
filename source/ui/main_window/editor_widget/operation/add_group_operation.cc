#include <ui/main_window/editor_widget/operation/add_group_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::AddGroupOperation::AddGroupOperation(int           index,
                                                   EditorWidget *editorWidget) :
    OperationBase<EditorWidget::AddGroupOperation, int, EditorWidget *>(
        editorWidget),
    m_index(index)
{
    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::AddGroupOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();
    Q_ASSERT(editorWidget != nullptr);

    // Make group.
    ::std::shared_ptr<SaveGroup> saveGroup = SaveGroup::create();

    // Add to save.
    editorWidget->m_save->insertGroup(m_index, saveGroup);

    // Make item.
    GroupItem *      groupItem   = new GroupItem(saveGroup);
    GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);

    // Add to editor.
    editorWidget->m_itemGroups->insertChild(m_index, groupItem);
    editorWidget->m_treeEditor->setItemWidget(groupItem, 1, groupWidget);
    groupItem->setExpanded(true);

    editorWidget->connect(groupWidget, &GroupItemWidget::upBtnClicked,
                          editorWidget, &EditorWidget::onGroupMoveUp);
    editorWidget->connect(groupWidget, &GroupItemWidget::downBtnClicked,
                          editorWidget, &EditorWidget::onGroupMoveDown);
    editorWidget->connect(groupWidget, &GroupItemWidget::removeBtnClicked,
                          editorWidget, &EditorWidget::removeGroupItem);

    // Update.
    editorWidget->updateGroupMoveButtonStatus(groupItem);

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::AddGroupOperation::undoOperation()
{
    EditorWidget *editorWidget = this->editorWidget();
    Q_ASSERT(editorWidget != nullptr);

    GroupItem *groupItem
        = dynamic_cast<GroupItem *>(editorWidget->m_itemGroups->child(m_index));
    Q_ASSERT(groupItem != nullptr);

    // Remove from save.
    editorWidget->m_save->removeGroup(m_index);

    // Remove group item.
    editorWidget->m_itemGroups->removeChild(groupItem);

    // Update.
    if (editorWidget->m_itemGroups->childCount() > 0) {
        if (m_index == 0) {
            GroupItem *nextItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(0));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(nextItem);

        } else if (m_index >= editorWidget->m_itemGroups->childCount()) {
            GroupItem *prevItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(m_index - 1));
            Q_ASSERT(prevItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(prevItem);
        }
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::AddGroupOperation::~AddGroupOperation() {}
