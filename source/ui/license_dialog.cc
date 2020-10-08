#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QScrollBar>

#include <locale/string_table.h>
#include <ui/license_dialog.h>

/**
 * @brief	Constructor.
 */
LicenseDialog::LicenseDialog() : QDialog(nullptr)
{
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);

    /// Layout
    m_layout = new QVBoxLayout();

    /// Text
    m_txtLicense = new QTextEdit();
    m_txtLicense->setReadOnly(true);
    QString templateCode;
    for (int i = 0; i < 80; i++) {
        templateCode += 'A';
    }
    QSize txtSize
        = m_txtLicense->fontMetrics().boundingRect(templateCode).size();
    txtSize.setHeight(txtSize.height() * 25);
    m_txtLicense->setMinimumSize(txtSize);
    this->loadLicense();
    m_txtLicense->moveCursor(QTextCursor::MoveOperation::Start);
    m_txtLicense->verticalScrollBar()->setValue(0);
    m_layout->addWidget(m_txtLicense);

    /// Buttons
    /// Laoyout
    m_layoutButton = new QHBoxLayout();

    /// Button agree
    m_layoutButton->addStretch();
    m_btnAgree = new QPushButton(STR("STR_BTN_AGREE"));
    this->connect(m_btnAgree, &QPushButton::clicked, this,
                  &LicenseDialog::accept);
    m_layoutButton->addWidget(m_btnAgree);

    /// Button disagree
    m_layoutButton->addStretch();
    m_btnDisagree = new QPushButton(STR("STR_BTN_DISAGREE"));
    this->connect(m_btnDisagree, &QPushButton::clicked, this,
                  &LicenseDialog::reject);
    m_layoutButton->addWidget(m_btnDisagree);
    m_layoutButton->addStretch();

    m_layout->addLayout(m_layoutButton);

    this->setLayout(m_layout);
    this->setWindowTitle(STR("STR_TITLE_LICENSE"));

    /// Window size and position.
    m_layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    QDesktopWidget *desktop = QApplication::desktop();
    QSize           sz      = m_layout->sizeHint();
    this->setGeometry((desktop->width() - sz.width()) / 2,
                      (desktop->height() - sz.height()) / 2, sz.width(),
                      sz.height());
}

/**
 * @brief	Destructor.
 */
LicenseDialog::~LicenseDialog() {}

/**
 * @brief	Load license data.
 */
void LicenseDialog::loadLicense()
{
    /// Head
    QString document;
    {
        QFile licenseFile(":/License/head");
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
    }

    /// License
    {
        QFile licenseFile(QString(":/License/license.%1")
                              .arg(StringTable::instance()->language()));
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
        document += "\n<hr />\n";
    }

    /// GPLv3
    {
        QFile licenseFile(":/License/gplv3");
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
        document += "\n<hr />\n";
    }

    /// 3rd-party
    document += QString("\n<h1 style=\"text-align: center;\">%1</h1>\n")
                    .arg(STR("STR_3RD_LICENSE"));
    document += "\n<hr />\n";
    /// Qt5(LGPLv3)
    {
        QFile licenseFile(":/License/lgplv3");
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
        document += "\n<hr />\n";
    }

    /// Egosoft
    {
        QFile licenseFile(":/License/egosoft");
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
        document += "\n<hr />\n";
    }

    /// Tail
    {
        QFile licenseFile(":/License/tail");
        if (! licenseFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open license file.";
            this->reject();
            return;
        }
        QString license = licenseFile.readAll();
        licenseFile.close();
        document += license;
    }

    m_txtLicense->setHtml(document);
}
