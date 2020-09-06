#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <locale/string_table.h>
#include <ui/main_window/editor_widget/editor_widget.h>

QMap<QString, EditorWidget *> EditorWidget::_opendFiles; ///< Opened files.

/**
 * @brief       Get editor widget by path.
 */
EditorWidget *EditorWidget::getEditorWidgetByPath(const QString &path)
{
    auto iter = _opendFiles.find(QDir(".").absoluteFilePath(path));
    if (iter == _opendFiles.end()) {
        return nullptr;

    } else {
        return *iter;
    }
}

/**
 * @brief		Constructor.
 */
EditorWidget::EditorWidget(::std::shared_ptr<Save>  save,
                           MainWindow::FileActions *fileActions,
                           MainWindow::EditActions *editActions,
                           InfoWidget *             infoWidget,
                           StationModulesWidget *   stationModulesWidget,
                           QMdiSubWindow *          parent) :
    QWidget(parent),
    m_stationModulesWidget(stationModulesWidget), m_infoWidget(infoWidget),
    m_save(save), m_savedUndoCount(0), m_fileActions(fileActions),
    m_editActions(editActions), m_backgroundTasks(new BackgroundTask(
                                    BackgroundTask::RunType::Newest, this)),
    m_treeEditor(nullptr)
{
    this->connect(this, &EditorWidget::windowTitleChanged, parent,
                  &QMdiSubWindow::setWindowTitle);
    this->updateTitle();

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
    this->connect(m_treeEditor, &QTreeWidget::itemSelectionChanged, this,
                  &EditorWidget::updateAddToStationStatus);

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

    // Add to container.
    parent->setWidget(this);
    parent->show();
    this->show();

    this->setAttribute(Qt::WA_DeleteOnClose);
    parent->setAttribute(Qt::WA_DeleteOnClose);

    if (m_save->path() != "") {
        _opendFiles[m_save->path()] = this;
    }
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
        this->updateSaveStatus();
        this->updateUndoRedoStatus();

    } else {
        qDebug() << "Operation failed.";
    }
}

/**
 * @brief       Update window title.
 */
void EditorWidget::updateTitle()
{
    if (m_save->path() == "") {
        this->setWindowTitle(STR("STR_NEW_STATION"));
    } else {
        this->setWindowTitle(
            m_save->path().split("/", Qt::SkipEmptyParts).back());
    }
}

/**
 * @brief       Update button "Save" status.
 */
void EditorWidget::updateSaveStatus()
{
    if (m_savedUndoCount == m_undoStack.size()) {
        m_fileActions->actionFileSave->setEnabled(false);
    } else {
        m_fileActions->actionFileSave->setEnabled(true);
    }
}

/**
 * @brief       Update button "Add to Station" status.
 */
void EditorWidget::updateAddToStationStatus()
{
    if (m_itemGroups->childCount() == 0) {
        emit this->addToStationStatusChaged(false);
    } else {
        emit this->addToStationStatusChaged(true);
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
 * @brief       Update move button status.
 */
void EditorWidget::updateGroupMoveButtonStatus(GroupItem *      item,
                                               GroupItemWidget *itemWidget)
{
    int index = m_itemGroups->indexOfChild(item);

    if (itemWidget == nullptr) {
        itemWidget = dynamic_cast<GroupItemWidget *>(
            m_treeEditor->itemWidget(item, 1));
        Q_ASSERT(itemWidget != nullptr);
    }

    if (index == 0) {
        if (index == m_itemGroups->childCount() - 1) {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(false);

        } else {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(true);
        }

    } else if (index == m_itemGroups->childCount() - 1) {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(false);

    } else {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(true);
    }
}

/**
 * @brief       Update move button status.
 */
void EditorWidget::updateModuleMoveButtonStatus(ModuleItem *      item,
                                                ModuleItemWidget *itemWidget)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);
    int index = groupItem->indexOfChild(item);

    if (itemWidget == nullptr) {
        itemWidget = dynamic_cast<ModuleItemWidget *>(
            m_treeEditor->itemWidget(item, 1));
        Q_ASSERT(itemWidget != nullptr);
    }

    if (index == 0) {
        if (index == groupItem->childCount() - 1) {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(false);

        } else {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(true);
        }

    } else if (index == groupItem->childCount() - 1) {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(false);

    } else {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(true);
    }
}

/**
 * @brief	Close save file.
 */
bool EditorWidget::closeSave()
{
    if (m_save == nullptr) {
        return true;
    }

    if (m_savedUndoCount == m_undoStack.size()) {
        return true;

    } else {
        switch (QMessageBox::question(
            this, STR("STR_TITLE_SAVE_STATION"),
            STR("STR_SAVE_STATION").arg(this->windowTitle()),
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
                | QMessageBox::StandardButton::Cancel,
            QMessageBox::StandardButton::Yes)) {
            case QMessageBox::StandardButton::Yes:
                this->save();
                return m_savedUndoCount == m_undoStack.size();
                break;

            case QMessageBox::StandardButton::No:
                m_savedUndoCount = m_undoStack.size();
                return true;
                break;

            case QMessageBox::StandardButton::Cancel:
            default:
                return false;
        }
    }
}

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
        this->connect(groupWidget, &GroupItemWidget::upBtnClicked, this,
                      &EditorWidget::onGroupMoveUp);
        this->connect(groupWidget, &GroupItemWidget::downBtnClicked, this,
                      &EditorWidget::onGroupMoveDown);
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

            this->connect(moduleWidget, &ModuleItemWidget::changeAmount, this,
                          &EditorWidget::onChangeAmount);
            this->connect(moduleWidget, &ModuleItemWidget::upBtnClicked, this,
                          &EditorWidget::onModuleMoveUp);
            this->connect(moduleWidget, &ModuleItemWidget::downBtnClicked, this,
                          &EditorWidget::onModuleMoveDown);
            this->connect(moduleWidget, &ModuleItemWidget::removeBtnClicked,
                          this, &EditorWidget::removeModuleItem);
        }

        ModuleItem *first = dynamic_cast<ModuleItem *>(groupItem->child(0));
        if (first != nullptr) {
            this->updateModuleMoveButtonStatus(first);
        }

        ModuleItem *last = dynamic_cast<ModuleItem *>(
            groupItem->child(groupItem->childCount() - 1));
        if (last != nullptr) {
            this->updateModuleMoveButtonStatus(last);
        }
    }

    GroupItem *first = dynamic_cast<GroupItem *>(m_itemGroups->child(0));
    if (first != nullptr) {
        this->updateGroupMoveButtonStatus(first);
    }

    GroupItem *last = dynamic_cast<GroupItem *>(
        m_itemGroups->child(m_itemGroups->childCount() - 1));
    if (last != nullptr) {
        this->updateGroupMoveButtonStatus(last);
    }
}

/**
 * @brief		Close event.
 */
void EditorWidget::closeEvent(QCloseEvent *event)
{
    if (this->closeSave()) {
        _opendFiles.remove(m_save->path());
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief	    Add module.
 */
void EditorWidget::addModules(const QStringList &macros)
{
    // Get item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        m_itemGroups->child(m_itemGroups->childCount() - 1));
    Q_ASSERT(groupItem != nullptr);
    ModuleItem *moduleItem = nullptr;

    QTreeWidgetItem *item = m_treeEditor->currentItem();
    if (item != nullptr) {
        if (dynamic_cast<GroupItem *>(item) != nullptr) {
            groupItem = static_cast<GroupItem *>(item);

        } else if (dynamic_cast<ModuleItem *>(item) != nullptr) {
            moduleItem = static_cast<ModuleItem *>(item);
            groupItem  = static_cast<GroupItem *>(item->parent());
        }
    }

    int groupIndex  = m_itemGroups->indexOfChild(groupItem);
    int moduleIndex = 0;
    if (moduleItem == nullptr) {
        moduleIndex = groupItem->childCount();
    } else {
        moduleIndex = groupItem->indexOfChild(moduleItem) + 1;
    }

    // Do operation.
    ::std::shared_ptr<Operation> operation
        = AddModuleOperation::create(groupIndex, moduleIndex, macros, this);
    this->doOperation(operation);
}

/**
 * @brief		Create new group.
 */
void EditorWidget::newGroup()
{
    // Get index.
    int index = m_itemGroups->childCount();

    QTreeWidgetItem *item = m_treeEditor->currentItem();
    if (item != nullptr) {
        if (dynamic_cast<GroupItem *>(item) != nullptr) {
            index = m_itemGroups->indexOfChild(item) + 1;

        } else if (dynamic_cast<ModuleItem *>(item) != nullptr) {
            index = m_itemGroups->indexOfChild(item->parent()) + 1;
        }
    }

    // Do operation.
    ::std::shared_ptr<Operation> operation
        = AddGroupOperation::create(index, this);
    this->doOperation(operation);

    this->updateAddToStationStatus();
}

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

    this->updateSaveStatus();
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

    this->updateAddToStationStatus();
}

/**
 * @brief		Remove group item.
 */
void EditorWidget::removeGroupItem(GroupItem *item)
{
    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create({item}, {}, this);
    this->doOperation(operation);

    this->updateAddToStationStatus();
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
    if (m_save->path() == "") {
        this->saveAs();

    } else {
        if (m_save->write()) {
            m_savedUndoCount = m_undoStack.size();
            qDebug() << "File" << this->windowTitle() << "Saved.";
            this->updateSaveStatus();
        } else {
            QMessageBox::critical(this, STR("STR_ERROR"),
                                  STR("STR_ERR_SAVE").arg(m_save->path()));
        }
    }
}

/**
 * @brief		Save as.
 */
void EditorWidget::saveAs()
{
    // Get path to save.
    QString fileName = QFileDialog::getSaveFileName(
        this, STR("STR_TITLE_SAVE_STATION"),
        Config::instance()->getString(
            "/savePath",
            Config::instance()->getString("/openPath", QDir::homePath())),
        STR("STR_SAVE_FILE_FILTER"));

    if (fileName == "") {
        return;
    }

    QString dir = QDir(fileName).absolutePath();
    dir         = dir.left(dir.lastIndexOf("/"));
    Config::instance()->setString("/savePath", dir);

    QString oldPath = m_save->path();

    // Save file.
    if (m_save->write(fileName)) {
        m_savedUndoCount = m_undoStack.size();
        qDebug() << "File" << this->windowTitle() << "Saved.";
        this->updateTitle();
        this->updateSaveStatus();
        _opendFiles.remove(oldPath);
        _opendFiles[m_save->path()] = this;
    } else {
        QMessageBox::critical(this, STR("STR_ERROR"),
                              STR("STR_ERR_SAVE").arg(fileName));
    }
}

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
 * @brief		Called when move up button of a group item clicked.
 */
void EditorWidget::onGroupMoveUp(GroupItem *item)
{
    qDebug() << "onGroupMoveUp() : " << item;

    int oldIndex = m_itemGroups->indexOfChild(item);
    Q_ASSERT(oldIndex > 0);
    int index = oldIndex - 1;

    ::std::shared_ptr<Operation> operation
        = MoveGroupOperation::create(oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move down button of a group item clicked.
 */
void EditorWidget::onGroupMoveDown(GroupItem *item)
{
    qDebug() << "onGroupMoveDown() : " << item;

    int oldIndex = m_itemGroups->indexOfChild(item);
    Q_ASSERT(oldIndex < m_itemGroups->childCount() - 1);
    int index = oldIndex + 1;

    ::std::shared_ptr<Operation> operation
        = MoveGroupOperation::create(oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move up button of a module item clicked.
 */
void EditorWidget::onModuleMoveUp(ModuleItem *item)
{
    qDebug() << "onModuleMoveUp() : " << item;

    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);

    int groupIndex = m_itemGroups->indexOfChild(groupItem);

    int oldIndex = groupItem->indexOfChild(item);
    Q_ASSERT(oldIndex > 0);
    int index = oldIndex - 1;

    ::std::shared_ptr<Operation> operation
        = MoveModuleOperation::create(groupIndex, oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move up button of a module item clicked.
 */
void EditorWidget::onModuleMoveDown(ModuleItem *item)
{
    qDebug() << "onModuleMoveDown() : " << item;

    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);

    int groupIndex = m_itemGroups->indexOfChild(groupItem);

    int oldIndex = groupItem->indexOfChild(item);
    Q_ASSERT(oldIndex < groupItem->childCount() - 1);
    int index = oldIndex + 1;

    ::std::shared_ptr<Operation> operation
        = MoveModuleOperation::create(groupIndex, oldIndex, index, this);

    this->doOperation(operation);
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

    this->disconnect(m_fileActions->actionFileSave, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileSave, &QAction::triggered, this,
                  &EditorWidget::save);

    this->disconnect(m_fileActions->actionFileSaveAs, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileSaveAs, &QAction::triggered, this,
                  &EditorWidget::saveAs);

    this->disconnect(m_fileActions->actionFileClose, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileClose, &QAction::triggered, this,
                  &EditorWidget::close);

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
    m_fileActions->actionFileSaveAs->setEnabled(true);
    m_fileActions->actionFileClose->setEnabled(true);
    m_editActions->actionEditNewGroup->setEnabled(true);
    this->updateSaveStatus();
    this->updateAddToStationStatus();
    this->updateUndoRedoStatus();
    this->updateCutCopyRemoveStatus();
    this->updatePasteStatus();

    qDebug() << "Editor actived : " << this;
}
