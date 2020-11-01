#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QPixmap>
#include <QtGui/QResizeEvent>

#include <ui/controls/square_label.h>

/**
 * @brief		Constructor.
 */
SquareLabel::SquareLabel(const QIcon &icon, QWidget *parent) :
    QLabel("", parent), m_icon(icon), m_iconInitialized(false)
{
    this->setProperty("class", "SquareLabel");
    this->setSizePolicy(QSizePolicy::Policy::Fixed,
                        this->sizePolicy().verticalPolicy());
    this->resize(this->height(), this->height());
}

/**
 * @brief		Constructor.
 */
SquareLabel::SquareLabel(QWidget *parent) : QLabel(parent)
{
    this->setProperty("class", "SquareLabel");
    this->setSizePolicy(QSizePolicy::Policy::Fixed,
                        this->sizePolicy().verticalPolicy());
    this->resize(this->height(), this->height());
}

/**
 * @brief       Set icon.
 */
void SquareLabel::setIcon(const QIcon &icon)
{
    m_icon = icon;
    this->resizeIcon(this->size());
}

/**
 * @brief		Resize icon.
 */
void SquareLabel::resizeIcon()
{
    this->resizeIcon(this->size());
}

/**
 * @brief		Destructor.
 */
SquareLabel::~SquareLabel() {}

/**
 * @brief		Resize event.
 */
void SquareLabel::resizeEvent(QResizeEvent *event)
{
    QSize sz = event->size();
    if (sz.width() == sz.height()) {
        if (sz == this->size() && m_iconInitialized) {
            event->ignore();
            return;
        }

        this->resizeIcon(event->size());
        QLabel::resizeEvent(event);
        m_iconInitialized = true;
    } else {
        sz.setWidth(sz.height());
        event->ignore();
        this->resize(sz);
    }
}

/**
 * @brief		Resize icon.
 */
void SquareLabel::resizeIcon(const QSize &sz)
{
    if (m_icon.isNull()) {
        return;
    }

    QSize   iconSize = m_icon.actualSize(QSize(65535, 65535));
    QPixmap pixmap   = m_icon.pixmap(iconSize);
    this->QLabel::setPixmap(
        pixmap.scaled(sz, Qt::AspectRatioMode::IgnoreAspectRatio,
                      Qt::TransformationMode::SmoothTransformation));

    this->update();
}
