#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class RenameGroupOperation :
    virtual public OperationBase<RenameGroupOperation,
                                 GroupItem *,
                                 const QString &,
                                 const QString &> {
    CREATE_FUNC(RenameGroupOperation,
                GroupItem *,
                const QString &,
                const QString &);

  private:
    GenericReference<GroupItem> m_groupItem; ///< Group item.
    QString                     m_oldName;   ///< Old name.
    QString                     m_newName;   ///< New name.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groupItem		Group item.
     * @param[in]	oldName			Old group name.
     * @param[in]	newName			New group name.
     */
    RenameGroupOperation(GroupItem *    groupItem,
                         const QString &oldName,
                         const QString &newName);

  public:
    /**
     * @brief	Do operation.
     *
     * @return	On success, the method will return \c true, otherwise returns
     *			\c false.
     */
    virtual bool doOperation() override;

    /**
     * @brief	Undo operation.
     */
    virtual void undoOperation() override;

    /**
     * @brief	Destructor.
     */
    virtual ~RenameGroupOperation();
};
