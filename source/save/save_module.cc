#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include <game_data/game_data.h>
#include <save/save.h>
#include <save/save_module.h>

QMap<QString, QString> SaveModule::_idMacroMap; ///< ID to macro map.

/**
 * @brief		Create a module information.
 */
SaveModule::SaveModule(const QString &module) : m_module(module), m_amount(1)
{
    if (GameData::instance()->stationModules()->module(module) != nullptr) {
        this->setInitialized();
    }
}

/**
 * @brief		Load a module information.
 */
SaveModule::SaveModule(QJsonObject &entry, const SaveVersion &version)
{
    if (version < SaveVersion(1, 0, 0)) {
        this->load0_x_x();
        if (! entry.contains("id")) {
            return;
        }
        if (! entry.contains("amount")) {
            return;
        }
        QJsonValue idValue = entry.value("id");
        if (! idValue.isString()) {
            return;
        }
        QJsonValue amountValue = entry.value("amount");
        if (! amountValue.isDouble()) {
            return;
        }
        auto iter = _idMacroMap.find(idValue.toString());
        if (iter == _idMacroMap.end()) {
            return;
        }
        m_module = *iter;
        m_amount = (quint64)(amountValue.toInt());

        this->setInitialized();
    } else {
        if (! entry.contains("macro")) {
            return;
        }
        if (! entry.contains("amount")) {
            return;
        }
        QJsonValue macroValue = entry.value("macro");
        if (! macroValue.isString()) {
            return;
        }
        m_module = macroValue.toString();

        QJsonValue amountValue = entry.value("amount");
        if (! amountValue.isDouble()) {
            return;
        }
        m_amount = (quint64)(amountValue.toInt());

        this->setInitialized();
    }
}

/**
 * @brief		Get module ID.
 */
const QString &SaveModule::module() const
{
    return m_module;
}

/**
 * @brief		Get amount.
 */
quint64 SaveModule::amount() const
{
    return m_amount;
}

/**
 * @brief		Set amount.
 */
void SaveModule::setAmount(quint64 amount)
{
    if (amount < 1) {
        m_amount = 1;
    } else {
        m_amount = amount;
    }
}

/**
 * @brief		Parse to json object.
 */
QJsonObject SaveModule::toJson() const
{
    QJsonObject ret;
    ret.insert("macro", m_module);
    ret.insert("amount", (qint64)m_amount);

    return ret;
}

/**
 * @brief		Destructor.
 */
SaveModule::~SaveModule() {}

/**
 * @brief	Load id map of old version.
 */
void SaveModule::load0_x_x()
{
    if (! _idMacroMap.empty()) {
        return;
    }

    // Load config data.
    QFile      file(":/Save/id_map_0.x.x.json");
    QByteArray jsonStr;

    if (file.open(QIODevice::ReadOnly)) {
        jsonStr = file.readAll();
        file.close();
    } else {
        qCritical() << "Missing resource \":/Save/id_map_0.x.x.json\".";
    }

    // Parse data.
    QJsonParseError jsonError;
    QJsonDocument   doc = QJsonDocument::fromJson(jsonStr, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qCritical() << "Illegal resource file \":/Save/id_map_0.x.x.json\".";
    }

    //  Load pairs.
    QJsonObject root = doc.object();
    for (auto &k : root.keys()) {
        _idMacroMap[k] = root.value(k).toString();
    }

    return;
}
