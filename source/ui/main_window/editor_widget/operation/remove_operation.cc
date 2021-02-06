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
    for (auto groupItem : groups)
    {
        int groupIndex
            = this->editorWidget()->m_itemGroups->indexOfChild(groupItem);
        // Skip existing group.
        if (groupIndexes.find(groupIndex) != groupIndexes.end())
        {
            continue;
        }

        // Add new group.
        groupIndexes.insert(groupIndex);
        ::std::shared_ptr<GroupToRemove> groupInfo(
            new GroupToRemove({groupIndex,
                               groupItem->group()->name(),
                               groupItem->isExpanded(),
                               {}}));

        // Scan modules.
        for (int i = 0; i < groupItem->childCount(); ++i)
        {
            ModuleItem *moduleItem
                = dynamic_cast<ModuleItem *>(groupItem->child(i));
            Q_ASSERT(moduleItem != nullptr);

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

    for (auto moduleItem : modules)
    {
        GroupItem *groupItem = dynamic_cast<GroupItem *>(moduleItem->parent());
        Q_ASSERT(groupItem != nullptr);
        int groupIndex
            = this->editorWidget()->m_itemGroups->indexOfChild(groupItem);

        // Skip groups to remove.
        if (groupIndexes.find(groupIndex) != groupIndexes.end())
        {
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

    this->setInitialized();
}

/**
 * @brief	Do operation.
 */
bool EditorWidget::RemoveOperation::doOperation()
{
    EditorWidget *editorWidget = this->editorWidget();

    // Remove modules in reserved order.
    for (auto iter = m_modules.rbegin(); iter != m_modules.rend(); ++iter)
    {
        ::std::shared_ptr<ModuleToRemove> moduleInfo = *iter;
        GroupItem *                       groupItem = dynamic_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(moduleInfo->groupIndex));
        Q_ASSERT(groupItem != nullptr);
        ModuleItem *moduleItem = dynamic_cast<ModuleItem *>(
            groupItem->child(moduleInfo->moduleIndex));
        Q_ASSERT(moduleItem != nullptr);

        // Remove from save file.
        editorWidget->m_save->group(moduleInfo->groupIndex)
            ->removeModule(moduleInfo->moduleIndex);

        // Remove child.
        groupItem->removeChild(moduleItem);

        // Update.
        if (groupItem->childCount() > 0)
        {
            if (moduleInfo->moduleIndex == 0)
            {
                ModuleItem *nextItem
                    = dynamic_cast<ModuleItem *>(groupItem->child(0));
                Q_ASSERT(nextItem != nullptr);
                editorWidget->updateModuleMoveButtonStatus(nextItem);
            }
            else if (moduleInfo->moduleIndex >= groupItem->childCount())
            {
                ModuleItem *prevItem = dynamic_cast<ModuleItem *>(
                    groupItem->child(moduleInfo->moduleIndex - 1));
                Q_ASSERT(prevItem != nullptr);
                editorWidget->updateModuleMoveButtonStatus(prevItem);
            }
        }
    }

    // Remove groups in reserved order.
    for (auto iter = m_groups.rbegin(); iter != m_groups.rend(); ++iter)
    {
        ::std::shared_ptr<GroupToRemove> groupInfo = *iter;
        GroupItem *                      groupItem = dynamic_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(groupInfo->groupIndex));
        Q_ASSERT(groupItem != nullptr);

        // Remove from save.
        editorWidget->m_save->removeGroup(groupInfo->groupIndex);

        // Remove group item.
        editorWidget->m_itemGroups->removeChild(groupItem);

        // Update.
        if (editorWidget->m_itemGroups->childCount() > 0)
        {
            if (groupInfo->groupIndex == 0)
            {
                GroupItem *nextItem = dynamic_cast<GroupItem *>(
                    editorWidget->m_itemGroups->child(0));
                Q_ASSERT(nextItem != nullptr);
                editorWidget->updateGroupMoveButtonStatus(nextItem);
            }
            else if (groupInfo->groupIndex
                     >= editorWidget->m_itemGroups->childCount())
            {
                GroupItem *prevItem = dynamic_cast<GroupItem *>(
                    editorWidget->m_itemGroups->child(groupInfo->groupIndex
                                                      - 1));
                Q_ASSERT(prevItem != nullptr);
                editorWidget->updateGroupMoveButtonStatus(prevItem);
            }
        }
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
    for (auto &groupInfo : m_groups)
    {
        // Make group.
        ::std::shared_ptr<SaveGroup> saveGroup = SaveGroup::create();
        saveGroup->setName(groupInfo->name);

        // Add to save.
        editorWidget->m_save->insertGroup(groupInfo->groupIndex, saveGroup);

        // Make item.
        GroupItem *      groupItem   = new GroupItem(saveGroup);
        GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);

        // Add to editor.
        editorWidget->m_itemGroups->insertChild(groupInfo->groupIndex,
                                                groupItem);
        editorWidget->m_treeEditor->setItemWidget(groupItem, 1, groupWidget);
        groupItem->setExpanded(groupInfo->expanded);

        editorWidget->connect(groupWidget, &GroupItemWidget::upBtnClicked,
                              editorWidget, &EditorWidget::onGroupMoveUp);
        editorWidget->connect(groupWidget, &GroupItemWidget::downBtnClicked,
                              editorWidget, &EditorWidget::onGroupMoveDown);
        editorWidget->connect(groupWidget, &GroupItemWidget::removeBtnClicked,
                              editorWidget, &EditorWidget::removeGroupItem);

        // Update.
        editorWidget->updateGroupMoveButtonStatus(groupItem);

        // Add modules.
        for (auto &moduleInfo : groupInfo->modules)
        {
            // Add to save.
            int index = saveGroup->insertModule(-1, moduleInfo->macro,
                                                moduleInfo->amount);

            // Get save module.
            ::std::shared_ptr<SaveModule> saveModule = saveGroup->module(index);

            // Make item.
            ModuleItem *      moduleItem   = new ModuleItem(saveModule);
            ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);

            // Add to editor.
            groupItem->addChild(moduleItem);
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

            // Update.
            editorWidget->updateModuleMoveButtonStatus(moduleItem);
        }
    }

    // Add modules.
    for (auto &moduleInfo : m_modules)
    {
        GroupItem *groupItem = dynamic_cast<GroupItem *>(
            editorWidget->m_itemGroups->child(moduleInfo->groupIndex));
        Q_ASSERT(groupItem != nullptr);

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

        // Add to editor.
        groupItem->insertChild(index, moduleItem);
        editorWidget->m_treeEditor->setItemWidget(moduleItem, 1, moduleWidget);

        editorWidget->connect(moduleWidget, &ModuleItemWidget::changeAmount,
                              editorWidget, &EditorWidget::onChangeAmount);
        editorWidget->connect(moduleWidget, &ModuleItemWidget::upBtnClicked,
                              editorWidget, &EditorWidget::onModuleMoveUp);
        editorWidget->connect(moduleWidget, &ModuleItemWidget::downBtnClicked,
                              editorWidget, &EditorWidget::onModuleMoveDown);
        editorWidget->connect(moduleWidget, &ModuleItemWidget::removeBtnClicked,
                              editorWidget, &EditorWidget::removeModuleItem);

        // Update.
        editorWidget->updateModuleMoveButtonStatus(moduleItem);
    }
}

/**
 * @brief	Destructor.
 */
EditorWidget::RemoveOperation::~RemoveOperation() {}
