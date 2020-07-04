#include <QtGui/QCloseEvent>
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
    m_layout->addWidget(m_treeEditor);

    // Items.
    m_itemModules = new QTreeWidgetItem();
    m_itemModules->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemModules);

    m_itemSummary = new QTreeWidgetItem();
    m_itemSummary->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemSummary);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &EditorWidget::onLanguageChanged);

    this->onLanguageChanged();
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
    m_operationStack.push_back(operation);
    operation->doOperation();
}

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
void EditorWidget::undo() {}

/**
 * @brief		Redo.
 */
void EditorWidget::redo() {}

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
    m_itemModules->setText(0, STR("STR_STATION_MODULES"));
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
