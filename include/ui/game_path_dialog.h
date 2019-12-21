#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

/**
 * @brief	Language select dialog.
 */
class GamePathDialog : public QDialog {
    Q_OBJECT
  private:
    QVBoxLayout *m_layout; //< Layout.

    QHBoxLayout *m_layoutSelectPath; //< Layout of selections.
    QLabel *     m_lblPath;          //< Lable Path.
    QLineEdit *  m_txtPath;          //< Path.
    QPushButton *m_btnBrowse;        //< Button browse.

    QHBoxLayout *m_layoutBtn; //< Layout of Buttons..
    QPushButton *m_btnOK;     //< Button OK.
    QPushButton *m_btnCancel; //< Button Cancel.

  public:
    /**
     * @brief	Constructor.
     */
    GamePathDialog();

    /**
     * @brief	Destructor.
     */
    virtual ~GamePathDialog();
};
