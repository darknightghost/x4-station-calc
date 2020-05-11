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
}
