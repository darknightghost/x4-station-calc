#include <ui/customized_widgets/customized_file_dialog.h>

/**
 * @brief       Constructor.
 */
CustomizedFileDialog::CustomizedFileDialog(QWidget *      parent,
                                           const QString &caption,
                                           const QString &directory,
                                           const QString &filter) :
    CustomizedDialog(CustomizedDialog::BorderType::Resizable,
                     TitleBar::TitleBarButton::AllButtons,
                     parent),
    m_fileDialog(new QFileDialog(this, "", directory, filter))
{
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    m_fileDialog->setWindowFlags(Qt::WindowType::Widget);
    m_fileDialog->setOption(QFileDialog::Option::DontUseNativeDialog);
    this->setWidget(m_fileDialog);
    this->setWindowTitle(caption);
    this->connect(m_fileDialog, &QFileDialog::accepted, this,
                  &CustomizedFileDialog::accept);
    this->connect(m_fileDialog, &QFileDialog::finished, this,
                  &CustomizedFileDialog::done);
    this->connect(m_fileDialog, &QFileDialog::rejected, this,
                  &CustomizedFileDialog::reject);
}

/**
 * @brief       Constructor.
 */
CustomizedFileDialog::CustomizedFileDialog(QWidget *       parent,
                                           Qt::WindowFlags flags) :
    CustomizedDialog(CustomizedDialog::BorderType::Resizable,
                     TitleBar::TitleBarButton::AllButtons,
                     parent),
    m_fileDialog(new QFileDialog(this, flags))

{
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    m_fileDialog->setWindowFlags(Qt::WindowType::Widget);
    m_fileDialog->setOption(QFileDialog::Option::DontUseNativeDialog);
    this->setWidget(m_fileDialog);
    this->connect(m_fileDialog, &QFileDialog::accepted, this,
                  &CustomizedFileDialog::accept);
    this->connect(m_fileDialog, &QFileDialog::finished, this,
                  &CustomizedFileDialog::done);
    this->connect(m_fileDialog, &QFileDialog::rejected, this,
                  &CustomizedFileDialog::reject);
}

/**
 * @brief       Destructor.
 */
CustomizedFileDialog::~CustomizedFileDialog() {}

/**
 * @brief       Get accept mode.
 */
QFileDialog::AcceptMode CustomizedFileDialog::acceptMode() const
{
    return m_fileDialog->acceptMode();
}

/**
 * @brief       Set accept mode.
 */
void CustomizedFileDialog::setAcceptMode(QFileDialog::AcceptMode mode)
{
    m_fileDialog->setAcceptMode(mode);
}

/**
 * @brief       Get file mode.
 */
QFileDialog::FileMode CustomizedFileDialog::fileMode() const
{
    return m_fileDialog->fileMode();
}

/**
 * @brief       Set file mode.
 */
void CustomizedFileDialog::setFileMode(QFileDialog::FileMode mode)
{
    m_fileDialog->setFileMode(mode);
}

/**
 * @brief       Get filter.
 */
QDir::Filters CustomizedFileDialog::filter() const
{
    return m_fileDialog->filter();
}

/**
 * @brief       Set filter.
 */
void CustomizedFileDialog::setFilter(QDir::Filters filters)
{
    m_fileDialog->setFilter(filters);
}

/**
 * @brief       Get selected files.
 */
QStringList CustomizedFileDialog::selectedFiles() const
{
    return m_fileDialog->selectedFiles();
}

/**
 * @brief       Get path to save.
 */
QString CustomizedFileDialog::getSaveFileName(QWidget *      parent,
                                              const QString &caption,
                                              const QString &dir,
                                              const QString &filter,
                                              QString *      selectedFilter,
                                              QFileDialog::Options options)
{}

/**
 * @brief       Get path to open.
 */
QString CustomizedFileDialog::getOpenFileName(QWidget *      parent,
                                              const QString &caption,
                                              const QString &dir,
                                              const QString &filter,
                                              QString *      selectedFilter,
                                              QFileDialog::Options options)
{}

/**
 * @brief       Get paths to open.
 */
QStringList CustomizedFileDialog::getOpenFileNames(QWidget *      parent,
                                                   const QString &caption,
                                                   const QString &dir,
                                                   const QString &filter,
                                                   QString *selectedFilter,
                                                   QFileDialog::Options options)
{}
