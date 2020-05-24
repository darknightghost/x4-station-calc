#include <QtGui/QIcon>
#include <QtWidgets/QHeaderView>

#include <locale/string_table.h>
#include <ui/main_window/info_widget/info_widget.h>

/**
 * @brief		Constructor.
 */
InfoWidget::InfoWidget(QAction *       statusAction,
                       QWidget *       parent,
                       Qt::WindowFlags flags) :
    ActionControlDockWidget(statusAction, parent, flags),
    m_historyIndex(0)
{
    m_widget = new QWidget(this);
    this->setWidget(m_widget);

    m_layout = new QGridLayout(m_widget);
    m_widget->setLayout(m_layout);

    m_btnBack = new SquareButton(QIcon(":/Icons/Back.png"), m_widget);
    m_layout->addWidget(m_btnBack, 0, 0);
    m_btnBack->setEnabled(false);

    m_btnForward = new SquareButton(QIcon(":/Icons/Forward.png"), m_widget);
    m_layout->addWidget(m_btnForward, 0, 1);
    m_btnForward->setEnabled(false);

    m_treeInfo = new QTreeWidget(m_widget);
    m_layout->addWidget(m_treeInfo, 1, 0, 1, 3);
    m_treeInfo->header()->setVisible(false);
    m_treeInfo->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);

    // Style
    this->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable
                      | QDockWidget::DockWidgetFeature::DockWidgetMovable);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &InfoWidget::onLanguageChanged);

    this->onLanguageChanged();
}

/**
 * @brief		Destructor.
 */
InfoWidget::~InfoWidget() {}

/**
 * @brief		Change language.
 */
void InfoWidget::onLanguageChanged()
{
    // Title
    this->setWindowTitle(STR("STR_INFO_WIDGET_TITLE"));
    this->showInfo();
}

/**
 * @brief		Show information of station module.
 */
void InfoWidget::showStationModuleInfo(QString macro, bool keepPrevious)
{
    if (! keepPrevious) {
        m_history.clear();
    } else {
        m_history.erase(m_history.begin() + m_historyIndex, m_history.end());
    }

    History history = {HistoryType::StationModule, macro};
    m_history.push_back(history);
    m_historyIndex = m_history.size() - 1;

    this->showInfo();
}

/**
 * @brief		Show information of ware.
 */
void InfoWidget::showWareInfo(QString id, bool keepPrevious)
{
    if (! keepPrevious) {
        m_history.clear();
    } else {
        m_history.erase(m_history.begin() + m_historyIndex, m_history.end());
    }

    History history = {HistoryType::Ware, id};
    m_history.push_back(history);
    m_historyIndex = m_history.size() - 1;

    this->showInfo();
}

/**
 * @brief		Show information.
 */
void InfoWidget::showInfo()
{
    m_treeInfo->clear();
}
