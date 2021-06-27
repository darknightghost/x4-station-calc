#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <save/save.h>
#include <ui/controls/square_button.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget/race_percentage_edit.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardSetRaceWidget : public NewFactoryWizardCentralWidget {
    Q_OBJECT

  private:
    QMap<QString, NewFactoryWizard::WorkforceInfo>
        &m_workforceInfo; ///< Workforce information.
    QVector<QString>
        &   m_orderOfProductionMethod; ///< Order of production method.
    quint32 m_total;                   ///< Total count.

    QVBoxLayout *m_layout;          ///< Layout;
    QGroupBox *  m_groupWorkforce;  ///< Group workforce.
    QGridLayout *m_workforceLayout; ///< Workforce layout.

    QGroupBox
        *m_groupOrderOfProductionhMethod; ///< Group order of production method.
    QHBoxLayout *
        m_orderOfProductionMethodLayout; ///< Layout order of production method.
    QListWidget * m_listProductionMethod; ///< List of production medhod.
    QVBoxLayout * m_moveRaceButtonLayout; ///< Layout of move race buttons.
    SquareButton *m_btnMoveRaceUp;        ///< Button move race up.
    SquareButton *m_btnMoveRaceDown;      ///< Button move race down.

  public:
    /**
     * @brief           Constructor.
     *
     * @param[in,out]   workforceInfo           Workforce information.
     * @param[out]      orderOfProductionMethod Order of production method.
     * @param[in]       wizard                  Wizard widget.
     */
    NewFactoryWizardSetRaceWidget(
        QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
        QVector<QString> &                              orderOfProductionMethod,
        NewFactoryWizard *                              wizard);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardSetRaceWidget();

  private slots:
    /**
     * @brief       On editing finished.
     *
     * @param[in]   edit        The widget which emited this signal.
     */
    void onEditingFinished(RacePercentageEdit *edit);

    /**
     * @brief       On button move race up clicked.
     */
    void onBtnMoveRaceUpClicked();

    /**
     * @brief       On button move race down clicked.
     */
    void onBtnMoveRaceDownClicked();

    /**
     * @brief       On current row changed.
     */
    void onCurrentRowChanged(int);

  private:
    /**
     * @brief       Update next button.
     */
    void updateNextButton();

    /**
     * @brief       Update move button.
     */
    void updateMoveButton();
};
