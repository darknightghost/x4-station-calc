#include <QtCore/QDebug>
#include <QtCore/QLocale>

#include <config.h>
#include <locale/string_table.h>

/**
 * @brief   Constructor.
 */
StringTable::StringTable() : QObject(nullptr)
{
    /// Get language.
    m_language
        = Config::instance()->getString("/language", this->systemLanguage());
    Config::instance()->setString("/language", m_language);
    qDebug() << "Language : " << m_language << ".";
    this->setGood();
}

/**
 * @brief       Set current locale.
 *  Set current locale to locale. If the locale does not supported,
 *  current locale will be set to default locale.
 *
 * @param[in]   locale      Locale.
 *
 */
void StringTable::setLocale(const QString locale) {}

/**
 * @brief Destructor.
 */
StringTable::~StringTable() {}

QMap<int, QString>
    StringTable::_languageTable({{QLocale::Language::Chinese, "zh_CN"},
                                 {QLocale::Language::English, "en_US"},
                                 {QLocale::Language::German, "de_DE"},
                                 {QLocale::Language::French, "fr_FR"},
                                 {QLocale::Language::Italian, "it_IT"},
                                 {QLocale::Language::Portuguese, "pt_PT"},
                                 {QLocale::Language::Spanish, "es_ES"},
                                 {QLocale::Language::Russian, "ru_RU"},
                                 {QLocale::Language::Korean, "ko_KR"},
                                 {QLocale::Language::Japanese, "ja_JP"}});

/**
 * @brief	Get system locale.
 *
 * @return	System locale.
 */
QString StringTable::systemLanguage()
{
    QLocale locale = QLocale::system();
    auto    iter   = _languageTable.find(locale.language());

    if (iter == _languageTable.end()) {
        return "en_US";
    } else {
        return *iter;
    }
}
