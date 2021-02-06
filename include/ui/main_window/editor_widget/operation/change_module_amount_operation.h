#pragma once

#pragma once

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::ChangeModuleAmountOperation :
    virtual public EditorWidget::OperationBase<
        EditorWidget::ChangeModuleAmountOperation,
        int,
        int,
        int,
        int,
        EditorWidget *>
{
    CREATE_FUNC(EditorWidget::ChangeModuleAmountOperation,
                int,
                int,
                int,
                int,
                EditorWidget *);

  private:
    int m_groupIndex;  ///< Index of group item.
    int m_moduleIndex; ///< Index of module item.
    int m_oldAmount;   ///< Old amount.
    int m_newAmount;   ///< New amount.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groupIndex      Index of groupItem.
     * @param[in]	moduleIndex     Index of moduleItem.
     * @param[in]	oldAmount		Old amount.
     * @param[in]	newAmount		New amount.
     * @param[in]	editorWidget    Editor widget.
     */
    ChangeModuleAmountOperation(int           groupIndex,
                                int           moduleIndex,
                                int &         oldAmount,
                                int &         newAmount,
                                EditorWidget *editorWidget);

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
    virtual ~ChangeModuleAmountOperation();
};
