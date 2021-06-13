#include <QtCore/QDebug>

#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget/race_percentage_validator.h>

/**
 * @brief       Constructor.
 */
RacePercentageValidator::RacePercentageValidator(quint32 &total,
                                                 QObject *parent) :
    QIntValidator(0, 100 - total, parent),
    m_total(total)
{}

/**
 * @brief       Destructor.
 */
RacePercentageValidator::~RacePercentageValidator() {}

/**
 * @brief           Fix up.
 */
void RacePercentageValidator::fixup(QString &input) const
{
    input = QString("%1").arg(100 - m_total);
}
