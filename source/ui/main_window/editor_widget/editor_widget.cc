#include <QtCore/QDebug>
#include <QtGui/QCloseEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QHeaderView>

#include <locale/string_table.h>
#include <ui/main_window/editor_widget/editor_widget.h>

/**
 * @brief		Constructor.
 */
EditorWidget::EditorWidget(::std::shared_ptr<Save>  save,
                           MainWindow::EditActions *editActions,
                           InfoWidget *             infoWidget,
                           StationModulesWidget *   stationModulesWidget,
                           QMdiSubWindow *          parent) :
    QWidget(parent),
    m_stationModulesWidget(stationModulesWidget), m_infoWidget(infoWidget),
    m_save(save), m_savedUndoCount(0), m_editActions(editActions),
    m_backgroundTasks(
        new BackgroundTask(BackgroundTask::RunType::Newest, this)),
    m_treeEditor(nullptr)
{
    this->connect(this, &EditorWidget::windowTitleChanged, parent,
                  &QMdiSubWindow::setWindowTitle);
    parent->setWidget(this);
    parent->show();
    this->show();

    this->setAttribute(Qt::WA_DeleteOnClose);
    parent->setAttribute(Qt::WA_DeleteOnClose);
    if (save->path() == "") {
        this->setWindowTitle(STR("STR_NEW_STATION"));
    } else {
        this->setWindowTitle(
            save->path().split("/", Qt::SkipEmptyParts).back());
    }

    // UI
    // Layout
    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);

    // Editor.
    m_treeEditor = new QTreeWidget(this);
    m_treeEditor->header()->setVisible(false);
    m_treeEditor->header()->setSectionResizeMode(
        QHeaderView::ResizeMode::ResizeToContents);
    m_treeEditor->header()->setStretchLastSection(true);
    m_treeEditor->setColumnCount(3);
    m_treeEditor->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);
    m_layout->addWidget(m_treeEditor);
    this->connect(m_treeEditor, &QTreeWidget::itemChanged, this,
                  &EditorWidget::onItemChanged);
    this->connect(m_treeEditor, &QTreeWidget::itemDoubleClicked, this,
                  &EditorWidget::onItemDoubleClicked);
    this->connect(m_treeEditor, &QTreeWidget::itemSelectionChanged, this,
                  &EditorWidget::updateCutCopyRemoveStatus);

    // Items.
    m_itemGroups = new QTreeWidgetItem();
    m_itemGroups->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemGroups);
    this->loadGroups();

    m_itemSummary = new QTreeWidgetItem();
    m_itemSummary->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemSummary);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &EditorWidget::onLanguageChanged);

    this->onLanguageChanged();

    m_treeEditor->expandAll();
}

/**
 * @brief	Destructors.
 */
EditorWidget::~EditorWidget() {}

/**
 * @brief		Do operation.
 */
void EditorWidget::doOperation(::std::shared_ptr<Operation> operation)
{
    if (operation->doOperation()) {
        m_redoStack.clear();
        m_undoStack.push_back(operation);
        qDebug() << "Operation done.";
        this->updateUndoRedoStatus();

    } else {
        qDebug() << "Operation failed.";
    }
}

/**
 * @brief       Update undo/redo action statis.
 */
void EditorWidget::updateUndoRedoStatus()
{
    if (m_undoStack.empty()) {
        m_editActions->actionEditUndo->setEnabled(false);

    } else {
        m_editActions->actionEditUndo->setEnabled(true);
    }

    if (m_redoStack.empty()) {
        m_editActions->actionEditRedo->setEnabled(false);

    } else {
        m_editActions->actionEditRedo->setEnabled(true);
    }
}

/**
 * @brief       Update cut/copy/remove action statis.
 */
void EditorWidget::updateCutCopyRemoveStatus()
{
    bool found = false;
    if (m_treeEditor != nullptr) {
        for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
            if (dynamic_cast<GroupItem *>(rawItem) != nullptr
                || dynamic_cast<ModuleItem *>(rawItem) != nullptr) {
                found = true;
                break;
            }
        }
    }

    m_editActions->actionEditCut->setEnabled(found);
    m_editActions->actionEditCopy->setEnabled(found);
    m_editActions->actionEditRemove->setEnabled(found);
}

/**
 * @brief       Update paste action statis.
 */
void EditorWidget::updatePasteStatus() {}

/**
 * @brief	Close save file.
 */
bool EditorWidget::closeSave()
{
    if (m_save == nullptr) {
        return true;
    }
    if (m_savedUndoCount != m_undoStack.size()) {
        this->save();
        if (m_savedUndoCount != m_undoStack.size()) {
            return false;
        }
    }

    return true;
}

/**
 * @brief	Check save file.
 */
void EditorWidget::checkSave() {}

/**
 * @brief	Load groups.
 */
void EditorWidget::loadGroups()
{
    // Load groups.
    for (::std::shared_ptr<SaveGroup> group : m_save->groups()) {
        GroupItem *groupItem = new GroupItem(group);
        m_itemGroups->addChild(groupItem);
        GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);
        groupItem->treeWidget()->setItemWidget(groupItem, 1, groupWidget);
        groupWidget->show();
        ::std::shared_ptr<GroupInfo> groupInfo(
            new GroupInfo({groupItem, groupWidget, {}, {}}));
        m_groupItems[groupItem] = groupInfo;
        this->connect(groupWidget, &GroupItemWidget::removeBtnClicked, this,
                      &EditorWidget::removeGroupItem);

        // Load modules.
        for (::std::shared_ptr<SaveModule> module : group->modules()) {
            ModuleItem *moduleItem = new ModuleItem(module);
            groupItem->addChild(moduleItem);
            ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);
            moduleItem->treeWidget()->setItemWidget(moduleItem, 1,
                                                    moduleWidget);
            moduleWidget->show();
            ::std::shared_ptr<ModuleInfo> moduleInfo(
                new ModuleInfo({moduleItem, moduleWidget}));
            groupInfo->moduleInfos[moduleItem]          = moduleInfo;
            groupInfo->moduleMacroMap[module->module()] = moduleInfo;

            this->connect(moduleWidget, &ModuleItemWidget::changeAmount, this,
                          &EditorWidget::onChangeAmount);
            this->connect(moduleWidget, &ModuleItemWidget::removeBtnClicked,
                          this, &EditorWidget::removeModuleItem);
        }
    }
}

/**
 * @brief		Close event.
 */
void EditorWidget::closeEvent(QCloseEvent *event)
{
    if (this->closeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief	    Add module.
 */
void EditorWidget::addModules(const QStringList &macro) {}

/**
 * @brief		Create new group.
 */
void EditorWidget::newGroup() {}

/**
 * @brief		Undo.
 */
void EditorWidget::undo()
{
    if (m_undoStack.empty()) {
        return;
    }

    // Get opetarion.
    ::std::shared_ptr<Operation> op = m_undoStack.takeLast();

    // Undo.
    op->undoOperation();

    // Add to redo stack.
    m_redoStack.push_back(op);

    this->updateUndoRedoStatus();
}

/**
 * @brief		Redo.
 */
void EditorWidget::redo()
{
    if (m_redoStack.empty()) {
        return;
    }

    // Get opetarion.
    ::std::shared_ptr<Operation> op = m_redoStack.takeLast();

    // Redo.
    op->doOperation();

    // Add to redo stack.
    m_undoStack.push_back(op);

    this->updateUndoRedoStatus();
}

/**
 * @brief		Cut.
 */
void EditorWidget::cut() {}

/**
 * @brief		Copy.
 */
void EditorWidget::copy() {}

/**
 * @brief		Paste.
 */
void EditorWidget::paste() {}

/**
 * @brief		Remove.
 */
void EditorWidget::remove()
{
    QVector<GroupItem *>  groupItems;
    QVector<ModuleItem *> moduleItems;
    for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
        {
            GroupItem *item = dynamic_cast<GroupItem *>(rawItem);

            if (item != nullptr) {
                groupItems.push_back(item);
                continue;
            }
        }

        {
            ModuleItem *item = dynamic_cast<ModuleItem *>(rawItem);
            if (item != nullptr) {
                moduleItems.push_back(item);
            }
        }
    }

    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create(groupItems, moduleItems, this);
    this->doOperation(operation);
}

/**
 * @brief		Remove group item.
 */
void EditorWidget::removeGroupItem(GroupItem *item)
{
    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create({item}, {}, this);
    this->doOperation(operation);
}

/**
 * @brief		Remove module item.
 */
void EditorWidget::removeModuleItem(ModuleItem *item)
{
    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create({}, {item}, this);
    this->doOperation(operation);
}

/**
 * @brief		Save.
 */
void EditorWidget::save()
{
    m_savedUndoCount = m_undoStack.size();
}

/**
 * @brief		Save as.
 */
void EditorWidget::saveAs() {}

/**
 * @brief		Change language.
 */
void EditorWidget::onLanguageChanged()
{
    m_itemGroups->setText(0, STR("STR_STATION_MODULES"));
    m_itemSummary->setText(0, STR("STR_SUMMARY"));
}

/**
 * @brief	Clear all warning informations.
 */
void EditorWidget::clearInfo()
{
    while (! m_widgetsWarningInfos.empty()) {
        m_widgetsWarningInfos.back()->close();
        delete m_widgetsWarningInfos.back();
        m_widgetsWarningInfos.pop_back();
    }
}

/**
 * @brief		Show error information.
 */
void EditorWidget::addWarning(QString id)
{
    WarningWidget *warningWidget = new WarningWidget(
        ::std::shared_ptr<GenericString>(new LocaleString(id)));
    m_layout->insertWidget(0, warningWidget);
    m_widgetsWarningInfos.push_back(warningWidget);
}

/**
 * @brief		Called when item changed.
 */
void EditorWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(item);
    if (groupItem != nullptr && column == 0) {
        int index = m_itemGroups->indexOfChild(item);
        if (groupItem->text(0) != groupItem->group()->name()) {
            ::std::shared_ptr<Operation> operation
                = RenameGroupOperation::create(index,
                                               groupItem->group()->name(),
                                               groupItem->text(0), this);
            this->doOperation(operation);
            qDebug() << "Group name edited.";
        }
    }
}

/**
 * @brief		Called when item double clicked.
 */
void EditorWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (column != 0) {
        return;
    }

    // Module item.
    ModuleItem *moduleItem = dynamic_cast<ModuleItem *>(item);
    if (moduleItem != nullptr) {
        m_infoWidget->showStationModuleInfo(moduleItem->module()->module());
        return;
    }
}

/**
 * @brief	    on amount changed.
 */
void EditorWidget::onChangeAmount(quint64     oldAmount,
                                  quint64     newAmount,
                                  ModuleItem *moduleItem)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(moduleItem->parent());
    if (groupItem != nullptr) {
        ::std::shared_ptr<Operation> operation
            = ChangeModuleAmountOperation::create(
                m_itemGroups->indexOfChild(groupItem),
                groupItem->indexOfChild(moduleItem), oldAmount, newAmount,
                this);

        this->doOperation(operation);
    }
}

/**
 * @brief       Active editor widget.
 */
void EditorWidget::active()
{
    // Connect actions.
    this->disconnect(m_stationModulesWidget,
                     &StationModulesWidget::addToStation, nullptr, nullptr);
    this->connect(m_stationModulesWidget, &StationModulesWidget::addToStation,
                  this, &EditorWidget::addModules);

    this->disconnect(m_editActions->actionEditNewGroup, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditNewGroup, &QAction::triggered, this,
                  &EditorWidget::newGroup);

    this->disconnect(m_editActions->actionEditUndo, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditUndo, &QAction::triggered, this,
                  &EditorWidget::undo);

    this->disconnect(m_editActions->actionEditRedo, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditRedo, &QAction::triggered, this,
                  &EditorWidget::redo);

    this->disconnect(m_editActions->actionEditCut, &QAction::triggered, nullptr,
                     nullptr);
    this->connect(m_editActions->actionEditCut, &QAction::triggered, this,
                  &EditorWidget::cut);

    this->disconnect(m_editActions->actionEditCopy, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditCopy, &QAction::triggered, this,
                  &EditorWidget::copy);

    this->disconnect(m_editActions->actionEditPaste, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditPaste, &QAction::triggered, this,
                  &EditorWidget::paste);

    this->disconnect(m_editActions->actionEditRemove, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditRemove, &QAction::triggered, this,
                  &EditorWidget::remove);

    // Update actions status.
    this->updateUndoRedoStatus();
    this->updateCutCopyRemoveStatus();
    this->updatePasteStatus();

    qDebug() << "Editor actived : " << this;
}
