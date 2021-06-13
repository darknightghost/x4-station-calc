#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardCentralWidget::NewFactoryWizardCentralWidget(
    NewFactoryWizard *wizard) :
    QScrollArea(nullptr),
    m_wizardWidget(wizard)
{}

/**
 * @brief       Get wizard.
 */
NewFactoryWizard *NewFactoryWizardCentralWidget::wizard()
{
    return m_wizardWidget;
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardCentralWidget::~NewFactoryWizardCentralWidget() {}

/**
 * @brief     On previous step.
 */
void NewFactoryWizardCentralWidget::onPrevStep() {}

/**
 * @brief     On next step.
 */
void NewFactoryWizardCentralWidget::onNextStep() {}
