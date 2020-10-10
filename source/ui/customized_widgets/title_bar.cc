#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QMouseEvent>

#include <common.h>
#include <skin_manager.h>
#include <ui/customized_widgets/title_bar.h>

/**
 * @brief       Constructor.
 */
TitleBar::TitleBar(TitleBarButtons buttons, QWidget *parent) :
    QWidget(parent), m_parent(parent), m_dragFlag(false)
{
    // Set window flags.
    this->setWindowFlags(Qt::WindowType::Widget
                         | Qt::WindowType::FramelessWindowHint);
    this->setProperty("class", "TitleBar");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);

    // Widgets.
    m_layout = new QHBoxLayout(this);
    this->setLayout(m_layout);

    m_lblIcon = new SquareLabel(this);
    m_layout->addWidget(m_lblIcon);
    m_lblIcon->setProperty("class", "TitleBarIcon");

    m_lblTitle = new QLabel(this);
    m_layout->addWidget(m_lblTitle);
    m_lblTitle->setProperty("class", "TitleBarTitle");

    m_layoutButton = new QHBoxLayout(this);
    m_layout->addLayout(m_layoutButton);

    m_btnMinimize = new SquareButton(this);
    m_layoutButton->addWidget(m_btnMinimize);
    m_btnMinimize->setProperty("class", "TitleBarBtnMinimize");
    this->connect(m_btnMinimize, &QPushButton::clicked, this,
                  &TitleBar::onBtnMinimizeClicked);

    m_btnNormalizeMaximize = new SquareButton(this);
    m_layoutButton->addWidget(m_btnNormalizeMaximize);
    m_btnNormalizeMaximize->setProperty("class",
                                        "TitleBarBtnNormalizeMaximize");
    this->connect(m_btnNormalizeMaximize, &QPushButton::clicked, this,
                  &TitleBar::onBtnNormalizeMaximizeClicked);

    m_btnClose = new SquareButton(this);
    m_layoutButton->addWidget(m_btnClose);
    m_btnClose->setProperty("class", "TitleBarBtnClose");
    this->connect(m_btnClose, &QPushButton::clicked, this,
                  &TitleBar::onBtnCloseClicked);

    this->connect(m_parent, &QWidget::windowIconChanged, this,
                  &TitleBar::updateIcon);
    this->connect(m_parent, &QWidget::windowTitleChanged, this,
                  &TitleBar::updateTitle);
    this->connect(SkinManager::instance().get(), &SkinManager::skinChanged,
                  this, &TitleBar::onSkinChanged);
    m_parent->installEventFilter(this);
    m_lblIcon->installEventFilter(this);
    m_lblTitle->installEventFilter(this);

    this->updateIcon(m_parent->windowIcon());
    this->updateTitle(m_parent->windowTitle());
    this->onSkinChanged(SkinManager::instance()->currentSkin());

    if (! (buttons | TitleBarButton::MinimizeButton)) {
        m_btnMinimize->setVisible(false);

    } else if (! (buttons | TitleBarButton::MaximizeButton)) {
        m_btnNormalizeMaximize->setVisible(false);

    } else if (! (buttons | TitleBarButton::CloseButton)) {
        m_btnClose->setVisible(false);
    }
}

/**
 * @brief       Destructor.
 */
TitleBar::~TitleBar() {}

/**
 * @brief       Update icon.
 */
void TitleBar::updateIcon(const QIcon &icon)
{
    m_lblIcon->setIcon(icon);
}

/**
 * @brief       Update title.
 */
void TitleBar::updateTitle(const QString &title)
{
    m_lblTitle->setText(title);
}

/**
 * @brief     Update status.
 */
void TitleBar::updateStatus()
{
    if (m_parent->windowState()
        & (Qt::WindowState::WindowMaximized
           | Qt::WindowState::WindowFullScreen)) {
        m_btnNormalizeMaximize->setIcon(
            QIcon(QString(":/Skins/%1/Icons/WindowNormalize.png")
                      .arg(SkinManager::instance()->currentSkin())));
    } else {
        m_btnNormalizeMaximize->setIcon(
            QIcon(QString(":/Skins/%1/Icons/WindowMaximize.png")
                      .arg(SkinManager::instance()->currentSkin())));
    }
}

/**
 * @brief       Skin changed slot.
 */
void TitleBar::onSkinChanged(const QString &currentSkin)
{
    m_btnMinimize->setIcon(
        QIcon(QString(":/Skins/%1/Icons/WindowMinimize.png").arg(currentSkin)));
    this->updateStatus();
    m_btnClose->setIcon(
        QIcon(QString(":/Skins/%1/Icons/WindowClose.png").arg(currentSkin)));
}

/**
 * @brief       Event filter.
 */
bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == static_cast<QObject *>(m_parent)) {
        switch (event->type()) {
            case QEvent::Type::WindowStateChange:
                this->updateStatus();
                return this->QObject::eventFilter(obj, event);

            default:
                return this->QObject::eventFilter(obj, event);
        }

    } else if (obj == static_cast<QObject *>(m_lblIcon)
               || obj == static_cast<QObject *>(m_lblTitle)) {
        return this->QObject::eventFilter(obj, event);
    } else {
        return this->QObject::eventFilter(obj, event);
    }
}

/**
 * @brief       On button minimize clicked.
 */
void TitleBar::onBtnMinimizeClicked()
{
    m_parent->setWindowState(Qt::WindowState::WindowMinimized);
}

/**
 * @brief       On button normalize/maximize clicked.
 */
void TitleBar::onBtnNormalizeMaximizeClicked()
{
    if (m_parent->windowState()
        & (Qt::WindowState::WindowMaximized | Qt::WindowState::WindowFullScreen
           | Qt::WindowState::WindowMinimized)) {
        m_parent->setWindowState(Qt::WindowState::WindowNoState);
    } else {
        m_parent->setWindowState(Qt::WindowState::WindowMaximized);
    }
}

/**
 * @brief       On button close clicked.
 */
void TitleBar::onBtnCloseClicked()
{
    m_parent->close();
}

/**
 * @brief       Mouse move event.
 */
void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragFlag) {
        QPoint currentPos = event->globalPos();
        int    dx         = currentPos.x() - m_prevMousePos.x();
        int    dy         = currentPos.y() - m_prevMousePos.y();
        m_prevMousePos    = currentPos;
        m_parent->move(m_parent->x() + dx, m_parent->y() + dy);
    }
    this->QWidget::mouseMoveEvent(event);
}

/**
 * @brief       Mouse pressed event.
 */
void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_dragFlag = true;
    this->setCursor(Qt::CursorShape::SizeAllCursor);
    m_prevMousePos = event->globalPos();
    this->QWidget::mousePressEvent(event);
}

/**
 * @brief       Mouse released event.
 */
void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragFlag = false;
    this->setCursor(Qt::CursorShape::ArrowCursor);
    this->QWidget::mouseReleaseEvent(event);
}

/**
 * @brief       Double click event.
 */
void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        this->onBtnNormalizeMaximizeClicked();
    }
    this->QWidget::mouseDoubleClickEvent(event);
}
