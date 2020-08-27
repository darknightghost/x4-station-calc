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

    // Add to index.
    ::std::shared_ptr<GroupInfo> groupIndex(
        new GroupInfo({groupItem, groupWidget, {}, {}}));
    editorWidget->m_groupItems[groupItem] = groupIndex;

    // Add to editor.
    editorWidget->m_itemGroups->insertChild(m_index, groupItem);
    editorWidget->m_treeEditor->setItemWidget(groupItem, 1, groupWidget);
    groupItem->setExpanded(true);

    editorWidget->connect(groupWidget, &GroupItemWidget::removeBtnClicked,
                          editorWidget, &EditorWidget::removeGroupItem);

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

    // Close widgets.
    for (auto moduleIndex :
         editorWidget->m_groupItems[groupItem]->moduleInfos) {
        moduleIndex->moduleWidget->close();
        groupItem->removeChild(moduleIndex->moduleItem);
    }

    // Remove from index.
    editorWidget->m_groupItems[groupItem]->groupWidget->close();
    editorWidget->m_groupItems.remove(groupItem);

    // Remove from save.
    editorWidget->m_save->removeGroup(m_index);

    // Remove group item.
    editorWidget->m_itemGroups->removeChild(groupItem);
}

/**
 * @brief	Destructor.
 */
EditorWidget::AddGroupOperation::~AddGroupOperation() {}
