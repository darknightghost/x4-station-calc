#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class RenameGroupOperation :
    virtual public OperationBase<RenameGroupOperation,
                                 int,
                                 const QString &,
                                 const QString &,
                                 EditorWidget *> {
    CREATE_FUNC(RenameGroupOperation,
                int,
                const QString &,
                const QString &,
                EditorWidget *);

  private:
    int     m_index;   ///< Index of group item.
    QString m_oldName; ///< Old name.
    QString m_newName; ///< New name.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	index           Index of group.
     * @param[in]	oldName			Old group name.
     * @param[in]	newName			New group name.
     * @param[in]	editorWidget    Editor widget.
     */
    RenameGroupOperation(int            index,
                         const QString &oldName,
                         const QString &newName,
                         EditorWidget * editorWidget);

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
