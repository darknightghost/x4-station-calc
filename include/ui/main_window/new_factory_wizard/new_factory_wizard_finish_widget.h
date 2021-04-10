#pragma once

#include <QtWidgets/QWidget>

#include <save/save.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardFinishWidget : public NewFactoryWizardCentralWidget {
    Q_OBJECT

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   wizard      Wizard widget.
     */
    NewFactoryWizardFinishWidget(NewFactoryWizard *wizard);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardFinishWidget();
};
