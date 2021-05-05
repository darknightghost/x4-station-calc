#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <save/save.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_workforce_widget/race_percentage_edit.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardWorkforceWidget : public NewFactoryWizardCentralWidget {
    Q_OBJECT

  private:
    QMap<QString, NewFactoryWizard::WorkforceInfo>
        &   m_workforceInfo; ///< Workforce information.
    quint32 m_total;         ///< Total count.

  public:
    /**
     * @brief           Constructor.
     *
     * @param[in,out]   workforceInfo   Workforce information.
     * @param[in]       wizard          Wizard widget.
     */
    NewFactoryWizardWorkforceWidget(
        QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
        NewFactoryWizard *                              wizard);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardWorkforceWidget();

  private slots:
    /**
     * @brief       On editing finished.
     *
     * @param[in]   edit        The widget which emited this signal.
     */
    void onEditingFinished(RacePercentageEdit *edit);
};
