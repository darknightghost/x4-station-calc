#include <locale/string_table.h>
#include <ui/main_window/info_widget/info_widget.h>

/**
 * @brief		Constructor.
 */
InfoWidget::InfoWidget(QAction *       statusAction,
                       QWidget *       parent,
                       Qt::WindowFlags flags) :
    ActionControlDockWidget(statusAction, parent, flags)
{
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
}
