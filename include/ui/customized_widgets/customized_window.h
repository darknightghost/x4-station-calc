#pragma once

#include <QtCore/QFlags>
#include <QtCore/QTimer>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <ui/customized_widgets/title_bar.h>

/**
 * @brief       Cusatomized widget.
 */
class CustomizedWindow : public QWidget {
    Q_OBJECT;

  public:
    /**
     * @brief   Border type.
     */
    enum BorderType {
        Resizable, ///< The window can be resized.
        Fixed      ///< The size of window si fixed.
    };

  protected:
    /**
     * @brief   Mouse position.
     */
    enum MouseRegion : uint32_t {
        RegionNone        = 0x00000000,
        RegionCenter      = 0x00000000,
        RegionLeft        = 0x00000001,
        RegionTop         = 0x00000002,
        RegionRight       = 0x00000004,
        RegionBottom      = 0x00000008,
        RegionTopLeft     = RegionTop | RegionLeft,
        RegionTopRight    = RegionTop | RegionRight,
        RegionBottomLeft  = RegionBottom | RegionLeft,
        RegionBottomRight = RegionBottom | RegionRight
    };

    /**
     * @brief   Drag status.
     */
    enum DragStatus { Normal, Resizing, Moving };

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
    CustomizedWindow(BorderType borderType = BorderType::Resizable,
                     TitleBar::TitleBarButtons titleBarButtons
                     = TitleBar::TitleBarButton::AllButtons,
                     QWidget *widget = nullptr,
                     QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~CustomizedWindow();

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
