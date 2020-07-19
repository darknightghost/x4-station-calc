#include <QtCore/QFile>
#include <QtGui/QResizeEvent>

#include <ui/controls/square_button.h>

/**
 * @brief		Constructor.
 */
SquareButton::SquareButton(const QIcon &icon, QWidget *parent) :
    QPushButton(icon, "", parent)
{
    // Set style sheet
    QFile styleFile(":/StyleSheet/square_button.qss");
    styleFile.open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::Text);
    this->setStyleSheet(styleFile.readAll());

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
