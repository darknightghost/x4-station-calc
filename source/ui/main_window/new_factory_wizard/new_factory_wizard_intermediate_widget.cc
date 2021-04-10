#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_intermediate_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardIntermediateWidget::NewFactoryWizardIntermediateWidget(
    NewFactoryWizard *wizard) :
    NewFactoryWizardCentralWidget(wizard)
{
    this->setWindowTitle(STR("STR_STEP_SET_INTERMEDIATE"));
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardIntermediateWidget::~NewFactoryWizardIntermediateWidget() {}
