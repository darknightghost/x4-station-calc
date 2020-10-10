#pragma once

#include <QtCore/QFlags>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include <ui/controls/square_button.h>
#include <ui/controls/square_label.h>

/**
 * @brief       Title bar of main window.
 */
class TitleBar : public QWidget {
    Q_OBJECT;

  public:
    /**
     * @brief       Titlebar buttons.
     */
    enum TitleBarButton : uint32_t {
        NoButton       = 0x00000000, ///< No button.
        MinimizeButton = 0x00000001, ///< Minimize button.
        MaximizeButton = 0x00000002, ///< Maximize/normalize button.
        CloseButton    = 0x00000004, ///< Close button.
        MinCloseButtons
        = MinimizeButton | CloseButton, ///< Minimize and close buttons.
        AllButtons
        = MinimizeButton | MaximizeButton | CloseButton ///< All buttons.
    };

    Q_DECLARE_FLAGS(TitleBarButtons, TitleBarButton);

  private:
    QWidget *     m_parent;               ///< Parent.
    QHBoxLayout * m_layout;               ///< Layout.
    SquareLabel * m_lblIcon;              ///< Label Icon.
    QLabel *      m_lblTitle;             ///< Label title.
    QHBoxLayout * m_layoutButton;         ///< Button layout.
    SquareButton *m_btnMinimize;          ///< Button Minimize.
    SquareButton *m_btnNormalizeMaximize; ///< Button normalize/maximize.
    SquareButton *m_btnClose;             ///< Button close.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   buttons     Buttons.
     * @param[in]   parent      Parent widget.
     */
    TitleBar(TitleBarButtons buttons, QWidget *parent);

    /**
     * @brief       Destructor.
     */
    virtual ~TitleBar();

  private slots:
    /**
     * @brief       Update icon.
     *
     * @param[in]   icon        Icon
     */
    void updateIcon(const QIcon &icon);

    /**
     * @brief       Update title.
     *
     * @param[in]   title       Window title
     */
    void updateTitle(const QString &title);

    /**
     * @brief     Update window state.
     */
    void updateStatus();

    /**
     * @brief       Skin changed slot.
     *
     * @param[in]   currentSkin     Current skin.
     */
    void onSkinChanged(const QString &currentSkin);

    /**
     * @brief       On button minimize clicked.
     */
    void onBtnMinimizeClicked();

    /**
     * @brief       On button normalize/maximize clicked.
     */
    void onBtnNormalizeMaximizeClicked();

    /**
     * @brief       On button close clicked.
     */
    void onBtnCloseClicked();

  protected:
    /**
     * @brief       Event filter.
     *
     * @param[in]   obj     Object.
     * @param[in]   event   Event.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;
};
