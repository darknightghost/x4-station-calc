#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QtGlobal>
#include <QtGui/QResizeEvent>
#include <QtGui/QShowEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <locale/string_table.h>
#include <ui/language_setting_dialog.h>

/**
 * @brief	Constructor.
 */
LanguageSettingDialog::LanguageSettingDialog() : QDialog(nullptr)
{
    /// Create widgets
    m_layout = new QVBoxLayout();

    /// Selections
    /// Layout
    m_selectionLayout = new QHBoxLayout();

    /// Label
    m_lblLang = new QLabel(STR("STR_LANGUAGE"));
    m_selectionLayout->addWidget(m_lblLang);

    /// Combobox
    m_comboLang     = new QComboBox();
    auto &languages = StringTable::instance()->getStrings("STR_LANGUAGE_TYPE");
    for (auto iter = languages.begin(); iter != languages.end(); iter++)
    {
        m_comboLang->addItem(*iter, iter.key());
    }
    m_comboLang->setCurrentIndex(
        m_comboLang->findText(STR("STR_LANGUAGE_TYPE")));
    m_comboLang->setMinimumWidth(m_comboLang->fontMetrics()
                                     .boundingRect(STR("STR_LANGUAGE_TYPE"))
                                     .width()
                                 * 4);
    this->connect(m_comboLang,
                  QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                  &LanguageSettingDialog::onSelectedChanged);

    m_selectionLayout->addWidget(m_comboLang);
    m_layout->addLayout(m_selectionLayout);

    /// Buttons
    /// Layout
    m_btnLayout = new QHBoxLayout();

    /// Button OK
    m_btnLayout->addStretch();
    m_btnOK = new QPushButton(STR("STR_BTN_OK"));
    this->connect(m_btnOK, &QPushButton::clicked, this, &QDialog::accept);
    m_btnLayout->addWidget(m_btnOK);

    m_btnLayout->addStretch();
    m_layout->addLayout(m_btnLayout);

    // Set layout
    this->setLayout(m_layout);
    this->setWindowTitle(STR("STR_TITLE_LANGUAGE"));

    /// Window size and position.
    m_layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    QDesktopWidget *desktop = QApplication::desktop();
    QSize           sz      = m_layout->sizeHint();
    this->setGeometry((desktop->width() - sz.width()) / 2,
                      (desktop->height() - sz.height()) / 2, sz.width(),
                      sz.height());

    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &LanguageSettingDialog::onLanguageChanged);
}

/**
 * @brief	Destructor.
 */
LanguageSettingDialog::~LanguageSettingDialog() {}

/**
 * @brief	Language changed slot.
 */
void LanguageSettingDialog::onLanguageChanged()
{
    this->setWindowTitle(STR("STR_TITLE_LANGUAGE"));
    m_lblLang->setText(STR("STR_LANGUAGE"));
    m_btnOK->setText(STR("STR_BTN_OK"));
}

/**
 * @brief	Selected language changed slot.
 */
void LanguageSettingDialog::onSelectedChanged(int index)
{
    StringTable::instance()->setLanguage(
        m_comboLang->itemData(index).toString());
}
