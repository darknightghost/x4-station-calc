#pragma once

#include <QtWidgets/QPushButton>

/**
 * @brief	Square button.
 */
class SquareButton : public QPushButton {
    Q_OBJECT
  private:
    QIcon m_icon;            ///< Icon.
    bool  m_iconInitialized; ///< Icon initialize flag.

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
     * @brief       Set icon.
     *
     * @param[in]   icon    Icon.
     */
    void setIcon(const QIcon &icon);

    /**
     * @brief		Resize icon.
     */
    void resizeIcon();

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

    /**
     * @brief		Resize icon.
     *
     * @param[in]	sz      Size.
     */
    void resizeIcon(const QSize &sz);
};
