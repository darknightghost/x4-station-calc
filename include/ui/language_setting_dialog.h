#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
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
