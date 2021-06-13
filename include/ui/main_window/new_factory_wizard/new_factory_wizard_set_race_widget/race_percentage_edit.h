#pragma once

#include <QtWidgets/QLineEdit>

#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget/race_percentage_validator.h>

/**
 * @brief   Race percentage editor.
 */
class RacePercentageEdit : public QLineEdit {
    Q_OBJECT

  private:
    QString                  m_race;      ///< Race ID.
    RacePercentageValidator *m_validator; ///< Validator.
    quint32 &                m_total;     ///< Total count.
    quint32                  m_oldValue;  ///< Old value.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   race        Race ID.
     * @param[in]   total       Reference to total count.
     * @param[in]   value       Initialize value.
     * @param[in]   parent      Parent widget.
     */
    RacePercentageEdit(const QString &race,
                       quint32 &      total,
                       quint32        value  = 0,
                       QWidget *      parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~RacePercentageEdit();

  public:
    /**
     * @brief       Get race.
     *
     * @return      Race ID.
     */
    const QString &race() const;

    /**
     * @brief       Get value.
     *
     * @return      Value.
     */
    quint32 value() const;

  signals:
    /**
     * @brief       Emmited when editing finished.
     *
     * @param[in]   edit        The widget which emited this signal.
     */
    void editingFinished(RacePercentageEdit *edit);

  private:
    using QLineEdit::setValidator;

    /**
     * @brief       Update value.
     *
     * @param[in]   value       Value.
     */
    void updateValue(quint32 value);

  private slots:
    /**
     * @brief       On editing finished.
     */
    void onEditingFinished();

  protected:
    /**
     * @brief       Focus in event.
     *
     * @param[in]   e       Event.
     */
    virtual void focusInEvent(QFocusEvent *e) override;
};
