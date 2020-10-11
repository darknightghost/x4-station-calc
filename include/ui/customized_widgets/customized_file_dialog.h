#pragma once

#include <QtWidgets/QFileDialog>

#include <ui/customized_widgets/customized_dialog.h>

/**
 * @brief       Customized file dialog.
 */
class CustomizedFileDialog : public CustomizedDialog {
    Q_OBJECT;

  protected:
    QFileDialog *m_fileDialog; ///< File dialog.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     * @param[in]   caption     Caption of the dialog.
     * @param[in]   directory   Default directory.
     * @param[in]   filter      Filter of filename.
     */
    CustomizedFileDialog(QWidget *      parent    = nullptr,
                         const QString &caption   = QString(),
                         const QString &directory = QString(),
                         const QString &filter    = QString());

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     * @param[in]   flags       Window flags.
     */
    CustomizedFileDialog(QWidget *parent, Qt::WindowFlags flags);

    /**
     * @brief       Destructor.
     */
    virtual ~CustomizedFileDialog();

  public:
    /**
     * @brief       Get accept mode.
     *
     * @return      Accept mode.
     */
    QFileDialog::AcceptMode acceptMode() const;

    /**
     * @brief       Set accept mode.
     *
     * @param[in]   mode        Accept mode.
     */
    void setAcceptMode(QFileDialog::AcceptMode mode);

    /**
     * @brief       Get file mode.
     *
     * @return      File mode.
     */
    QFileDialog::FileMode fileMode() const;

    /**
     * @brief       Set file mode.
     *
     * @param[in]   mode        File mode.
     */
    void setFileMode(QFileDialog::FileMode mode);

    /**
     * @brief       Get filter.
     *
     * @return      Filter.
     */
    QDir::Filters filter() const;

    /**
     * @brief       Set filter.
     *
     * @param[in]   filters     Filters.
     */
    void setFilter(QDir::Filters filters);

    /**
     * @brief       Get the options that affect the look and feel of the dialog.
     *
     * @return      Options.
     */
    QFileDialog::Options options() const;

    /**
     * @brief       Set the options that affect the look and feel of the dialog.
     *
     * @param[in]   options     Options.
     */
    void setOptions(QFileDialog::Options options);

  public:
    /**
     * @brief       Get selected files.
     *
     * @return      List of selected files.
     */
    QStringList selectedFiles() const;

  public:
    /**
     * @brief       Get path to save.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   caption         Caption of the dialog.
     * @param[in]   dir             Default directory.
     * @param[in]   filter          Filter of filename.
     * @param[in]   selectedFilter  The filter selected.
     * @param[in]   options         Options about how to run the dialog.
     *
     * @return      Path to save.
     */
    static QString getSaveFileName(QWidget *      parent         = nullptr,
                                   const QString &caption        = QString(),
                                   const QString &dir            = QString(),
                                   const QString &filter         = QString(),
                                   QString *      selectedFilter = nullptr,
                                   QFileDialog::Options options
                                   = QFileDialog::Options());

    /**
     * @brief       Get path to open.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   caption         Caption of the dialog.
     * @param[in]   dir             Default directory.
     * @param[in]   filter          Filter of filename.
     * @param[in]   selectedFilter  The filter selected.
     * @param[in]   options         Options about how to run the dialog.
     *
     * @return      Path to open.
     */
    static QString getOpenFileName(QWidget *      parent         = nullptr,
                                   const QString &caption        = QString(),
                                   const QString &dir            = QString(),
                                   const QString &filter         = QString(),
                                   QString *      selectedFilter = nullptr,
                                   QFileDialog::Options options
                                   = QFileDialog::Options());

    /**
     * @brief       Get paths to open.
     *
     * @param[in]   parent          Parent widget.
     * @param[in]   caption         Caption of the dialog.
     * @param[in]   dir             Default directory.
     * @param[in]   filter          Filter of filename.
     * @param[in]   selectedFilter  The filter selected.
     * @param[in]   options         Options about how to run the dialog.
     *
     * @return      Paths to open.
     */
    static QStringList getOpenFileNames(QWidget *      parent  = nullptr,
                                        const QString &caption = QString(),
                                        const QString &dir     = QString(),
                                        const QString &filter  = QString(),
                                        QString *      selectedFilter = nullptr,
                                        QFileDialog::Options options
                                        = QFileDialog::Options());
};
