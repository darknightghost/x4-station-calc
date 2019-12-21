#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <locale/string_table.h>
#include <ui/license_dialog.h>

/**
 * @brief	Constructor.
 */
LicenseDialog::LicenseDialog() : QDialog(nullptr)
{
    /// Load data
    QFile licenseFile(":/License/gplv3.html");
    if (! licenseFile.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open license file.";
        this->reject();
        return;
    }
    QString license = licenseFile.readAll();
    licenseFile.close();

    /// Layout
    m_layout = new QVBoxLayout();

    /// Text
    m_txtLicense = new QTextEdit();
    m_txtLicense->insertHtml(license);
    m_txtLicense->setReadOnly(true);
    QString templateCode;
    for (int i = 0; i < 80; i++) {
        templateCode += 'A';
    }
    QSize txtSize
        = m_txtLicense->fontMetrics().boundingRect(templateCode).size();
    txtSize.setHeight(txtSize.height() * 25);
    m_txtLicense->setMinimumSize(txtSize);
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
