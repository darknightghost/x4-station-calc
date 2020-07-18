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
                           QMdiSubWindow *          parent) :
    QWidget(parent),
    m_save(save), m_dirty(false), m_editActions(editActions),
    m_backgroundTasks(new BackgroundTask(BackgroundTask::RunType::Newest, this))
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
    m_layout->addWidget(m_treeEditor);
    this->connect(m_treeEditor, &QTreeWidget::itemChanged, this,
                  &EditorWidget::onItemChanged);

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
        m_dirty = true;
        this->updateUndoRedoStatus();

    } else {
        qDebug() << "Operation failed.";
    }
}

/**
 * @brief       Update newGroup action statis.
 */
void EditorWidget::updateNewGroup() {}

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
 * @brief       Update cut/copy action statis.
 */
void EditorWidget::updateCutCopyStatus() {}

/**
 * @brief       Update paste action statis.
 */
void EditorWidget::updatePasteStatus() {}

/**
 * @brief       Update remove action statis.
 */
void EditorWidget::updateRemoveStatus() {}

/**
 * @brief	Close save file.
 */
bool EditorWidget::closeSave()
{
    if (m_save == nullptr) {
        return true;
    }
    if (m_dirty) {
        this->save();
        if (m_dirty) {
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
    for (::std::shared_ptr<SaveGroup> group : m_save->groups()) {
        GroupItem *item = new GroupItem(group);
        m_itemGroups->addChild(item);
        GroupItemWidget *widget = new GroupItemWidget(item);
        item->treeWidget()->setItemWidget(item, 1, widget);
        widget->show();
        m_groupItems[item] = widget;
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
void EditorWidget::remove() {}

/**
 * @brief		Save.
 */
void EditorWidget::save() {}

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
    if (m_groupItems.find(static_cast<GroupItem *>(item)) != m_groupItems.end()
        && column == 0) {
        int        index     = m_itemGroups->indexOfChild(item);
        GroupItem *groupItem = static_cast<GroupItem *>(item);
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
 * @brief       Active editor widget.
 */
void EditorWidget::active()
{
    // Connect actions.
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
    this->updateNewGroup();
    this->updateUndoRedoStatus();
    this->updateCutCopyStatus();
    this->updatePasteStatus();
    this->updateRemoveStatus();

    qDebug() << "Editor actived : " << this;
}

/**
 * @brief       Get group item by index.
 */
GroupItem *EditorWidget::getGroupItemByIndex(int index)
{
    return static_cast<GroupItem *>(m_itemGroups->child(index));
}
