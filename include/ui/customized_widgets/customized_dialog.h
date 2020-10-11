#pragma once

#include <QtWidgets/QDialog>

#include <ui/customized_widgets/customized_window.h>

/**
 * @brief       Customized dialog.
 */
class CustomizedDialog : public QDialog {
    Q_OBJECT;

  public:
    typedef CustomizedWindow::BorderType BorderType;

  protected:
    /**
     * @brief   mouse position.
     */
    typedef CustomizedWindow::MouseRegion MouseRegion;

    /**
     * @brief   drag status.
     */
    typedef CustomizedWindow::DragStatus DragStatus;

  private:
    BorderType   m_borderType;     ///< Type of the border.
    QVBoxLayout *m_layout;         ///< Layout.
    TitleBar *   m_titleBar;       ///< Title bar.
    QWidget *    m_widget;         ///< Widget.
    DragStatus   m_dragStatus;     ///< Dragging status.
    MouseRegion  m_resizingRegion; ///< Resizing region.
    QTimer *     m_mouseTimer;     ///< Timer to update mouse status.

  public:
    /**
     * @brief       Constructor
     *
     * @param[in]   borderType          Border type.
     * @param[in]   titleBarButtons     Buttons on the title bar.
     * @param[in]   widget              Widget.
     * @param[in]   parent              Parent widget.
     */
    CustomizedDialog(BorderType borderType = BorderType::Resizable,
                     TitleBar::TitleBarButtons titleBarButtons
                     = TitleBar::TitleBarButton::AllButtons,
                     QWidget *widget = nullptr,
                     QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~CustomizedDialog();

  public:
    /**
     * @brief       Set widget.
     *
     * @param[in]   newWidget       New widget.
     *
     * @return      Preview widget.
     */
    QWidget *setWidget(QWidget *newWidget);

    /**
     * @brief       Get widget.
     *
     * @return      Widget.
     */
    QWidget *widget();

  protected:
    /**
     * @brief       Get mouse region.
     *
     * @param[in]   event       Event.
     *
     * @return      Mouse regoin.
     */
    MouseRegion mouseRegoin(const QPoint &pos);

  protected slots:
    /**
     * @brief       Update cursor.
     */
    void updateCursor();

  protected:
    /**
     * @brief       Mouse leave event.
     *
     * @param[in]   event       Event.
     */
    virtual void leaveEvent(QEvent *event) override;

    /**
     * @brief       Mouse move event.
     *
     * @param[in]   event       Event.
     */
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief       Mouse move event.
     *
     * @param[in]   event       Event.
     */
    void mouseMoveEventResizing(QMouseEvent *event);

    /**
     * @brief       Mouse move event.
     *
     * @param[in]   event       Event.
     */
    void mouseMoveEventMoving(QMouseEvent *event);

    /**
     * @brief       Mouse move event.
     *
     * @param[in]   event       Event.
     */
    void mouseMoveEventNormal(QMouseEvent *event);

    /**
     * @brief       Mouse pressed event.
     *
     * @param[in]   event       Event.
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief       Mouse released event.
     *
     * @param[in]   event       Event.
     */
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};
