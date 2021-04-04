#include <algorithm>

#include <QtCore/QSet>

#include <ui/main_window/editor_widget/operation/paste_module_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::PasteModuleOperation::PasteModuleOperation(
    GroupItem *                               groupToPaste,
    ModuleItem *                              moduleAfter,
    const X4SCModuleClipboardMimeDataBuilder &builder,
    EditorWidget *                            editorWidget) :
    EditorWidget::OperationBase<EditorWidget::PasteModuleOperation,
                                GroupItem *,
                                ModuleItem *,
                                const X4SCModuleClipboardMimeDataBuilder &,
                                EditorWidget *>(editorWidget)
{
    // Position to paste.
    if (groupToPaste == nullptr) {
        m_groupIndex = 0;
    } else {
        m_groupIndex = editorWidget->m_itemGroups->indexOfChild(groupToPaste);
    }

    if (moduleAfter == nullptr) {
        m_firstModuleIndex = 0;
    } else {
        m_groupIndex
            = editorWidget->m_itemGroups->indexOfChild(moduleAfter->parent());
        m_firstModuleIndex
            = moduleAfter->parent()->indexOfChild(moduleAfter) + 1;
    }

    // Modules to paste.
    for (auto module : builder.modules()) {
        m_modules.append(::std::shared_ptr<ModuleInfo>(
            new ModuleInfo({module->module(), module->amount()})));
    }

    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::PasteModuleOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    // Paste modules.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);
    int newIndex = m_firstModuleIndex;
    int oldCount = groupItem->childCount();

    for (auto module : m_modules) {
        QString &                    macro     = module->macro;
        ::std::shared_ptr<SaveGroup> saveGroup = groupItem->group();

        ModuleItem *moduleItem = groupItem->child(module->macro);
        if (moduleItem == nullptr) {
            // Add module.
            // Add to save.
            int index = groupItem->group()->insertModule(newIndex, macro,
                                                         module->amount);
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
            editorWidget->connect(moduleWidget, &ModuleItemWidget::btnUpClicked,
                                  editorWidget, &EditorWidget::onModuleMoveUp);
            editorWidget->connect(
                moduleWidget, &ModuleItemWidget::btnDownClicked, editorWidget,
                &EditorWidget::onModuleMoveDown);
            editorWidget->connect(
                moduleWidget, &ModuleItemWidget::btnRemoveClicked, editorWidget,
                &EditorWidget::removeModuleItem);
            ++newIndex;

            // Update.
            editorWidget->updateModuleMoveButtonStatus(moduleItem);
        } else {
            // Increase amount.
            moduleItem->setModuleAmount(moduleItem->moduleAmount()
                                        + module->amount);

            ModuleItemWidget *itemWidget = dynamic_cast<ModuleItemWidget *>(
                editorWidget->m_treeEditor->itemWidget(moduleItem, 1));
            itemWidget->updateAmount();
        }
    }

    // Update
    if (oldCount > 0) {
        if (m_firstModuleIndex == oldCount) {
            ModuleItem *item = dynamic_cast<ModuleItem *>(
                groupItem->child(m_firstModuleIndex - 1));
            Q_ASSERT(item != nullptr);
            editorWidget->updateModuleMoveButtonStatus(item);
        }
    }
    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::PasteModuleOperation::undoOperation()
{
    EditorWidget *editorWidget = this->editorWidget();
    Q_ASSERT(editorWidget != nullptr);

    // Get group information.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        editorWidget->m_itemGroups->child(m_groupIndex));
    Q_ASSERT(groupItem != nullptr);
    ::std::shared_ptr<SaveGroup> saveGroup = groupItem->group();

    for (auto module : m_modules) {
        QString &                    macro     = module->macro;
        ::std::shared_ptr<SaveGroup> saveGroup = groupItem->group();

        // Get module.
        ModuleItem *moduleItem = groupItem->child(macro);
        Q_ASSERT(moduleItem != nullptr);
        ModuleItemWidget *moduleWidget = dynamic_cast<ModuleItemWidget *>(
            editorWidget->m_treeEditor->itemWidget(moduleItem, 1));
        Q_ASSERT(moduleWidget != nullptr);

        if (moduleItem->moduleAmount() > module->amount) {
            // Decrease amount/
            moduleItem->setModuleAmount(moduleItem->moduleAmount()
                                        - module->amount);
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
                if (m_firstModuleIndex == 0) {
                    ModuleItem *nextItem
                        = dynamic_cast<ModuleItem *>(groupItem->child(0));
                    Q_ASSERT(nextItem != nullptr);
                    editorWidget->updateModuleMoveButtonStatus(nextItem);
                } else if (m_firstModuleIndex >= groupItem->childCount()) {
                    ModuleItem *prevItem = dynamic_cast<ModuleItem *>(
                        groupItem->child(m_firstModuleIndex - 1));
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
EditorWidget::PasteModuleOperation::~PasteModuleOperation() {}
