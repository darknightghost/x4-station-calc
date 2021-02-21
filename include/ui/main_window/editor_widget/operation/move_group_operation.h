#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::MoveGroupOperation :
    virtual public EditorWidget::OperationBase<EditorWidget::MoveGroupOperation,
                                               int,
                                               int,
                                               EditorWidget *> {
    CREATE_FUNC(EditorWidget::MoveGroupOperation, int, int, EditorWidget *);

  private:
    int m_oldIndex; ///< Old index of the group item.
    int m_newIndex; ///< New index of the group item.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	oldIndex        Old index.
     * @param[in]	newIndex        New index.
     * @param[in]	editorWidget    Editor widget.
     */
    MoveGroupOperation(int oldIndex, int newIndex, EditorWidget *editorWidget);

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
    virtual ~MoveGroupOperation();
};
