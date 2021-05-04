#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QLocale>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

#include <config.h>
#include <locale/string_table.h>

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

QMap<QString, QLocale> StringTable::_qtLanguageTable(
    {{"zh_CN", QLocale(QLocale::Language::Chinese,
                       QLocale::Script::SimplifiedChineseScript,
                       QLocale::Country::AnyCountry)},
     {"zh_TW", QLocale(QLocale::Language::Chinese,
                       QLocale::Script::TraditionalChineseScript,
                       QLocale::Country::AnyCountry)},
     {"en_US", QLocale(QLocale::Language::English)},
     {"de_DE", QLocale(QLocale::Language::German)},
     {"fr_FR", QLocale(QLocale::Language::French)},
     {"it_IT", QLocale(QLocale::Language::Italian)},
     {"pt_PT", QLocale(QLocale::Language::Portuguese)},
     {"es_ES", QLocale(QLocale::Language::Spanish)},
     {"ru_RU", QLocale(QLocale::Language::Russian)},
     {"ko_KR", QLocale(QLocale::Language::Korean)},
     {"ja_JP", QLocale(QLocale::Language::Japanese)}});

QMap<QString, uint32_t> StringTable::_languageIDTable({{"zh_CN", 86},
                                                       {"zh_TW", 88},
                                                       {"en_US", 44},
                                                       {"de_DE", 49},
                                                       {"fr_FR", 33},
                                                       {"it_IT", 39},
                                                       {"pt_PT", 55},
                                                       {"es_ES", 34},
                                                       {"ru_RU", 7},
                                                       {"ko_KR", 82},
                                                       {"ja_JP", 81}});

/**
 * @brief   Constructor.
 */
StringTable::StringTable() :
    QObject(nullptr), m_notFoundStr("---INNEKGAL-STRING-ID---")
{
    // Get language.
    m_language
        = Config::instance()->getString("/language", this->systemLanguage());
    Config::instance()->setString("/language", m_language);
    m_languageID = _languageIDTable[m_language];
    qDebug() << "Language : " << m_language << ".";
    this->updateLocale();

    // Read string tables.
    QDir stringDir(":/StringTable");
    for (QString &name : stringDir.entryList()) {
        QString path = stringDir.absoluteFilePath(name);
        qDebug() << "Loading string table :" << path;
        QFile jsonFile(path);
        if (! jsonFile.open(QFile::ReadOnly)) {
            qDebug() << "Failed to open string table.";
            return;
        }
        QByteArray jsonStr = jsonFile.readAll();
        jsonFile.close();

        // Parse string table
        QJsonParseError err;
        QJsonDocument   doc = QJsonDocument::fromJson(jsonStr, &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "Failed to parse string table : " << err.errorString();
            return;
        }
        QJsonObject root = doc.object();
        for (auto iter = root.begin(); iter != root.end(); iter++) {
            QJsonValue value = iter.value();
            if (! value.isObject()) {
                qDebug() << "Illegal string, ID = " << iter.key() << ".";
                return;
            }

            m_stringTable[iter.key()] = QMap<QString, QString>();

            QJsonObject strObject   = value.toObject();
            bool        defaultFlag = false;
            for (auto strIter = strObject.begin(); strIter != strObject.end();
                 strIter++) {
                QJsonValue value = strIter.value();
                if (! value.isString()) {
                    qDebug() << "Illegal string, ID = " << iter.key() << ".";
                    return;
                }
                m_stringTable[iter.key()][strIter.key()]
                    = strIter.value().toString();
                if (strIter.key() == "en_US") {
                    defaultFlag = true;
                }
            }
            if (defaultFlag) {
                qDebug() << "String" << iter.key() << "loaded.";
            } else {
                qDebug() << "String" << iter.key()
                         << "requires \"en_US\" support.";
                return;
            }
        }
    }

    this->setInitialized();
}

/**
 * @brief	Get current language.
 */
const QString &StringTable::language()
{
    QReadLocker lock(&m_lock);
    return m_language;
}

/**
 * @brief	Get current language ID.
 */
uint32_t StringTable::languageId()
{
    QReadLocker lock(&m_lock);
    return m_languageID;
}

/**
 * @brief		Get string.
 */
const QString &StringTable::getString(const QString &id)
{
    QReadLocker lock(&m_lock);
    auto        iter = m_stringTable.find(id);
    if (iter == m_stringTable.end()) {
        qDebug() << "Illegal string ID :" << id << ".";
        return m_notFoundStr;
    }
    auto strIter = (*iter).find(m_language);
    if (strIter == (*iter).end()) {
        return (*iter)["en_US"];
    } else {
        return *strIter;
    }
}

/**
 * @brief		Get string in all languages.
 */
const QMap<QString, QString> &StringTable::getStrings(const QString &id)
{
    QReadLocker lock(&m_lock);

    auto iter = m_stringTable.find(id);
    if (iter == m_stringTable.end()) {
        qDebug() << "Illegal string ID :" << id << ".";
        return m_notFoundMap;
    }

    return *iter;
}

/**
 * @brief       Set current language.
 *  Set current locale to locale. If the locale does not supported,
 *  current locale will be set to default locale.
 *
 */
void StringTable::setLanguage(const QString &language)
{
    {
        QWriteLocker lock(&m_lock);
        if (language == m_language) {
            return;
        }

        // Search language
        auto iter = _languageIDTable.find(language);
        if (iter == _languageIDTable.end()) {
            return;
        }

        m_language   = language;
        m_languageID = *iter;
        Config::instance()->setString("/language", m_language);
    }
    this->updateLocale();
    emit this->languageChanged();
    emit this->afterLanguageChanged();
}

/**
 * @brief   Get a \c QCollator object.
 */
QCollator StringTable::collator()
{
    return QCollator(_qtLanguageTable[m_language]);
}

/**
 * @brief Destructor.
 */
StringTable::~StringTable() {}

/**
 * @brief	Get system locale.
 */
QString StringTable::systemLanguage()
{
    QLocale locale = QLocale::system();
    auto    iter   = _languageTable.find(locale.language());

    if (iter == _languageTable.end()) {
        return "en_US";
    } else {
        if (*iter == "zh_CN" && locale.country() != QLocale::Country::China) {
            // Where is QLocale::Country::Scotland, QLocale::Country::Catalonian
            // or QLocale::Country::California.
            return "zh_TW";
        }
        return *iter;
    }
}

/**
 * @brief	Update default locale.
 */
void StringTable::updateLocale()
{
    QLocale::setDefault(_qtLanguageTable[m_language]);
}
