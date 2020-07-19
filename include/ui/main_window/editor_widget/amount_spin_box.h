#pragma once

#include <QtWidgets/QSpinBox>

/**
 * @breif   Spin box for module amount.
 */
class AmountSpinBox : public QSpinBox {
    Q_OBJECT;

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent object.
     */
    AmountSpinBox(QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~AmountSpinBox();

  signals:
    /**
     * @brief   Emitted when amount edited.
     */
    void amountEdited(int i);

  private slots:
    /**
     * @brief       Connect to QSpinBox::valueChanged.
     *
     * @param[in]   i       New value.
     */
    void onValueChanged(int i);

    /**
     * @brief       Connect to QSpinBox::editingFinished.
     */
    void onEditingFinished();
};
