#include <ui/main_window/editor_widget/operation/rename_group_operation.h>

/**
 * @brief		Constructor.
 */
RenameGroupOperation::RenameGroupOperation(GroupItem *    groupItem,
                                           const QString &oldName,
                                           const QString &newName) :
    m_groupItem(groupItem),
    m_oldName(oldName), m_newName(newName)
{
    this->setGood();
}

/**
 * @brief	Do operation.
 */
bool RenameGroupOperation::doOperation()
{
    m_groupItem->group()->setName(m_newName);
    m_groupItem->updateGroupName();
    return true;
}

/**
 * @brief	Undo operation.
 */
void RenameGroupOperation::undoOperation()
{
    m_groupItem->group()->setName(m_oldName);
    m_groupItem->updateGroupName();

    return;
}

/**
 * @brief	Destructor.
 */
RenameGroupOperation::~RenameGroupOperation() {}
