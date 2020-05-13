#include <locale/string_table.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief		Constructor.
 */
StationModulesWidget::StationModulesWidget(QAction *       statusAction,
                                           QWidget *       parent,
                                           Qt::WindowFlags flags) :
    ActionControlDockWidget(statusAction, parent, flags)
{
    // Style
    // this->setWindowFlags(Qt::WindowType::Tool);
    this->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable
                      | QDockWidget::DockWidgetFeature::DockWidgetMovable);

    // Load widgets
    m_widget = new QWidget(this);

    m_layout = new QVBoxLayout(m_widget);
    m_widget->setLayout(m_layout);

    // Button bar.
    m_widgetButtonBar = new QWidget(m_widget);
    m_layout->addWidget(m_widgetButtonBar);

    m_layoutButtonBar = new QHBoxLayout(m_widgetButtonBar);
    m_widgetButtonBar->setLayout(m_layoutButtonBar);

    m_btnShowHideFilter = new QPushButton(m_widgetButtonBar);
    m_layoutButtonBar->addWidget(m_btnShowHideFilter);
    this->connect(m_btnShowHideFilter, &QPushButton::clicked, this,
                  &StationModulesWidget::onBtnShowHideFilterWidgetClicked);

    m_layoutButtonBar->addStretch();

    m_btnAddToStation = new QPushButton(m_widgetButtonBar);
    m_layoutButtonBar->addWidget(m_btnAddToStation);

    // Filters.
    m_widgetFilters = new QWidget(m_widget);
    m_layout->addWidget(m_widgetFilters);
    m_widgetFilters->setVisible(false);

    m_layoutFilters = new QGridLayout(m_widgetFilters);

    m_chkByRace = new QCheckBox(m_widgetFilters);
    m_chkByRace->setChecked(false);
    m_chkByRace->setTristate(false);
    m_layoutFilters->addWidget(m_chkByRace, 0, 0);
    m_comboByRaces = new QComboBox(m_widgetFilters);
    m_layoutFilters->addWidget(m_comboByRaces, 0, 1);

    m_chkByProduction = new QCheckBox(m_widgetFilters);
    m_chkByProduction->setChecked(false);
    m_chkByProduction->setTristate(false);
    m_layoutFilters->addWidget(m_chkByProduction, 1, 0);
    m_comboByProduction = new QComboBox(m_widgetFilters);
    m_layoutFilters->addWidget(m_comboByProduction, 1, 1);

    m_chkByKeyword = new QCheckBox(m_widgetFilters);
    m_chkByKeyword->setChecked(false);
    m_chkByKeyword->setTristate(false);
    m_layoutFilters->addWidget(m_chkByKeyword, 2, 0);
    m_txtKeyword = new QLineEdit(m_widgetFilters);
    m_layoutFilters->addWidget(m_txtKeyword, 2, 1);

    // Station modules.
    m_treeStationModules = new QTreeWidget(m_widget);
    m_layout->addWidget(m_treeStationModules);

    this->setWidget(m_widget);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &StationModulesWidget::onLanguageChanged);

    this->onLanguageChanged();
}

/**
 * @brief		Destructor.
 */
StationModulesWidget::~StationModulesWidget() {}

/**
 * @brief		Change language.
 */
void StationModulesWidget::onLanguageChanged()
{
    // Title
    this->setWindowTitle(STR("STR_STATION_MODULE_WIDGET_TITLE"));

    // Button bar.
    if (m_widgetFilters->isVisible()) {
        m_btnShowHideFilter->setText(STR("STR_HIDE_FILTER"));

    } else {
        m_btnShowHideFilter->setText(STR("STR_SHOW_FILTER"));
    }
    m_btnAddToStation->setText(STR("STR_ADD_TO_STATION"));

    // Filters.
    m_chkByRace->setText(STR("STR_BY_RACE"));
    m_chkByProduction->setText(STR("STR_BY_PRODUCT"));
    m_chkByKeyword->setText(STR("STR_BY_KEYWORD"));
}

/**
 * @brief		\c m_btnShowHideFilter is clicked.
 */
void StationModulesWidget::onBtnShowHideFilterWidgetClicked()
{
    // Button bar.
    if (m_widgetFilters->isVisible()) {
        m_widgetFilters->setVisible(false);
        m_btnShowHideFilter->setText(STR("STR_SHOW_FILTER"));

    } else {
        m_widgetFilters->setVisible(true);
        m_btnShowHideFilter->setText(STR("STR_HIDE_FILTER"));
    }
}
