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
    ::std::shared_ptr<SaveGroup> saveGroup = groupItem->group();

    ::std::shared_ptr<GroupInfo> groupIndex
        = editorWidget->m_groupItems[groupItem];

    int newIndex = m_index;
    for (auto &macro : m_macros) {
        auto iter = groupIndex->moduleMacroMap.find(macro);
        if (iter == groupIndex->moduleMacroMap.end()) {
            // Add module.
            // Add to save.
            int index = saveGroup->insertModule(newIndex, macro, 1);
            Q_ASSERT(index == newIndex);

            // Get save module.
            ::std::shared_ptr<SaveModule> saveModule = saveGroup->module(index);

            // Make item.
            ModuleItem *      moduleItem   = new ModuleItem(saveModule);
            ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);

            // Add to index.
            ::std::shared_ptr<ModuleInfo> moduleIndex(
                new ModuleInfo({moduleItem, moduleWidget}));
            groupIndex->moduleInfos[moduleItem]              = moduleIndex;
            groupIndex->moduleMacroMap[saveModule->module()] = moduleIndex;

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
            ::std::shared_ptr<ModuleInfo> moduleIndex = *iter;
            moduleIndex->moduleItem->setModuleAmount(
                moduleIndex->moduleItem->moduleAmount() + 1);
            moduleIndex->moduleWidget->updateAmount();
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

    ::std::shared_ptr<GroupInfo> groupIndex
        = editorWidget->m_groupItems[groupItem];

    for (auto &macro : m_macros) {
        // Get module.
        ::std::shared_ptr<ModuleInfo> moduleIndex
            = groupIndex->moduleMacroMap[macro];
        if (moduleIndex->moduleItem->moduleAmount() > 1) {
            // Decrease amount/
            moduleIndex->moduleItem->setModuleAmount(
                moduleIndex->moduleItem->moduleAmount() - 1);
            moduleIndex->moduleWidget->updateAmount();

        } else {
            // Remove.
            // Remove from index.
            ModuleItem *moduleItem = moduleIndex->moduleItem;
            groupIndex->moduleMacroMap.remove(moduleItem->module()->module());
            groupIndex->moduleInfos[moduleItem]->moduleWidget->close();
            groupIndex->moduleInfos.remove(moduleItem);

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
