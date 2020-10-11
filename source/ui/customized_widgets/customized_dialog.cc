#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>

#include <common.h>
#include <ui/customized_widgets/customized_dialog.h>

#define BORDER_SIZE 3

/**
 * @brief       Constructor
 */
CustomizedDialog::CustomizedDialog(BorderType                borderType,
                                   TitleBar::TitleBarButtons titleBarButtons,
                                   QWidget *                 widget,
                                   QWidget *                 parent) :
    QDialog(parent),
    m_borderType(borderType), m_layout(nullptr), m_titleBar(nullptr),
    m_widget(nullptr), m_dragStatus(DragStatus::Normal)
{
    // Set window flags
    this->setWindowFlags(Qt::WindowType::CustomizeWindowHint
                         | Qt::WindowType::FramelessWindowHint
                         | Qt::WindowType::WindowSystemMenuHint
                         | Qt::WindowType::Dialog);

    // Layout.
    m_layout = new QVBoxLayout(this);
    Q_ASSERT(m_layout != nullptr);
    this->setLayout(m_layout);

    // Title bar.
    m_titleBar = new TitleBar(titleBarButtons, this);
    Q_ASSERT(m_titleBar != nullptr);
    m_layout->addWidget(m_titleBar);

    // Central widget.
    this->setWidget(widget);

    // Timer.
    m_mouseTimer = new QTimer(this);
    m_mouseTimer->setSingleShot(true);
    this->connect(m_mouseTimer, &QTimer::timeout, this,
                  &CustomizedDialog::updateCursor);

    this->setMouseTracking(true);
}

/**
 * @brief       Destructor.
 */
CustomizedDialog::~CustomizedDialog() {}

/**
 * @brief       Set widget.
 */
QWidget *CustomizedDialog::setWidget(QWidget *newWidget)
{
    if (m_widget != nullptr) {
        m_layout->removeWidget(m_widget);
        m_widget->setParent(nullptr);
    }

    QWidget *ret = m_widget;
    m_widget     = newWidget;

    if (newWidget != nullptr) {
        newWidget->setParent(this);
        m_layout->addWidget(newWidget);
    }

    return ret;
}

/**
 * @brief       Get widget.
 */
QWidget *CustomizedDialog::widget()
{
    return m_widget;
}

/**
 * @brief       Get mouse region.
 */
CustomizedDialog::MouseRegion CustomizedDialog::mouseRegoin(const QPoint &pos)
{
    // Border.
    Range<int> borderTop(0, max(m_titleBar->y(), BORDER_SIZE) - 1);
    Range<int> borderLeft(0, max(m_titleBar->x(), BORDER_SIZE) - 1);
    int        borderRightSize = max(
        this->width() - m_titleBar->width() - m_titleBar->x(), BORDER_SIZE);
    Range<int> borderRight(this->width() - borderRightSize, this->width() - 1);
    int        borderBottomSize
        = max((m_widget == nullptr
                   ? this->height() - m_titleBar->height() - m_titleBar->y()
                   : this->height() - m_widget->height() - m_widget->y()),
              BORDER_SIZE);
    Range<int> borderBottom(this->height() - borderBottomSize,
                            this->height() - 1);

    // Compare mouse position.
    uint32_t mousePos = MouseRegion::RegionNone;
    if (borderTop.compare(pos.y()) == 0) {
        mousePos |= (uint32_t)MouseRegion::RegionTop;
    }

    if (borderBottom.compare(pos.y()) == 0) {
        mousePos |= (uint32_t)MouseRegion::RegionBottom;
    }

    if (borderLeft.compare(pos.x()) == 0) {
        mousePos |= (uint32_t)MouseRegion::RegionLeft;
    }

    if (borderRight.compare(pos.x()) == 0) {
        mousePos |= (uint32_t)MouseRegion::RegionRight;
    }

    return (MouseRegion)mousePos;
}

/**
 * @brief       Update cursor.
 */
void CustomizedDialog::updateCursor()
{
    if (m_dragStatus != DragStatus::Normal) {
        m_mouseTimer->start(100);
        return;
    }

    QPoint globalPos = QCursor::pos();
    if (Range<int>(this->x(), this->x() + this->width() - 1)
                .compare(globalPos.x())
            != 0
        || Range<int>(this->y(), this->y() + this->height() - 1)
                   .compare(globalPos.y())
               != 0) {
        this->setCursor(Qt::CursorShape::ArrowCursor);
        return;
    }
    QPoint pos(globalPos.x() - this->x(), globalPos.y() - this->y());

    MouseRegion region = this->mouseRegoin(pos);

    switch (region) {
        case MouseRegion::RegionLeft:
            this->setCursor(Qt::CursorShape::SizeHorCursor);
            break;

        case MouseRegion::RegionTop:
            this->setCursor(Qt::CursorShape::SizeVerCursor);
            break;

        case MouseRegion::RegionRight:
            this->setCursor(Qt::CursorShape::SizeHorCursor);
            break;

        case MouseRegion::RegionBottom:
            this->setCursor(Qt::CursorShape::SizeVerCursor);
            break;

        case MouseRegion::RegionTopLeft:
            this->setCursor(Qt::CursorShape::SizeFDiagCursor);
            break;

        case MouseRegion::RegionTopRight:
            this->setCursor(Qt::CursorShape::SizeBDiagCursor);
            break;

        case MouseRegion::RegionBottomLeft:
            this->setCursor(Qt::CursorShape::SizeBDiagCursor);
            break;

        case MouseRegion::RegionBottomRight:
            this->setCursor(Qt::CursorShape::SizeFDiagCursor);
            break;

        default:
            this->setCursor(Qt::CursorShape::ArrowCursor);
            break;
    }

    if (region == MouseRegion::RegionCenter) {
        m_mouseTimer->stop();

    } else {
        m_mouseTimer->start(100);
    }

    return;
}

/**
 * @brief       Mouse leave event.
 */
void CustomizedDialog::leaveEvent(QEvent *event)
{
    if (m_borderType == BorderType::Fixed
        || m_dragStatus != DragStatus::Normal) {
        this->QDialog::leaveEvent(event);
        return;
    }

    switch (m_dragStatus) {
        case DragStatus::Normal:
            this->setCursor(Qt::CursorShape::ArrowCursor);
            this->QDialog::leaveEvent(event);
            break;

        case DragStatus::Resizing:
            this->QDialog::leaveEvent(event);
            break;

        case DragStatus::Moving:
            this->QDialog::leaveEvent(event);
            break;
    }

    m_mouseTimer->stop();

    return;
}

/**
 * @brief       Mouse move event.
 */
void CustomizedDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_borderType == BorderType::Fixed
        || this->windowState() != Qt::WindowState::WindowNoState) {
        this->QDialog::mouseMoveEvent(event);
        return;
    }

    switch (m_dragStatus) {
        case DragStatus::Normal:
            this->mouseMoveEventNormal(event);
            break;

        case DragStatus::Resizing:
            this->mouseMoveEventResizing(event);
            break;

        case DragStatus::Moving:
            this->mouseMoveEventMoving(event);
            break;
    }

    return;
}

/**
 * @brief       Mouse move event.
 */
void CustomizedDialog::mouseMoveEventNormal(QMouseEvent *event)
{
    MouseRegion region = this->mouseRegoin(event->pos());

    switch (region) {
        case MouseRegion::RegionLeft:
            this->setCursor(Qt::CursorShape::SizeHorCursor);
            break;

        case MouseRegion::RegionTop:
            this->setCursor(Qt::CursorShape::SizeVerCursor);
            break;

        case MouseRegion::RegionRight:
            this->setCursor(Qt::CursorShape::SizeHorCursor);
            break;

        case MouseRegion::RegionBottom:
            this->setCursor(Qt::CursorShape::SizeVerCursor);
            break;

        case MouseRegion::RegionTopLeft:
            this->setCursor(Qt::CursorShape::SizeFDiagCursor);
            break;

        case MouseRegion::RegionTopRight:
            this->setCursor(Qt::CursorShape::SizeBDiagCursor);
            break;

        case MouseRegion::RegionBottomLeft:
            this->setCursor(Qt::CursorShape::SizeBDiagCursor);
            break;

        case MouseRegion::RegionBottomRight:
            this->setCursor(Qt::CursorShape::SizeFDiagCursor);
            break;

        default:
            this->setCursor(Qt::CursorShape::ArrowCursor);
            break;
    }
    if (region == MouseRegion::RegionCenter) {
        m_mouseTimer->stop();

    } else {
        m_mouseTimer->start(100);
    }

    this->QDialog::mouseMoveEvent(event);
    return;
}

/**
 * @brief       Mouse move event.
 */
void CustomizedDialog::mouseMoveEventResizing(QMouseEvent *event)
{
    QRect geo = this->geometry();

    switch (m_resizingRegion) {
        case MouseRegion::RegionLeft: {
            int width = max(geo.x() + geo.width() - event->globalX(),
                            this->minimumWidth());

            geo = QRect(geo.x() + geo.width() - width, geo.y(), width,
                        geo.height());
        } break;

        case MouseRegion::RegionTop: {
            int height = max(geo.y() + geo.height() - event->globalY(),
                             this->minimumHeight());

            geo = QRect(geo.x(), geo.y() + geo.height() - height, geo.width(),
                        height);
        } break;

        case MouseRegion::RegionRight:
            geo = QRect(geo.x(), geo.y(), event->globalX() - geo.x(),
                        geo.height());
            break;

        case MouseRegion::RegionBottom:
            geo = QRect(geo.x(), geo.y(), geo.width(),
                        event->globalY() - geo.y());
            break;

        case MouseRegion::RegionTopLeft: {
            int width  = max(geo.x() + geo.width() - event->globalX(),
                            this->minimumWidth());
            int height = max(geo.y() + geo.height() - event->globalY(),
                             this->minimumHeight());
            geo        = QRect(geo.x() + geo.width() - width,
                        geo.y() + geo.height() - height, width, height);
        } break;

        case MouseRegion::RegionTopRight: {
            int height = max(geo.y() + geo.height() - event->globalY(),
                             this->minimumHeight());
            geo        = QRect(geo.x(), geo.y() + geo.height() - height,
                        event->globalX() - geo.x(), height);
        } break;

        case MouseRegion::RegionBottomLeft: {
            int width = max(geo.x() + geo.width() - event->globalX(),
                            this->minimumWidth());
            geo       = QRect(geo.x() + geo.width() - width, geo.y(), width,
                        event->globalY() - geo.y());
        } break;

        case MouseRegion::RegionBottomRight:
            geo = QRect(geo.x(), geo.y(), event->globalX() - geo.x(),
                        event->globalY() - geo.y());
            break;

        default:
            break;
    }

    this->setGeometry(geo);
    this->QDialog::mouseMoveEvent(event);
    return;
}

/**
 * @brief       Mouse move event.
 */
void CustomizedDialog::mouseMoveEventMoving(QMouseEvent *event)
{
    this->QDialog::mouseMoveEvent(event);
    return;
}

/**
 * @brief       Mouse pressed event.
 */
void CustomizedDialog::mousePressEvent(QMouseEvent *event)
{
    if (m_dragStatus == DragStatus::Normal
        && this->windowState() == Qt::WindowState::WindowNoState) {
        MouseRegion region = this->mouseRegoin(event->pos());
        if (region != MouseRegion::RegionCenter) {
            m_dragStatus     = DragStatus::Resizing;
            m_resizingRegion = region;
        }
    }

    this->QDialog::mousePressEvent(event);
    return;
}

/**
 * @brief       Mouse released event.
 */
void CustomizedDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragStatus = DragStatus::Normal;

    this->QDialog::mouseReleaseEvent(event);
    return;
}
