#pragma once

#include <QtWidgets/QScrollArea>

#include <save/save.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardCentralWidget : public QScrollArea {
    Q_OBJECT

  private:
    NewFactoryWizard *m_wizardWidget; ///< Wizard widget.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   wizard      Wizard widget.
     */
    NewFactoryWizardCentralWidget(NewFactoryWizard *wizard);

    /**
     * @brief       Get wizard.
     *
     * @return      Wizard.
     */
    NewFactoryWizard *wizard();

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardCentralWidget();

  public slots:
    /**
     * @brief     On previous step.
     */
    virtual void onPrevStep();

    /**
     * @brief     On next step.
     */
    virtual void onNextStep();
};
