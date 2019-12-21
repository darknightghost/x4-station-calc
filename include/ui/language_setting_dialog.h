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
class LanguageSettingDialog : public QDialog {
    Q_OBJECT
  private:
    QVBoxLayout *m_layout;          //< Layout.
    QHBoxLayout *m_selectionLayout; //< Layout of selections.
    QLabel *     m_lblLang;         //< Lable language.
    QComboBox *  m_comboLang;       //< Select language.

    QHBoxLayout *m_btnLayout; //< Layout of Buttons..
    QPushButton *m_btnOK;     //< Button OK.

  private slots:
    /**
     * @brief	Language changed slot.
     */
    void onLanguageChanged();

    /**
     * @brief	Selected language changed slot.
     */
    void onSelectedChanged(int index);

  public:
    /**
     * @brief	Constructor.
     */
    LanguageSettingDialog();

    /**
     * @brief	Destructor.
     */
    virtual ~LanguageSettingDialog();
};
