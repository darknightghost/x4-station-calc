#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::AddGroupOperation :
    virtual public EditorWidget::
        OperationBase<EditorWidget::AddGroupOperation, int, EditorWidget *> {
    CREATE_FUNC(EditorWidget::AddGroupOperation, int, EditorWidget *);

  private:
    int m_index; ///< Index of group item.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	index           Index of group.
     * @param[in]	editorWidget    Editor widget.
     */
    AddGroupOperation(int index, EditorWidget *editorWidget);

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
    virtual ~AddGroupOperation();
};
