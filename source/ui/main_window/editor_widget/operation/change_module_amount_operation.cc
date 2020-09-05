#include <ui/main_window/editor_widget/operation/change_module_amount_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::ChangeModuleAmountOperation::ChangeModuleAmountOperation(
    int           groupIndex,
    int           moduleIndex,
    int &         oldAmount,
    int &         newAmount,
    EditorWidget *editorWidget) :
    EditorWidget::OperationBase<EditorWidget::ChangeModuleAmountOperation,
                                int,
                                int,
                                int,
                                int,
                                EditorWidget *>(editorWidget),
    m_groupIndex(groupIndex), m_moduleIndex(moduleIndex),
    m_oldAmount(oldAmount), m_newAmount(newAmount)
{
    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::ChangeModuleAmountOperation::doOperation()
{
    GroupItem *groupItem = static_cast<GroupItem *>(
        this->editorWidget()->m_itemGroups->child(m_groupIndex));
    ModuleItem *moduleItem
        = static_cast<ModuleItem *>(groupItem->child(m_moduleIndex));

    moduleItem->setModuleAmount((quint64)m_newAmount);

    ModuleItemWidget *moduleWidget = dynamic_cast<ModuleItemWidget *>(
        this->editorWidget()->m_treeEditor->itemWidget(moduleItem, 1));

    moduleWidget->updateAmount();

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::ChangeModuleAmountOperation::undoOperation()
{
    GroupItem *groupItem = static_cast<GroupItem *>(
        this->editorWidget()->m_itemGroups->child(m_groupIndex));
    ModuleItem *moduleItem
        = static_cast<ModuleItem *>(groupItem->child(m_moduleIndex));

    moduleItem->setModuleAmount((quint64)m_oldAmount);

    ModuleItemWidget *moduleWidget = dynamic_cast<ModuleItemWidget *>(
        this->editorWidget()->m_treeEditor->itemWidget(moduleItem, 1));

    moduleWidget->updateAmount();
}

/**
 * @brief	Destructor.
 */
EditorWidget::ChangeModuleAmountOperation::~ChangeModuleAmountOperation() {}
