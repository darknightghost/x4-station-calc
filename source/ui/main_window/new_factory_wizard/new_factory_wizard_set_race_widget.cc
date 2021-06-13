#include <algorithm>

#include <QtCore/QList>

#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardSetRaceWidget::NewFactoryWizardSetRaceWidget(
    QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
    QVector<QString> &                              orderOfProductionMethod,
    NewFactoryWizard *                              wizard) :
    NewFactoryWizardCentralWidget(wizard),
    m_workforceInfo(workforceInfo),
    m_orderOfProductionMethod(orderOfProductionMethod), m_total(0)
{
    this->setWindowTitle(STR("STR_STEP_SET_RACE"));

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
    m_layout = new QVBoxLayout();
    this->setLayout(m_layout);

    m_groupWorkforce = new QGroupBox(STR("STR_RACE_OF_WORKFORCE"));
    m_layout->addWidget(m_groupWorkforce);

    m_workforceLayout = new QGridLayout();
    m_groupWorkforce->setLayout(m_workforceLayout);
    for (int i = 0; i < races.count(); ++i) {
        // Label race.
        QLabel *lblRace = new QLabel(QString("%1 :").arg(gameTexts->text(
                                         gameRaces->race(races[i]).name)),
                                     m_groupWorkforce);
        lblRace->setAlignment(Qt::AlignmentFlag::AlignRight
                              | Qt::AlignmentFlag::AlignVCenter);
        m_workforceLayout->addWidget(lblRace, i, 0);

        // Edit percentage.
        RacePercentageEdit *txtPercentage = new RacePercentageEdit(
            races[i], m_total, m_workforceInfo[races[i]].percentage, this);
        m_workforceLayout->addWidget(txtPercentage, i, 1);
        this->connect(txtPercentage, &RacePercentageEdit::editingFinished, this,
                      &NewFactoryWizardSetRaceWidget::onEditingFinished);

        // Label percentage.
        QLabel *lblPercentage = new QLabel("%", this);
        lblRace->setAlignment(Qt::AlignmentFlag::AlignLeft
                              | Qt::AlignmentFlag::AlignVCenter);
        m_workforceLayout->addWidget(lblPercentage, i, 2);
    }

    QGroupBox *m_groupOrderOfProductionhMethod
        = new QGroupBox(STR("STR_ORDER_OF_PRODUCTION_METHOD"));
    m_layout->addWidget(m_groupOrderOfProductionhMethod);

    m_orderOfProductionMethodLayout = new QHBoxLayout();
    m_groupOrderOfProductionhMethod->setLayout(m_orderOfProductionMethodLayout);
    m_listProductionMethod = new QListWidget();
    m_orderOfProductionMethodLayout->addWidget(m_listProductionMethod);
    for (auto &race : m_orderOfProductionMethod) {
        m_listProductionMethod->addItem(
            gameTexts->text(gameRaces->race(race).name));
    }
    m_listProductionMethod->setSelectionMode(
        QAbstractItemView::SelectionMode::SingleSelection);
    m_listProductionMethod->setCurrentRow(0);
    this->connect(m_listProductionMethod, &QListWidget::currentRowChanged, this,
                  &NewFactoryWizardSetRaceWidget::onCurrentRowChanged);

    m_moveRaceButtonLayout = new QVBoxLayout();
    m_orderOfProductionMethodLayout->addLayout(m_moveRaceButtonLayout);
    m_moveRaceButtonLayout->addStretch();

    m_btnMoveRaceUp = new SquareButton(QIcon(":/Icons/Up.png"));
    m_moveRaceButtonLayout->addWidget(m_btnMoveRaceUp);
    this->connect(m_btnMoveRaceUp, &QPushButton::clicked, this,
                  &NewFactoryWizardSetRaceWidget::onBtnMoveRaceUpClicked);

    m_btnMoveRaceDown = new SquareButton(QIcon(":/Icons/Down.png"));
    m_moveRaceButtonLayout->addWidget(m_btnMoveRaceDown);
    this->connect(m_btnMoveRaceDown, &QPushButton::clicked, this,
                  &NewFactoryWizardSetRaceWidget::onBtnMoveRaceDownClicked);

    m_moveRaceButtonLayout->addStretch();

    this->updateNextButton();
    this->updateMoveButton();
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardSetRaceWidget::~NewFactoryWizardSetRaceWidget() {}

/**
 * @brief       On editing finished.
 */
void NewFactoryWizardSetRaceWidget::onEditingFinished(RacePercentageEdit *edit)
{
    m_workforceInfo[edit->race()].percentage = edit->value();
    this->updateNextButton();
}

/**
 * @brief       On button move race up clicked.
 */
void NewFactoryWizardSetRaceWidget::onBtnMoveRaceUpClicked()
{
    int currentRow = m_listProductionMethod->currentRow();
    if (currentRow > 0) {
        auto *item = m_listProductionMethod->takeItem(currentRow);
        m_listProductionMethod->insertItem(currentRow - 1, item);
        m_listProductionMethod->setCurrentRow(currentRow - 1);

        qSwap(m_orderOfProductionMethod[currentRow],
              m_orderOfProductionMethod[currentRow - 1]);
    }
}

/**
 * @brief       On current row changed.
 */
void NewFactoryWizardSetRaceWidget::onCurrentRowChanged(int)
{
    this->updateMoveButton();
}

/**
 * @brief       On button move race down clicked.
 */
void NewFactoryWizardSetRaceWidget::onBtnMoveRaceDownClicked()
{
    int currentRow = m_listProductionMethod->currentRow();
    if (currentRow < m_listProductionMethod->count() - 1) {
        auto *item = m_listProductionMethod->takeItem(currentRow);
        m_listProductionMethod->insertItem(currentRow + 1, item);
        m_listProductionMethod->setCurrentRow(currentRow + 1);

        qSwap(m_orderOfProductionMethod[currentRow],
              m_orderOfProductionMethod[currentRow + 1]);
    }
}

/**
 * @brief       Update next button.
 */
void NewFactoryWizardSetRaceWidget::updateNextButton()
{
    if (m_total == 100) {
        this->wizard()->setNextBtnEnabled(true);

    } else {
        this->wizard()->setNextBtnEnabled(false);
    }
}

/**
 * @brief       Update move button.
 */
void NewFactoryWizardSetRaceWidget::updateMoveButton()
{
    if (m_listProductionMethod->currentRow() <= 0) {
        m_btnMoveRaceUp->setEnabled(false);

    } else {
        m_btnMoveRaceUp->setEnabled(true);
    }

    if (m_listProductionMethod->currentRow()
        >= m_listProductionMethod->count() - 1) {
        m_btnMoveRaceDown->setEnabled(false);

    } else {
        m_btnMoveRaceDown->setEnabled(true);
    }
}
