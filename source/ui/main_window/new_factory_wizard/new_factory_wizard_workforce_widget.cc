#include <algorithm>

#include <QtCore/QList>

#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_workforce_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardWorkforceWidget::NewFactoryWizardWorkforceWidget(
    QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
    NewFactoryWizard *                              wizard) :
    NewFactoryWizardCentralWidget(wizard),
    m_workforceInfo(workforceInfo), m_total(0)
{
    this->setWindowTitle(STR("STR_STEP_SET_WORKFORCE"));

    // Sort.
    QList<QString> races;
    for (auto &key : workforceInfo.keys()) {
        races.push_back(key);
    }

    auto      gameTexts = GameData::instance()->texts();
    auto      gameRaces = GameData::instance()->races();
    QCollator collator  = StringTable::instance()->collator();

    ::std::sort(races.begin(), races.end(),
                [&](const QString &race1, const QString &race2) -> bool {
                    QString name1
                        = gameTexts->text(gameRaces->race(race1).name);
                    QString name2
                        = gameTexts->text(gameRaces->race(race2).name);

                    return collator.compare(name1, name2) < 0;
                });

    // Add widgets.
    QVBoxLayout *layout = new QVBoxLayout();
    this->setLayout(layout);

    QGridLayout *raceLayout = new QGridLayout();
    layout->addLayout(raceLayout);
    for (int i = 0; i < races.count(); ++i) {
        // Label race.
        QLabel *lblRace = new QLabel(QString("%1 :").arg(gameTexts->text(
                                         gameRaces->race(races[i]).name)),
                                     this);
        lblRace->setAlignment(Qt::AlignmentFlag::AlignRight
                              | Qt::AlignmentFlag::AlignVCenter);
        raceLayout->addWidget(lblRace, i, 0);

        // Edit percentage.
        RacePercentageEdit *txtPercentage = new RacePercentageEdit(
            races[i], m_total, m_workforceInfo[races[i]].percentage, this);
        raceLayout->addWidget(txtPercentage, i, 1);
        this->connect(txtPercentage, &RacePercentageEdit::editingFinished, this,
                      &NewFactoryWizardWorkforceWidget::onEditingFinished);

        // Label percentage.
        QLabel *lblPercentage = new QLabel("%", this);
        lblRace->setAlignment(Qt::AlignmentFlag::AlignLeft
                              | Qt::AlignmentFlag::AlignVCenter);
        raceLayout->addWidget(lblPercentage, i, 2);
    }

    layout->addStretch();
    this->updateNextButton();
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardWorkforceWidget::~NewFactoryWizardWorkforceWidget() {}

/**
 * @brief       On editing finished.
 */
void NewFactoryWizardWorkforceWidget::onEditingFinished(
    RacePercentageEdit *edit)
{
    m_workforceInfo[edit->race()].percentage = edit->value();
    this->updateNextButton();
}

/**
 * @brief       Update next button.
 */
void NewFactoryWizardWorkforceWidget::updateNextButton()
{
    if (m_total == 100) {
        this->wizard()->setNextBtnEnabled(true);

    } else {
        this->wizard()->setNextBtnEnabled(false);
    }
}
