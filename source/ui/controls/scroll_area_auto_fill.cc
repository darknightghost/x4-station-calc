#include <QtGui/QResizeEvent>

#include <ui/controls/scroll_area_auto_fill.h>

/**
 * @brief       Constructor.
 */
ScrollAreaAutoFill::ScrollAreaAutoFill(QWidget *parent) : QScrollArea(parent) {}

/**
 * @brief       Set the scroll area's widget.
 */
void ScrollAreaAutoFill::setWidget(QWidget *widget)
{
    this->QScrollArea::setWidget(widget);

    if (widget != nullptr) {
        widget->setMinimumSize(this->viewport()->size());
    }
}

/**
 * @brief       Destructor.
 */
ScrollAreaAutoFill::~ScrollAreaAutoFill() {}

/**
 * @brief       Resize event.
 */
void ScrollAreaAutoFill::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    QWidget *childWidget = this->widget();

    if (childWidget != nullptr) {
        childWidget->setMinimumSize(this->viewport()->size());
    }
}
