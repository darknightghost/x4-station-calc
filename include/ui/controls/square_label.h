#pragma once

#include <QtGui/QIcon>
#include <QtWidgets/QLabel>

/**
 * @brief	Square label.
 */
class SquareLabel : public QLabel {
    Q_OBJECT
  private:
    QIcon m_icon; ///< Icon.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	icon		Icon.
     * @param[in]	parent		Parent object.
     */
    SquareLabel(const QIcon &icon, QWidget *parent = nullptr);

    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent object.
     */
    SquareLabel(QWidget *parent = nullptr);

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
    virtual ~SquareLabel();

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
