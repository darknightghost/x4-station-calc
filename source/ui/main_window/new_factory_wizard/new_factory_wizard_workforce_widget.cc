#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_workforce_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardWorkforceWidget::NewFactoryWizardWorkforceWidget(
    NewFactoryWizard *wizard) :
    NewFactoryWizardCentralWidget(wizard)
{
    this->setWindowTitle(STR("STR_STEP_SET_WORKFORCE"));
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardWorkforceWidget::~NewFactoryWizardWorkforceWidget() {}
