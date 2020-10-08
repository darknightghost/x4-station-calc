#pragma once

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

  private:
    QWidget *     m_parent;               ///< Parent.
    QHBoxLayout * m_layout;               ///< Layout.
    SquareLabel * m_lblIcon;              ///< Label Icon.
    QLabel *      m_lblTitle;             ///< Label title.
    SquareButton *m_btnMinimize;          ///< Button Minimize.
    SquareButton *m_btnNormalizeMaximize; ///< Button normalize/maximize.
    SquareButton *m_btnClose;             ///< Button close.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    TitleBar(QWidget *parent);

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
