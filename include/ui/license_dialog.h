#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <ui/customized_widgets/customized_dialog.h>

/**
 * @brief	License widget.
 */
class LicenseDialog : public CustomizedDialog {
    Q_OBJECT
  private:
    QWidget *m_centralWidget; ///< Central widget.

    QVBoxLayout *m_layout; //< Layout

    /// Text
    QTextEdit *m_txtLicense; //< Text

    /// Buttons
    QHBoxLayout *m_layoutButton; //< Layout of buttons.
    QPushButton *m_btnAgree;     //< Argree.
    QPushButton *m_btnDisagree;  //< Disagree.

  public:
    /**
     * @brief	Constructor.
     */
    LicenseDialog();

    /**
     * @brief	Destructor.
     */
    virtual ~LicenseDialog();

  private:
    /**
     * @brief	Load license data.
     */
    void loadLicense();
};
