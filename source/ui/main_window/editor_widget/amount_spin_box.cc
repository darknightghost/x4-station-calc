#include <QtWidgets/QLineEdit>

#include <ui/main_window/editor_widget/amount_spin_box.h>

/**
 * @brief       Constructor.
 */
AmountSpinBox::AmountSpinBox(QWidget *parent) : QSpinBox(parent)
{
    this->setSingleStep(1);
    this->setValue(1);
    this->setRange(1, 2147483646);

    this->connect(this, QOverload<int>::of(&QSpinBox::valueChanged), this,
                  &AmountSpinBox::onValueChanged);
    this->connect(this, &QSpinBox::editingFinished, this,
                  &AmountSpinBox::onEditingFinished);
}

/**
 * @brief       Destructor.
 */
AmountSpinBox::~AmountSpinBox() {}

/**
 * @brief       Connect to QSpinBox::valueChanged.
 */
void AmountSpinBox::onValueChanged(int)
{
    if (! this->lineEdit()->hasFocus()) {
        emit this->amountEdited(this->value());
    }
}

/**
 * @brief       Connect to QSpinBox::editingFinished.
 */
void AmountSpinBox::onEditingFinished()
{
    emit this->amountEdited(this->value());
}
