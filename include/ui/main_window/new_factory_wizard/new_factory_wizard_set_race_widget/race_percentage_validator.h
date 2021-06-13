#pragma once

#include <QtGui/QIntValidator>

/**
 * @brief   Race percentage validator.
 */
class RacePercentageValidator : public QIntValidator {
    Q_OBJECT
  private:
    quint32 &m_total; ///< Total count.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   total       Reference to total count.
     * @param[in]   parent      Parent objuect.
     */
    RacePercentageValidator(quint32 &total, QObject *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~RacePercentageValidator();

  public:
    /**
     * @brief           Fix up.
     *
     * @param[in, out]  input       Input.
     */
    virtual void fixup(QString &input) const override;
};
