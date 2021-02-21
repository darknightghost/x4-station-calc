#pragma once

#include <QtGui/QIntValidator>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <ui/controls/square_button.h>

/**
 * @brief   Spin box for module amount.
 */
class AmountSpinBox : public QWidget {
    Q_OBJECT;

  private:
    QHBoxLayout *m_layout; ///< Layout.

    QLineEdit *    m_txtAmount;   ///< Text amount.
    QIntValidator *m_validAmount; ///< Amount valudator.

    QVBoxLayout * m_layoutButton; ///< Button layout.
    SquareButton *m_btnInc;       ///< Move up button.
    SquareButton *m_btnDec;       ///< Decrease button.

    int m_oldValue; ///< Old value.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent object.
     */
    AmountSpinBox(QWidget *parent = nullptr);

    /**
     * @brief       Set value.
     *
     * @param[in]   value       Value.
     */
    void setValue(int value);

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
     * @brief       On m_btnInc clicked.
     */
    void onBtnIncClicked();

    /**
     * @brief       On m_btnDec clicked.
     */
    void onBtnDecClicked();

    /**
     * @brief       Connect to QSpinBox::editingFinished.
     */
    void onEditingFinished();

  private:
    /**
     * @brief       Emit edited signal.
     */
    void emitEdited();

    /**
     * @brief       Update button status.
     */
    void updateBtnStatus();
};
