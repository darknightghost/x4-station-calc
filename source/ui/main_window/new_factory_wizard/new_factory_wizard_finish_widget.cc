#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_finish_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardFinishWidget::NewFactoryWizardFinishWidget(
    NewFactoryWizard *wizard) :
    NewFactoryWizardCentralWidget(wizard)
{
    this->setWindowTitle(STR("STR_STEP_FINISH"));
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardFinishWidget::~NewFactoryWizardFinishWidget() {}
