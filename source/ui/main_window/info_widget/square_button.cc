#include <QtGui/QResizeEvent>

#include <ui/main_window/info_widget/square_button.h>

/**
 * @brief		Constructor.
 */
SquareButton::SquareButton(const QIcon &icon, QWidget *parent) :
    QPushButton(icon, "", parent)
{
    this->setSizePolicy(QSizePolicy::Policy::Fixed,
                        this->sizePolicy().verticalPolicy());
}

/**
 * @brief		Constructor.
 */
SquareButton::SquareButton(QWidget *parent) : QPushButton(parent) {}

/**
 * @brief		Destructor.
 */
SquareButton::~SquareButton() {}

/**
 * @brief		Resize event.
 */
void SquareButton::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if (sz.width() == sz.height()) {
        QPushButton::resizeEvent(event);
    } else {
        sz.setWidth(sz.height());
        event->ignore();
        this->resize(sz);
    }
}