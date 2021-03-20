#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

/**
 * @brief       About dialog.
 */
class AboutDialog : public QDialog
{
    Q_OBJECT
  private:
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
