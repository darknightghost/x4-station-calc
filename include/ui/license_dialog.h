#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

/**
 * @brief	License widget.
 */
class LicenseDialog : public QDialog {
    Q_OBJECT
  private:
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
