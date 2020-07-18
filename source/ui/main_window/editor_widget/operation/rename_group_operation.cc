#include <ui/main_window/editor_widget/operation/rename_group_operation.h>

/**
 * @brief		Constructor.
 */
RenameGroupOperation::RenameGroupOperation(int            index,
                                           const QString &oldName,
                                           const QString &newName,
                                           EditorWidget * editorWidget) :
    OperationBase<RenameGroupOperation,
                  int,
                  const QString &,
                  const QString &,
                  EditorWidget *>(editorWidget),
    m_index(index), m_oldName(oldName), m_newName(newName)
{
    this->setGood();
}

/**
 * @brief	Do operation.
 */
bool RenameGroupOperation::doOperation()
{
    GroupItem *groupItem = static_cast<GroupItem *>(
        editorWidget()->m_itemGroups->child(m_index));

    groupItem->group()->setName(m_newName);
    groupItem->updateGroupName();

    return true;
}

/**
 * @brief	Undo operation.
 */
void RenameGroupOperation::undoOperation()
{
    GroupItem *groupItem = static_cast<GroupItem *>(
        editorWidget()->m_itemGroups->child(m_index));

    groupItem->group()->setName(m_oldName);
    groupItem->updateGroupName();

    return;
}

/**
 * @brief	Destructor.
 */
RenameGroupOperation::~RenameGroupOperation() {}
