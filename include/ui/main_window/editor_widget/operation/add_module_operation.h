#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::AddModuleOperation :
    virtual public EditorWidget::OperationBase<EditorWidget::AddModuleOperation,
                                               int,
                                               int,
                                               EditorWidget *> {
    CREATE_FUNC(EditorWidget::AddModuleOperation, int, int, EditorWidget *);

  private:
    int m_groupIndex; ///< Index of group item.
    int m_index;      ///< Index of module item.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groupIndex      Index of group.
     * @param[in]	index           Index of module.
     * @param[in]	editorWidget    Editor widget.
     */
    AddModuleOperation(int groupIndex, int index, EditorWidget *editorWidget);

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
    virtual ~AddModuleOperation();
};
