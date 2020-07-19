#include <QtCore/QFile>
#include <QtCore/QtDebug>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QStyleOption>

#include <ui/controls/transparent_label.h>

/**
 * @brief       Constructor.
 */
TransparentLabel::TransparentLabel(const QString &text, QWidget *parent) :
    QLabel(text, parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    // Set style sheet
    QFile styleFile(":/StyleSheet/transparent_label.qss");
    styleFile.open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::Text);
    this->setStyleSheet(styleFile.readAll());

    this->setAlignment(Qt::AlignmentFlag::AlignLeft
                       | Qt::AlignmentFlag::AlignTop);

    this->setWordWrap(true);
}

/**
 * @brief       Constructor.
 */
TransparentLabel::TransparentLabel(QWidget *parent) : QLabel(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    // Set style sheet
    QFile styleFile(":/StyleSheet/transparent_label.qss");
    styleFile.open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::Text);
    this->setStyleSheet(styleFile.readAll());

    this->setAlignment(Qt::AlignmentFlag::AlignLeft
                       | Qt::AlignmentFlag::AlignTop);

    this->setWordWrap(true);
}

/**
 * @brief       Destructor.
 */
TransparentLabel::~TransparentLabel() {}
