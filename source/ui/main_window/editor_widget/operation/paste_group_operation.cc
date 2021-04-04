#include <algorithm>

#include <QtCore/QSet>

#include <ui/main_window/editor_widget/operation/paste_group_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::PasteGroupOperation::PasteGroupOperation(
    GroupItem *                              groupAfter,
    const X4SCGroupClipboardMimeDataBuilder &builder,
    EditorWidget *                           editorWidget) :
    EditorWidget::OperationBase<EditorWidget::PasteGroupOperation,
                                GroupItem *,
                                const X4SCGroupClipboardMimeDataBuilder &,
                                EditorWidget *>(editorWidget)
{
    // Position to paste.
    if (groupAfter == nullptr) {
        m_firstGroupIndex = 0;
    } else {
        m_firstGroupIndex
            = editorWidget->m_itemGroups->indexOfChild(groupAfter);
    }

    // Groups to paste.
    for (auto group : builder.groups()) {
        m_groups.append(
            ::std::shared_ptr<GroupInfo>(new GroupInfo({group->name(), {}})));
        for (auto module : group->modules()) {
            m_groups.back()->modules.append(::std::shared_ptr<ModuleInfo>(
                new ModuleInfo({module->module(), module->amount()})));
        }
    }

    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::PasteGroupOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    // Paste groups.
    for (int i = 0; i < m_groups.size(); ++i) {
        int index = i + m_firstGroupIndex;

        // Save group.
        ::std::shared_ptr<SaveGroup> saveGroup = SaveGroup::create();
        saveGroup->setName(m_groups[i]->name);
        editorWidget->m_save->insertGroup(index, saveGroup);

        // Group item.
        GroupItem *groupItem = new GroupItem(saveGroup);
        editorWidget->m_itemGroups->insertChild(index, groupItem);

        // Group widget.
        GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);
        editorWidget->m_treeEditor->setItemWidget(groupItem, 1, groupWidget);
        groupItem->setExpanded(true);

        editorWidget->connect(groupWidget, &GroupItemWidget::btnUpClicked,
                              editorWidget, &EditorWidget::onGroupMoveUp);
        editorWidget->connect(groupWidget, &GroupItemWidget::btnDownClicked,
                              editorWidget, &EditorWidget::onGroupMoveDown);
        editorWidget->connect(groupWidget, &GroupItemWidget::btnRemoveClicked,
                              editorWidget, &EditorWidget::removeGroupItem);

        // Modules.
        for (auto module : m_groups[i]->modules) {
            // Add module.
            // Add to save.
            int index
                = saveGroup->insertModule(-1, module->macro, module->amount);
            Q_ASSERT(index >= 0);

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
        }

        // Update
        if (groupItem->childCount() > 0) {
            {
                ModuleItem *item
                    = dynamic_cast<ModuleItem *>(groupItem->child(0));
                Q_ASSERT(item != nullptr);
                editorWidget->updateModuleMoveButtonStatus(item);
            }
            {
                ModuleItem *item = dynamic_cast<ModuleItem *>(
                    groupItem->child(groupItem->childCount() - 1));
                Q_ASSERT(item != nullptr);
                editorWidget->updateModuleMoveButtonStatus(item);
            }
        }
    }

    // Update
    if (m_firstGroupIndex == 0) {
        GroupItem *item
            = dynamic_cast<GroupItem *>(editorWidget->m_itemGroups->child(0));
        Q_ASSERT(item != nullptr);
        editorWidget->updateGroupMoveButtonStatus(item);
    } else if (m_firstGroupIndex
               == editorWidget->m_itemGroups->childCount() - m_groups.size()) {
        GroupItem *item = dynamic_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(m_firstGroupIndex - 1));
        Q_ASSERT(item != nullptr);
        editorWidget->updateGroupMoveButtonStatus(item);
    }

    if (editorWidget->m_itemGroups->childCount() > 0) {
        GroupItem *item
            = dynamic_cast<GroupItem *>(editorWidget->m_itemGroups->child(
                editorWidget->m_itemGroups->childCount() - 1));
        Q_ASSERT(item != nullptr);
        editorWidget->updateGroupMoveButtonStatus(item);
    }

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::PasteGroupOperation::undoOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    for (int i = 0; i < m_groups.size(); ++i) {
        editorWidget->m_save->removeGroup(m_firstGroupIndex);
        editorWidget->m_itemGroups->removeChild(
            editorWidget->m_itemGroups->child(m_firstGroupIndex));
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::PasteGroupOperation::~PasteGroupOperation() {}
