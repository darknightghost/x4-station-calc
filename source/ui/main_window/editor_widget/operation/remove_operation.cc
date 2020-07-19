#include <algorithm>

#include <QtCore/QSet>

#include <ui/main_window/editor_widget/operation/remove_operation.h>

/**
 * @brief		Constructor.
 */
EditorWidget::RemoveOperation::RemoveOperation(
    const QVector<GroupItem *> & groups,
    const QVector<ModuleItem *> &modules,
    EditorWidget *               editorWidget) :
    EditorWidget::OperationBase<EditorWidget::RemoveOperation,
                                const QVector<GroupItem *> &,
                                const QVector<ModuleItem *> &,
                                EditorWidget *>(editorWidget)
{
    // Scan groups.
    QSet<int> groupIndexes;
    for (auto groupItem : groups) {
        int groupIndex
            = this->editorWidget()->m_itemGroups->indexOfChild(groupItem);
        // Skip existing group.
        if (groupIndexes.find(groupIndex) != groupIndexes.end()) {
            continue;
        }

        // Add new group.
        groupIndexes.insert(groupIndex);
        ::std::shared_ptr<GroupToRemove> groupInfo(
            new GroupToRemove({groupIndex, groupItem->group()->name(), {}}));

        // Scan modules.
        for (int i = 0; i < groupItem->childCount(); ++i) {
            ModuleItem *moduleItem
                = static_cast<ModuleItem *>(groupItem->child(i));

            ::std::shared_ptr<ModuleInGroup> moduleInfo(
                new ModuleInGroup({moduleItem->module()->module(),
                                   (int)moduleItem->moduleAmount()}));
            groupInfo->modules.append(moduleInfo);
        }

        m_groups.append(groupInfo);
    }
    ::std::sort(m_groups.begin(), m_groups.end(),
                [](::std::shared_ptr<GroupToRemove> info1,
                   ::std::shared_ptr<GroupToRemove> info2) -> bool {
                    return info1->groupIndex < info2->groupIndex;
                });

    for (auto moduleItem : modules) {
        GroupItem *groupItem = static_cast<GroupItem *>(moduleItem->parent());
        int        groupIndex
            = this->editorWidget()->m_itemGroups->indexOfChild(groupItem);

        // Skip groups to remove.
        if (groupIndexes.find(groupIndex) != groupIndexes.end()) {
            continue;
        }

        // Append module.
        ::std::shared_ptr<ModuleToRemove> moduleInfo(new ModuleToRemove(
            {groupIndex, groupItem->indexOfChild(moduleItem),
             moduleItem->module()->module(), (int)moduleItem->moduleAmount()}));

        m_modules.append(moduleInfo);
    }

    ::std::sort(m_modules.begin(), m_modules.end(),
                [](::std::shared_ptr<ModuleToRemove> info1,
                   ::std::shared_ptr<ModuleToRemove> info2) -> bool {
                    return info1->moduleIndex < info2->moduleIndex;
                });

    this->setGood();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::RemoveOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    // Remove modules in reserved order.
    for (auto iter = m_modules.rbegin(); iter != m_modules.rend(); ++iter) {
        ::std::shared_ptr<ModuleToRemove> moduleInfo = *iter;
        GroupItem *                       groupItem  = static_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(moduleInfo->groupIndex));
        ModuleItem *moduleItem = static_cast<ModuleItem *>(
            groupItem->child(moduleInfo->moduleIndex));

        // Remove from index.
        editorWidget->m_groupItems[groupItem]->moduleMacroMap.remove(
            moduleItem->module()->module());
        editorWidget->m_groupItems[groupItem]
            ->moduleInfos[moduleItem]
            ->moduleWidget->close();
        editorWidget->m_groupItems[groupItem]->moduleInfos.remove(moduleItem);

        // Remove from save file.
        editorWidget->m_save->group(moduleInfo->groupIndex)
            ->removeModule(moduleInfo->moduleIndex);

        // Remove child.
        groupItem->removeChild(moduleItem);
    }

    // Remove groups in reserved order.
    for (auto iter = m_groups.rbegin(); iter != m_groups.rend(); ++iter) {
        ::std::shared_ptr<GroupToRemove> groupInfo = *iter;
        GroupItem *                      groupItem = static_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(groupInfo->groupIndex));

        // Close widgets.
        for (auto moduleIndex :
             editorWidget->m_groupItems[groupItem]->moduleInfos) {
            moduleIndex->moduleWidget->close();
        }

        // Remove from index.
        editorWidget->m_groupItems[groupItem]->groupWidget->close();
        editorWidget->m_groupItems.remove(groupItem);

        // Remove from save.
        editorWidget->m_save->removeGroup(groupInfo->groupIndex);

        // Remove group item.
        editorWidget->m_itemGroups->removeChild(groupItem);
    }

    return true;
}

/**
 * @brief	Undo operation.
 */
void EditorWidget::RemoveOperation::undoOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    // Add groups.
    for (auto &groupInfo : m_groups) {
        // Make group.
        ::std::shared_ptr<SaveGroup> saveGroup = SaveGroup::create();
        saveGroup->setName(groupInfo->name);

        // Add to save.
        editorWidget->m_save->insertGroup(groupInfo->groupIndex, saveGroup);

        // Make item.
        GroupItem *      groupItem   = new GroupItem(saveGroup);
        GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);

        // Add to index.
        ::std::shared_ptr<GroupInfo> groupIndex(
            new GroupInfo({groupItem, groupWidget, {}, {}}));
        editorWidget->m_groupItems[groupItem] = groupIndex;

        // Add to editor.
        editorWidget->m_itemGroups->insertChild(groupInfo->groupIndex,
                                                groupItem);
        editorWidget->m_treeEditor->setItemWidget(groupItem, 1, groupWidget);

        // Add modules.
        for (auto &moduleInfo : groupInfo->modules) {
            // Add to save.
            int index = saveGroup->insertModule(-1, moduleInfo->macro,
                                                moduleInfo->amount);

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
            groupItem->addChild(moduleItem);
            editorWidget->m_treeEditor->setItemWidget(moduleItem, 1,
                                                      moduleWidget);
        }
    }

    // Add modules.
    for (auto &moduleInfo : m_modules) {
        GroupItem *groupItem = static_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(moduleInfo->groupIndex));
        ::std::shared_ptr<GroupInfo> groupIndex
            = editorWidget->m_groupItems[groupItem];

        // Add to save.
        int index = groupItem->group()->insertModule(
            moduleInfo->moduleIndex, moduleInfo->macro, moduleInfo->amount);
        Q_ASSERT(index == moduleInfo->moduleIndex);

        // Get Save module.
        ::std::shared_ptr<SaveModule> saveModule
            = groupItem->group()->module(index);

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
        editorWidget->m_treeEditor->setItemWidget(moduleItem, 1, moduleWidget);
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::RemoveOperation::~RemoveOperation() {}
