#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QPixmap>
#include <QtGui/QResizeEvent>

#include <ui/controls/square_button.h>

/**
 * @brief		Constructor.
 */
SquareButton::SquareButton(const QIcon &icon, QWidget *parent) :
    QPushButton("", parent), m_icon(icon)
{
    // Set style sheet
    QFile styleFile(":/StyleSheet/square_button.qss");
    styleFile.open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::Text);
    this->setStyleSheet(styleFile.readAll());

    this->setSizePolicy(QSizePolicy::Policy::Fixed,
                        this->sizePolicy().verticalPolicy());

    this->resizeIcon(this->size());
}

/**
 * @brief		Constructor.
 */
SquareButton::SquareButton(QWidget *parent) : QPushButton(parent)
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
 * @brief       Set icon.
 */
void SquareButton::setIcon(const QIcon &icon)
{
    m_icon = icon;
    this->resizeIcon(this->size());
}

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
        this->resizeIcon(event->size());
    } else {
        sz.setWidth(sz.height());
        event->ignore();
        this->resize(sz);
    }
}

/**
 * @brief		Resize icon.
 */
void SquareButton::resizeIcon(const QSize &sz)
{
    if (m_icon.isNull()) {
        return;
    }

    QIcon scaledIcon;
    for (auto mode : {QIcon::Mode::Normal, QIcon::Mode::Disabled,
                      QIcon::Mode::Active, QIcon::Mode::Selected}) {
        QSize   iconSize = m_icon.actualSize(QSize(65535, 65535), mode);
        QPixmap pixmap   = m_icon.pixmap(iconSize, mode);
        scaledIcon.addPixmap(
            pixmap.scaled(sz, Qt::AspectRatioMode::IgnoreAspectRatio,
                          Qt::TransformationMode::SmoothTransformation),
            mode);
    }

    this->QPushButton::setIcon(scaledIcon);
}
