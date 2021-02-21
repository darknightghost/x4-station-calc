#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>

#include <ui/main_window/editor_widget/amount_spin_box.h>

/**
 * @brief       Constructor.
 */
AmountSpinBox::AmountSpinBox(QWidget *parent) : QWidget(parent)
{
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    this->setLayout(m_layout);

    m_txtAmount = new QLineEdit("1");
    m_oldValue  = 1;
    m_layout->addWidget(m_txtAmount);
    this->connect(m_txtAmount, &QLineEdit::editingFinished, this,
                  &AmountSpinBox::onEditingFinished);
    m_validAmount = new QIntValidator(1, 65535, m_txtAmount);
    m_txtAmount->setValidator(m_validAmount);

    m_layoutButton = new QVBoxLayout();
    m_layout->addLayout(m_layoutButton);
    m_layoutButton->setMargin(0);
    m_layoutButton->setSpacing(0);

    m_btnInc = new SquareButton(QIcon(":/Icons/Up.png"));
    m_layoutButton->addWidget(m_btnInc);
    this->connect(m_btnInc, &QPushButton::clicked, this,
                  &AmountSpinBox::onBtnIncClicked);

    m_btnDec = new SquareButton(QIcon(":/Icons/Down.png"));
    m_layoutButton->addWidget(m_btnDec);
    this->connect(m_btnDec, &QPushButton::clicked, this,
                  &AmountSpinBox::onBtnDecClicked);
}

/**
 * @brief       Set value.
 */
void AmountSpinBox::setValue(int value)
{
    m_oldValue = (quint32)value;
    m_txtAmount->setText(QString("%1").arg(value));
    this->updateBtnStatus();
}

/**
 * @brief       Destructor.
 */
AmountSpinBox::~AmountSpinBox() {}

/**
 * @brief       On m_btnInc clicked.
 */
void AmountSpinBox::onBtnIncClicked()
{
    int num = m_txtAmount->text().toInt();
    ++num;
    m_txtAmount->setText(QString("%1").arg(num));
    this->emitEdited();
}

/**
 * @brief       On m_btnDec clicked.
 */
void AmountSpinBox::onBtnDecClicked()
{
    int num = m_txtAmount->text().toInt();
    --num;
    m_txtAmount->setText(QString("%1").arg(num));
    this->emitEdited();
}

/**
 * @brief       Connect to QSpinBox::editingFinished.
 */
void AmountSpinBox::onEditingFinished()
{
    this->emitEdited();
}

/**
 * @brief       Emit edited signal.
 */
void AmountSpinBox::emitEdited()
{
    int num = m_txtAmount->text().toInt();
    if (m_oldValue != num)
    {
        m_oldValue = num;
        emit this->amountEdited(num);
        this->updateBtnStatus();
    }
}

/**
 * @brief       Update button status.
 */
void AmountSpinBox::updateBtnStatus()
{
    int num = m_txtAmount->text().toInt();
    if (num <= m_validAmount->bottom())
    {
        m_btnDec->setEnabled(false);
    }
    else
    {
        m_btnDec->setEnabled(true);
    }

    if (num >= m_validAmount->top())
    {
        m_btnInc->setEnabled(false);
    }
    else
    {
        m_btnInc->setEnabled(true);
    }
}
