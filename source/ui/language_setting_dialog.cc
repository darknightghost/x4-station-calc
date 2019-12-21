#include <QtCore/QDebug>
#include <QtCore/QMap>
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
    for (auto iter = languages.begin(); iter != languages.end(); iter++) {
        m_comboLang->addItem(*iter, iter.key());
    }
    m_comboLang->setCurrentIndex(
        m_comboLang->findText(STR("STR_LANGUAGE_TYPE")));
    m_comboLang->setMinimumWidth(m_comboLang->fontMetrics()
                                     .boundingRect(STR("STR_LANGUAGE_TYPE"))
                                     .width()
                                 * 4);

    m_selectionLayout->addWidget(m_comboLang);

    m_layout->addLayout(m_selectionLayout);

    this->setLayout(m_layout);
    this->setWindowTitle(STR("STR_TITLE_LANGUAGE"));

    /// Window size
    m_layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
}

/**
 * @brief	Destructor.
 */
LanguageSettingDialog::~LanguageSettingDialog() {}
