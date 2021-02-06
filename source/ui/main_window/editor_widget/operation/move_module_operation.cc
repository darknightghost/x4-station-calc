#include <ui/main_window/editor_widget/operation/move_module_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::MoveModuleOperation::MoveModuleOperation(
    int groupIndex, int oldIndex, int newIndex, EditorWidget *editorWidget) :
    EditorWidget::OperationBase<EditorWidget::MoveModuleOperation,
                                int,
                                int,
                                int,
                                EditorWidget *>(editorWidget),
    m_groupIndex(groupIndex), m_oldIndex(oldIndex), m_newIndex(newIndex)
{
    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::MoveModuleOperation::doOperation()
{
    // Editor Widget.
    EditorWidget *editorWidget = dynamic_cast<EditorWidget *>(m_editorWidget);
    Q_ASSERT(editorWidget != nullptr);

    // Get group item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);

    // Take module item.
    ModuleItem *moduleItem
        = dynamic_cast<ModuleItem *>(groupItem->takeChild(m_oldIndex));
    Q_ASSERT(moduleItem != nullptr);

    // Insert module item.
    groupItem->insertChild(m_newIndex, moduleItem);
    ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);
    editorWidget->connect(moduleWidget, &ModuleItemWidget::changeAmount,
                          editorWidget, &EditorWidget::onChangeAmount);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::upBtnClicked,
                          editorWidget, &EditorWidget::onModuleMoveUp);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::downBtnClicked,
                          editorWidget, &EditorWidget::onModuleMoveDown);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::removeBtnClicked,
                          editorWidget, &EditorWidget::removeModuleItem);
    editorWidget->m_treeEditor->setItemWidget(moduleItem, 1, moduleWidget);

    // Set index.
    groupItem->group()->setIndex(m_oldIndex, m_newIndex);

    // Update.
    editorWidget->updateModuleMoveButtonStatus(moduleItem, moduleWidget);

    if (groupItem->childCount() > 0) {
        if (m_newIndex == 0) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(1));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        } else if (m_newIndex == groupItem->childCount() - 1) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(m_newIndex - 1));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        }

        if (m_oldIndex == 0) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(0));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        } else if (m_oldIndex == groupItem->childCount() - 1) {
            ModuleItem *prevItem
                = dynamic_cast<ModuleItem *>(groupItem->child(m_oldIndex));
            Q_ASSERT(prevItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(prevItem);
        }
    }

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::MoveModuleOperation::undoOperation()
{
    // Editor Widget.
    EditorWidget *editorWidget = dynamic_cast<EditorWidget *>(m_editorWidget);
    Q_ASSERT(editorWidget != nullptr);

    // Get group item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);

    // Take module item.
    ModuleItem *moduleItem
        = dynamic_cast<ModuleItem *>(groupItem->takeChild(m_newIndex));
    Q_ASSERT(moduleItem != nullptr);

    // Insert module item.
    groupItem->insertChild(m_oldIndex, moduleItem);
    ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);
    editorWidget->connect(moduleWidget, &ModuleItemWidget::changeAmount,
                          editorWidget, &EditorWidget::onChangeAmount);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::upBtnClicked,
                          editorWidget, &EditorWidget::onModuleMoveUp);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::downBtnClicked,
                          editorWidget, &EditorWidget::onModuleMoveDown);
    moduleWidget->connect(moduleWidget, &ModuleItemWidget::removeBtnClicked,
                          editorWidget, &EditorWidget::removeModuleItem);
    editorWidget->m_treeEditor->setItemWidget(moduleItem, 1, moduleWidget);

    // Set index.
    groupItem->group()->setIndex(m_newIndex, m_oldIndex);

    // Update.
    editorWidget->updateModuleMoveButtonStatus(moduleItem, moduleWidget);

    if (groupItem->childCount() > 0) {
        if (m_oldIndex == 0) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(1));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        } else if (m_oldIndex == groupItem->childCount() - 1) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(m_oldIndex - 1));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        }

        if (m_newIndex == 0) {
            ModuleItem *nextItem
                = dynamic_cast<ModuleItem *>(groupItem->child(0));
            Q_ASSERT(nextItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(nextItem);
        } else if (m_newIndex == groupItem->childCount() - 1) {
            ModuleItem *prevItem
                = dynamic_cast<ModuleItem *>(groupItem->child(m_newIndex));
            Q_ASSERT(prevItem != nullptr);
            editorWidget->updateModuleMoveButtonStatus(prevItem);
        }
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::MoveModuleOperation::~MoveModuleOperation() {}
