#include <ui/main_window/editor_widget/operation/add_module_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::AddModuleOperation::AddModuleOperation(
    int                groupIndex,
    int                index,
    const QStringList &macros,
    EditorWidget *     editorWidget) :

    OperationBase<EditorWidget::AddModuleOperation,
                  int,
                  int,
                  const QStringList &,
                  EditorWidget *>(editorWidget),
    m_groupIndex(groupIndex), m_index(index), m_macros(macros)
{
    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::AddModuleOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();
    Q_ASSERT(editorWidget != nullptr);

    // Get group information.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);
    ::std::shared_ptr<SaveGroup> saveGroup   = groupItem->group();
    int                          oldCount    = groupItem->childCount();
    bool                         expandGroup = false;

    if (groupItem->childCount() == 0) {
        expandGroup = true;
    }

    int newIndex = m_index;
    for (auto &macro : m_macros) {
        ModuleItem *moduleItem = groupItem->child(macro);
        if (moduleItem == nullptr) {
            // Add module.
            // Add to save.
            int index = saveGroup->insertModule(newIndex, macro, 1);
            Q_ASSERT(index == newIndex);

            // Get save module.
            ::std::shared_ptr<SaveModule> saveModule = saveGroup->module(index);

            // Make item.
            ModuleItem *      moduleItem   = new ModuleItem(saveModule);
            ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);

            // Add to editor.
            groupItem->insertChild(index, moduleItem);
            editorWidget->m_treeEditor->setItemWidget(moduleItem, 1,
                                                      moduleWidget);

            editorWidget->connect(moduleWidget, &ModuleItemWidget::changeAmount,
                                  editorWidget, &EditorWidget::onChangeAmount);
            editorWidget->connect(moduleWidget, &ModuleItemWidget::upBtnClicked,
                                  editorWidget, &EditorWidget::onModuleMoveUp);
            editorWidget->connect(
                moduleWidget, &ModuleItemWidget::downBtnClicked, editorWidget,
                &EditorWidget::onModuleMoveDown);
            editorWidget->connect(
                moduleWidget, &ModuleItemWidget::removeBtnClicked, editorWidget,
                &EditorWidget::removeModuleItem);
            ++newIndex;

            // Update.
            editorWidget->updateModuleMoveButtonStatus(moduleItem);
        } else {
            // Increase amount.
            moduleItem->setModuleAmount(moduleItem->moduleAmount() + 1);

            ModuleItemWidget *itemWidget = dynamic_cast<ModuleItemWidget *>(
                editorWidget->m_treeEditor->itemWidget(moduleItem, 1));
            itemWidget->updateAmount();
        }
    }

    if (expandGroup) {
        groupItem->setExpanded(true);
    }

    // Update
    if (oldCount > 0) {
        if (m_index == oldCount) {
            ModuleItem *item
                = dynamic_cast<ModuleItem *>(groupItem->child(m_index - 1));
            Q_ASSERT(item != nullptr);
            editorWidget->updateModuleMoveButtonStatus(item);
        }
    }

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::AddModuleOperation::undoOperation()
{
    EditorWidget *editorWidget = this->editorWidget();
    Q_ASSERT(editorWidget != nullptr);

    // Get group information.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);
    ::std::shared_ptr<SaveGroup> saveGroup = groupItem->group();

    for (auto &macro : m_macros) {
        // Get module.
        ModuleItem *moduleItem = groupItem->child(macro);
        Q_ASSERT(moduleItem != nullptr);
        ModuleItemWidget *moduleWidget = dynamic_cast<ModuleItemWidget *>(
            editorWidget->m_treeEditor->itemWidget(moduleItem, 1));
        Q_ASSERT(moduleWidget != nullptr);

        if (moduleItem->moduleAmount() > 1) {
            // Decrease amount/
            moduleItem->setModuleAmount(moduleItem->moduleAmount() - 1);
            moduleWidget->updateAmount();
        } else {
            // Remove.
            // Remove from save file.
            editorWidget->m_save->group(m_groupIndex)
                ->removeModule(groupItem->indexOfChild(moduleItem));

            // Remove child.
            groupItem->removeChild(moduleItem);

            // Update.
            if (groupItem->childCount() > 0) {
                if (m_index == 0) {
                    ModuleItem *nextItem
                        = dynamic_cast<ModuleItem *>(groupItem->child(0));
                    Q_ASSERT(nextItem != nullptr);
                    editorWidget->updateModuleMoveButtonStatus(nextItem);
                } else if (m_index >= groupItem->childCount()) {
                    ModuleItem *prevItem = dynamic_cast<ModuleItem *>(
                        groupItem->child(m_index - 1));
                    Q_ASSERT(prevItem != nullptr);
                    editorWidget->updateModuleMoveButtonStatus(prevItem);
                }
            }
        }
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::AddModuleOperation::~AddModuleOperation() {}
