#include <ui/main_window/editor_widget/operation/move_group_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::MoveGroupOperation::MoveGroupOperation(
    int oldIndex, int newIndex, EditorWidget *editorWidget) :
    OperationBase<EditorWidget::MoveGroupOperation, int, int, EditorWidget *>(
        editorWidget),
    m_oldIndex(oldIndex), m_newIndex(newIndex)
{
    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::MoveGroupOperation::doOperation()
{
    // Editor Widget.
    EditorWidget *editorWidget = dynamic_cast<EditorWidget *>(m_editorWidget);
    Q_ASSERT(editorWidget != nullptr);

    // Take group item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->takeChild(m_oldIndex));
    Q_ASSERT(groupItem != nullptr);

    // Group info.
    ::std::shared_ptr<GroupInfo> groupInfo
        = editorWidget->m_groupItems[groupItem];

    // Insert group item.
    editorWidget->m_itemGroups->insertChild(m_newIndex, groupItem);
    editorWidget->m_treeEditor->setItemWidget(groupItem, 1,
                                              groupInfo->groupWidget);

    // Set index.
    editorWidget->m_save->setIndex(m_oldIndex, m_newIndex);

    // Update.
    editorWidget->updateGroupMoveButtonStatus(groupItem);

    if (editorWidget->m_itemGroups->childCount() > 0) {
        if (m_oldIndex == 0) {
            GroupItem *nextItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(0));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(nextItem);

        } else if (m_oldIndex >= editorWidget->m_itemGroups->childCount()) {
            GroupItem *prevItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(m_oldIndex - 1));
            Q_ASSERT(prevItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(prevItem);
        }
    }

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::MoveGroupOperation::undoOperation()
{
    // Editor Widget.
    EditorWidget *editorWidget = dynamic_cast<EditorWidget *>(m_editorWidget);
    Q_ASSERT(editorWidget != nullptr);

    // Take group item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->takeChild(m_newIndex));
    Q_ASSERT(groupItem != nullptr);

    // Group info.
    ::std::shared_ptr<GroupInfo> groupInfo
        = editorWidget->m_groupItems[groupItem];

    // Insert group item.
    editorWidget->m_itemGroups->insertChild(m_oldIndex, groupItem);
    editorWidget->m_treeEditor->setItemWidget(groupItem, 1,
                                              groupInfo->groupWidget);

    // Set index.
    editorWidget->m_save->setIndex(m_newIndex, m_oldIndex);

    // Update.
    editorWidget->updateGroupMoveButtonStatus(groupItem);

    if (editorWidget->m_itemGroups->childCount() > 0) {
        if (m_newIndex == 0) {
            GroupItem *nextItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(0));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(nextItem);

        } else if (m_newIndex >= editorWidget->m_itemGroups->childCount()) {
            GroupItem *prevItem = dynamic_cast<GroupItem *>(
                editorWidget->m_itemGroups->child(m_newIndex - 1));
            Q_ASSERT(prevItem != nullptr);
            editorWidget->updateGroupMoveButtonStatus(prevItem);
        }
    }

    return;
}

/**
 * @brief	Destructor.
 */
EditorWidget::MoveGroupOperation::~MoveGroupOperation() {}
