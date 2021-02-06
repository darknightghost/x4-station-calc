#include <QtGui/QCloseEvent>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QAction>

#include <ui/main_window/action_control_dock_widget.h>

/**
 * @brief		Constructor.
 */
ActionControlDockWidget::ActionControlDockWidget(QAction *       statusAction,
                                                 QWidget *       parent,
                                                 Qt::WindowFlags flags) :
    QDockWidget(parent, flags),
    m_statusAction(statusAction), m_enableClose(false)
{
    statusAction->setCheckable(true);
    statusAction->setChecked(this->isVisible());
    this->connect(statusAction, &QAction::toggled, this,
                  &ActionControlDockWidget::onCheckStateChanged);
}

/**
 * @brief		Constructor.
 */
ActionControlDockWidget::ActionControlDockWidget(QAction *       statusAction,
                                                 const QString & title,
                                                 QWidget *       parent,
                                                 Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags),
    m_statusAction(statusAction), m_enableClose(false)
{
    statusAction->setCheckable(true);
    statusAction->setChecked(this->isVisible());
    this->connect(statusAction, &QAction::toggled, this,
                  &ActionControlDockWidget::onCheckStateChanged);
}

/**
 * @brief		Make the widget closeable.
 */
void ActionControlDockWidget::enableClose()
{
    m_enableClose = true;
}

/**
 * @brief		Make the widget not closeable.
 */
void ActionControlDockWidget::disableClose()
{
    m_enableClose = false;
}

/**
 * @brief		Destructor.
 */
ActionControlDockWidget::~ActionControlDockWidget() {}

/**
 * @brief		Show event.
 */
void ActionControlDockWidget::showEvent(QShowEvent *event)
{
    if (! m_statusAction->isChecked()) {
        m_statusAction->setChecked(true);
    }

    event->accept();
}

/**
 * @brief		Close event.
 */
void ActionControlDockWidget::closeEvent(QCloseEvent *event)
{
    if (m_enableClose) {
        event->accept();
    } else {
        event->ignore();
        if (m_statusAction->isChecked()) {
            m_statusAction->setChecked(false);
        }

        this->setVisible(false);
    }
}

/**
 * @brief		Slot to deal with the \c toggled signal of the action.
 */
void ActionControlDockWidget::onCheckStateChanged(bool status)
{
    if (this->isVisible() != status) {
        this->setVisible(status);
    }
}
