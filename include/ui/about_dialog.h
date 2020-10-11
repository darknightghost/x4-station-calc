#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <ui/customized_widgets/customized_dialog.h>

/**
 * @brief       About dialog.
 */
class AboutDialog : public CustomizedDialog {
    Q_OBJECT
  private:
    QWidget *    m_centralWidget;    ///< Central widget.
    QVBoxLayout *m_layout;           ///< Layout.
    QLabel *     m_lblTitle;         ///< Title.
    QLabel *     m_lblVersion;       ///< Version.
    QScrollArea *m_scrollChanglog;   ///< Scoll area of changelog.
    QVBoxLayout *m_layoutScrollArea; ///< Layout.
    QTextEdit *  m_txtChangelog;     ///< Changelog.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent.
     */
    AboutDialog(QWidget *parent = nullptr);

    /**
     * @brief   Destructor.
     */
    virtual ~AboutDialog();
};
