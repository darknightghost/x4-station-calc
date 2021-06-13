#include <QtCore/QDebug>

#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget/race_percentage_edit.h>

/**
 * @brief       Constructor.
 */
RacePercentageEdit::RacePercentageEdit(const QString &race,
                                       quint32 &      total,
                                       quint32        value,
                                       QWidget *      parent) :
    QLineEdit(QString("%1").arg(value), parent),
    m_race(race), m_total(total), m_oldValue(0)
{
    m_validator = new RacePercentageValidator(total, this);
    this->setValidator(m_validator);

    this->connect(this, &QLineEdit::editingFinished, this,
                  &RacePercentageEdit::onEditingFinished);
    this->updateValue(value);
}

/**
 * @brief       Destructor.
 */
RacePercentageEdit::~RacePercentageEdit() {}

/**
 * @brief       Get race.
 */
const QString &RacePercentageEdit::race() const
{
    return m_race;
}

/**
 * @brief       Get value.
 */
quint32 RacePercentageEdit::value() const
{
    return static_cast<quint32>(this->text().toULong());
}

/**
 * @brief       Update value.
 */
void RacePercentageEdit::updateValue(quint32 value)
{
    if (value > m_oldValue) {
        m_total += value - m_oldValue;

    } else if (value < m_oldValue) {
        m_total -= m_oldValue - value;
    }

    m_oldValue = value;
}

/**
 * @brief       On editing finished.
 */
void RacePercentageEdit::onEditingFinished()
{
    this->updateValue(this->value());
    emit this->editingFinished(this);
}

/**
 * @brief       Focus in event.
 */
void RacePercentageEdit::focusInEvent(QFocusEvent *e)
{
    this->setText(QString("%1").arg(100 + m_oldValue - m_total));
    m_validator->setTop(100 + m_oldValue - m_total);
    this->QLineEdit::focusInEvent(e);
}
