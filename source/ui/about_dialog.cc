#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <locale/string_table.h>
#include <ui/about_dialog.h>
#include <version.h>

/**
 * @brief       Constructor.
 */
AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(STR("STR_TITLE_ABOUT"));

    m_layout = new QVBoxLayout();
    this->setLayout(m_layout);

    m_lblTitle = new QLabel("X4 Station Calculator");
    m_layout->addWidget(m_lblTitle);
    m_lblTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

    m_lblVersion = new QLabel(STR("STR_ABOUT_VERSION").arg(VERSION_STR));
    m_layout->addWidget(m_lblVersion);
    m_lblVersion->setAlignment(Qt::AlignmentFlag::AlignCenter);

    m_scrollChanglog = new QScrollArea();
    m_layout->addWidget(m_scrollChanglog);

    m_layoutScrollArea = new QVBoxLayout();
    m_scrollChanglog->setLayout(m_layoutScrollArea);

    m_txtChangelog = new QTextEdit();
    m_layoutScrollArea->addWidget(m_txtChangelog);
    m_txtChangelog->setAlignment(Qt::AlignmentFlag::AlignLeft);
    m_txtChangelog->setReadOnly(true);

    // Read changelog.
    QFile file(":/Text/changelog");
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    m_txtChangelog->setText(QString::fromUtf8(file.readAll()));

    // Size.
    int width  = QApplication::desktop()->width() / 3;
    int height = QApplication::desktop()->height() * 2 / 3;

    this->setFixedSize(width, height);
}

/**
 * @brief   Destructor.
 */
AboutDialog::~AboutDialog() {}
