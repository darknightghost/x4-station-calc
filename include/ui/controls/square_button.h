#pragma once

#include <QtWidgets/QPushButton>

/**
 * @brief	Square button.
 */
class SquareButton : public QPushButton {
    Q_OBJECT
  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	icon		Icon.
     * @param[in]	parent		Parent object.
     */
    SquareButton(const QIcon &icon, QWidget *parent = nullptr);

    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent object.
     */
    SquareButton(QWidget *parent = nullptr);

    /**
     * @brief		Destructor.
     */
    virtual ~SquareButton();

  protected:
    /**
     * @brief		Resize event.
     *
     * @param[in]	event	Event.
     */
    virtual void resizeEvent(QResizeEvent *event) override;
};
