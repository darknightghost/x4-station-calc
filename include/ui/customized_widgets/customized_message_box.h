#pragma once

#include <QtWidgets/QMessageBox>

#include <ui/customized_widgets/customized_dialog.h>

/**
 * @brief       Customized file dialog.
 */
class CustomizedMessageBox : public CustomizedDialog {
    Q_OBJECT;

  protected:
    QMessageBox *m_msgBox; ///< Message box.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   icon        Icon.
     * @param[in]   title       Window title.
     * @param[in]   text        Message text.
     * @param[in]   buttons     Buttons.
     * @param[in]   parent      Parent widget.
     * @param[in]   f           Window flags.
     */
    CustomizedMessageBox(QMessageBox::Icon            icon,
                         const QString &              title,
                         const QString &              text,
                         QMessageBox::StandardButtons buttons
                         = QMessageBox::StandardButton::NoButton,
                         QWidget *       parent = nullptr,
                         Qt::WindowFlags f
                         = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    CustomizedMessageBox(QWidget *parent = nullptr);

    /**
     * @brief       Set default button.
     *
     * @param[in]   button      Default button to set.
     */
    void setDefaultButton(QMessageBox::StandardButton button);

    /**
     * @brief       Get clicked standard button.
     *
     * @return      Clicked standard button.
     */
    QMessageBox::StandardButton clickedStandardButton() const;

    /**
     * @brief       Destructor.
     */
    virtual ~CustomizedMessageBox();

  public:
    /**
     * @brief       Show message box critical.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   title           Window title.
     * @param[in]   text            Message text.
     * @param[in]   buttons         Buttons.
     * @param[in]   defaultButton   Default focused button.
     *
     * @return      Button clicked.
     */
    static QMessageBox::StandardButton
        critical(QWidget *                    parent,
                 const QString &              title,
                 const QString &              text,
                 QMessageBox::StandardButtons buttons
                 = QMessageBox::StandardButton::Ok,
                 QMessageBox::StandardButton defaultButton
                 = QMessageBox::StandardButton::NoButton);

    /**
     * @brief       Show message box information.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   title           Window title.
     * @param[in]   text            Message text.
     * @param[in]   buttons         Buttons.
     * @param[in]   defaultButton   Default focused button.
     *
     * @return      Button clicked.
     */
    static QMessageBox::StandardButton
        information(QWidget *                    parent,
                    const QString &              title,
                    const QString &              text,
                    QMessageBox::StandardButtons buttons
                    = QMessageBox::StandardButton::Ok,
                    QMessageBox::StandardButton defaultButton
                    = QMessageBox::StandardButton::NoButton);
    /**
     * @brief       Show message box question.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   title           Window title.
     * @param[in]   text            Message text.
     * @param[in]   buttons         Buttons.
     * @param[in]   defaultButton   Default focused button.
     *
     * @return      Button clicked.
     */
    static QMessageBox::StandardButton question(
        QWidget *                    parent,
        const QString &              title,
        const QString &              text,
        QMessageBox::StandardButtons buttons
        = QMessageBox::StandardButtons(QMessageBox::StandardButton::Yes
                                       | QMessageBox::StandardButton::No),
        QMessageBox::StandardButton defaultButton
        = QMessageBox::StandardButton::NoButton);

    /**
     * @brief       Show message box warning.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   title           Window title.
     * @param[in]   text            Message text.
     * @param[in]   buttons         Buttons.
     * @param[in]   defaultButton   Default focused button.
     *
     * @return      Button clicked.
     */
    static QMessageBox::StandardButton
        warning(QWidget *                    parent,
                const QString &              title,
                const QString &              text,
                QMessageBox::StandardButtons buttons
                = QMessageBox::StandardButton::Ok,
                QMessageBox::StandardButton defaultButton
                = QMessageBox::StandardButton::NoButton);
};
