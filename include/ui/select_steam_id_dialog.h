#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

/**
 * @brief	Language select dialog.
 */
class SelectSteamIDDialog : public QDialog {
    Q_OBJECT
  private:
    QVBoxLayout *m_layout;          //< Layout.
    QHBoxLayout *m_layoutSelection; //< Layout of selections.
    QLabel *     m_lblSteamID;      //< Lable Steam ID.
    QComboBox *  m_comboSteamID;    //< Select Steam ID.

    QHBoxLayout *m_layoutButton; //< Layout of Buttons.
    QPushButton *m_btnOK;        //< Button OK.

    QString m_selected; ///< Selected steam ID.

  public:
    /**
     * @brief	    Constructor.
     *
     * @param[in]   steam_id3s      List of steam ID3.
     */
    SelectSteamIDDialog(const QStringList &steam_id3s);

    /**
     * @brief       Get selected steam ID.
     *
     * @return      Selected steam ID.
     */
    const QString &selected() const;

    /**
     * @brief	Destructor.
     */
    virtual ~SelectSteamIDDialog();

  public:
    /**
     * @brief       Show the dialog and get the result.
     *
     * @param[in]   steam_id3s      List of steam ID3.
     *
     * @return      Selected steam ID3.
     */
    static QString selectSteamIDs(const QStringList &steam_id3s);

  public slots:
    /**
     * @brief       Wrapper of exec().
     *
     * @return      Return value of exec();
     */
    virtual int exec() override;

  protected:
    /**
     * @brief       Close event handler.
     *
     * @param[in]   e       Event.
     */
    virtual void closeEvent(QCloseEvent *e) override;
};
