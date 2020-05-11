#pragma once

#include <QtWidgets/QDockWidget>

/**
 * @brief	QDockWidget whose wisibility can be controlled by an action.
 */
class ActionControlDockWidget : public QDockWidget {
    Q_OBJECT
  protected:
    QAction
        *m_statusAction; ///< Action to control the visibility of the widget.
    bool m_enableClose;  ///< If \c true, the widget can be closed. If \c false,
                         ///< the widget will hide instead of close.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	statusAction	Action to control the visibility of the
     *								widget.
     * @param[in]	parent			Parent object.
     * @param[in]	flags			Window flags.
     */
    ActionControlDockWidget(QAction *       statusAction,
                            QWidget *       parent = nullptr,
                            Qt::WindowFlags flags  = Qt::WindowFlags());

    /**
     * @brief		Constructor.
     *
     * @param[in]	statusAction	Action to control the visibility of the
     *								widget.
     * @param[in]	title			Title.
     * @param[in]	parent			Parent object.
     * @param[in]	flags			Window flags.
     */
    ActionControlDockWidget(QAction *       statusAction,
                            const QString & title,
                            QWidget *       parent = nullptr,
                            Qt::WindowFlags flags  = Qt::WindowFlags());

    /**
     * @brief		Make the widget closeable.
     */
    void enableClose();

    /**
     * @brief		Make the widget not closeable.
     */
    void disableClose();

    /**
     * @brief		Destructor.
     */
    virtual ~ActionControlDockWidget();

  protected:
    /**
     * @brief		Show event.
     *
     * @param[in]	event		Event.
     */
    virtual void showEvent(QShowEvent *event) override;

    /**
     * @brief		Hide event.
     *
     * @param[in]	event		Event.
     */
    virtual void hideEvent(QHideEvent *event) override;

    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  protected slots:
    /**
     * @brief		Slot to deal with the \c toggled signal of the action.
     *
     * @param[in]	status		Check status.
     */
    void onCheckStateChanged(bool status);
};
