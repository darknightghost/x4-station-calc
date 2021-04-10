#pragma once

#include <QtWidgets/QScrollArea>

/**
 * @brief       Scroll area which set the minimum size to fill the child area.
 */
class ScrollAreaAutoFill : public QScrollArea {
    Q_OBJECT;

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    ScrollAreaAutoFill(QWidget *parent = nullptr);

    /**
     * @brief       Set the scroll area's widget.
     *
     * @param[in]   widget      Widget to set.
     */
    void setWidget(QWidget *widget);

    /**
     * @brief       Destructor.
     */
    virtual ~ScrollAreaAutoFill();

  protected:
    /**
     * @brief       Resize event.
     *
     * @param[in]   event       Event.
     */
    virtual void resizeEvent(QResizeEvent *event) override;
};
