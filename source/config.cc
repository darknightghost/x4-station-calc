#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QJsonObject>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

#include <config.h>

/**
 * @brief   Constructor.
 */
Config::Config() : m_globalInfo(Global::instance())
{
    // Load config data.
    QFile      file(m_globalInfo->configPath());
    QByteArray jsonStr;

    if (file.open(QIODevice::ReadOnly)) {
        jsonStr = file.readAll();
        file.close();
    } else {
        jsonStr = "{}";
    }

    // Parse data.
    QJsonParseError jsonError;
    m_doc = QJsonDocument::fromJson(jsonStr, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << jsonError.errorString();
        m_doc.fromJson("{}", &jsonError);
    }

    this->setInitialized();
}

/**
 * @brief       Get value type.
 */
Config::ValueType Config::valueType(const QString &key)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return ValueType::None;
    }

    // Find value
    QReadLocker locker(&m_lock);
    QJsonValue  value;
    if (! this->findNode(splitedKey, value)) {
        return ValueType::None;
    }

    return this->valueType(value);
}

/**
 * @brief       Get boolean value.
 */
bool Config::getBool(const QString &key, bool defaultVal)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return defaultVal;
    }

    // Find value
    QReadLocker locker(&m_lock);
    QJsonValue  value;
    if (! this->findNode(splitedKey, value)) {
        return defaultVal;
    }

    // Check type.
    if (this->valueType(value) != ValueType::Bool) {
        return defaultVal;
    }

    // Return
    return value.toBool();
}

/**
 * @brief       Get float value.
 */
double Config::getFloat(const QString &key, double defaultVal)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return defaultVal;
    }

    // Find value
    QReadLocker locker(&m_lock);
    QJsonValue  value;
    if (! this->findNode(splitedKey, value)) {
        return defaultVal;
    }

    // Check type.
    if (this->valueType(value) != ValueType::Number) {
        return defaultVal;
    }

    // Return
    return value.toDouble();
}

/**
 * @brief       Get integer value.
 */
int64_t Config::getInt(const QString &key, int64_t defaultVal)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return defaultVal;
    }

    // Find value
    QReadLocker locker(&m_lock);
    QJsonValue  value;
    if (! this->findNode(splitedKey, value)) {
        return defaultVal;
    }

    // Check type.
    if (this->valueType(value) != ValueType::Number) {
        return defaultVal;
    }

    // Return
    return value.toInt();
}

/**
 * @brief       Get string value.
 */
QString Config::getString(const QString &key, const QString &defaultVal)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return defaultVal;
    }

    // Find value
    QReadLocker locker(&m_lock);
    QJsonValue  value;
    if (! this->findNode(splitedKey, value)) {
        return defaultVal;
    }

    // Check type.
    if (this->valueType(value) != ValueType::String) {
        return defaultVal;
    }

    // Return
    return value.toString();
}

// Setters of simple values
/**
 * @brief       Set boolean value.
 */
void Config::setBool(const QString &key, bool value)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return;
    }

    // Set value.
    QJsonValue v(value);
    this->setNode(splitedKey, v);
}

/**
 * @brief       Set float value.
 */
void Config::setFloat(const QString &key, double value)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return;
    }

    // Set value.
    QJsonValue v(value);
    this->setNode(splitedKey, v);
}

/**
 * @brief       Set number value.
 */
void Config::setInt(const QString &key, int64_t value)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return;
    }

    // Set value.
    QJsonValue v((qint64)value);
    this->setNode(splitedKey, v);
}

/**
 * @brief       Set string value.
 */
void Config::setString(const QString &key, const QString &value)
{
    // Split key.
    QVector<QString> splitedKey;
    if (! this->splitKey(key, splitedKey)) {
        return;
    }

    // Set value.
    QJsonValue v(value);
    this->setNode(splitedKey, v);
}

/**
 * @brief       Split key.
 */
bool Config::splitKey(const QString &key, QVector<QString> &splitedKey)
{
    if (key.isEmpty() || key[0] != '/') {
        return false;
    }

    for (auto &c : key) {
        if ((c > '9' || c < '0') && (c > 'z' || c < 'a') && (c > 'Z' || c < 'A')
            && (c != '/') && (c != '_')) {
            return false;
        }
    }

    splitedKey.clear();
    QStringList splitted = key.split('/');
    for (auto &s : splitted) {
        if (s != "") {
            splitedKey.append(s);
        }
    }

    return true;
}

/**
 * @brief       Join key.
 */
QString Config::joinKey(const QVector<QString> &splitted)
{
    QString ret;
    for (auto &s : splitted) {
        ret += '/';
        ret += s;
    }

    return ret;
}

/**
 * @brief       Search for node.
 */
bool Config::findNode(const QVector<QString> &key, QJsonValue &ret)
{
    if (key.empty()) {
        return false;
    }

    // Search
    QJsonObject obj = m_doc.object();
    for (auto iter = key.begin(); iter < key.end() - 1; iter++) {
        if (obj.contains(*iter)) {
            QJsonValue v = obj.value(*iter);
            if (v.isObject()) {
                obj = v.toObject();
            } else {
                return false;
            }
        }
    }

    if (obj.contains(key.back())) {
        ret = obj.value(key.back());
        return true;
    } else {
        return false;
    }
}

/**
 * @brief       Set node.
 */
void Config::setNode(const QVector<QString> &key, QJsonValue &node)
{
    if (key.empty()) {
        return;
    }

    QVector<QJsonObject> nodeStack;
    nodeStack.push_back(m_doc.object());

    // Search and create parent nodes
    for (auto iter = key.begin(); iter < key.end() - 1; iter++) {
        if (nodeStack.back().contains(*iter)) {
            QJsonValue v = nodeStack.back().value(*iter);
            if (v.isObject()) {
                nodeStack.push_back(v.toObject());
            } else {
                nodeStack.push_back(QJsonObject());
            }
        } else {
            nodeStack.push_back(QJsonObject());
        }
    }

    // Set value
    nodeStack.back().insert(key.back(), node);

    // Save nodes to document.
    for (auto iter = key.rbegin() + 1; iter < key.rend(); iter++) {
        QJsonObject obj(nodeStack.takeLast());
        nodeStack.back().insert(*iter, QJsonValue(obj));
    }

    m_doc.setObject(nodeStack.front());
}

/**
 * @brief       Get value type.
 */
Config::ValueType Config::valueType(QJsonValue &node)
{
    switch (node.type()) {
        case QJsonValue::Null:
            return ValueType::None;

        case QJsonValue::Bool:
            return ValueType::Bool;

        case QJsonValue::Double:
            return ValueType::Number;

        case QJsonValue::String:
            return ValueType::String;

        case QJsonValue::Object:
            return ValueType::Node;

        case QJsonValue::Undefined:
        default:
            return ValueType::None;
    }
}

/**
 * @brief   Destructor.
 */
Config::~Config()
{
    // Save config file.
    qDebug() << "Saving config file " << m_globalInfo->configPath() << ".";
    QFile file(m_globalInfo->configPath());

    if (! file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save config file " << m_globalInfo->configPath()
                 << ".";
        return;
    }

    file.write(m_doc.toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "Config file " << m_globalInfo->configPath() << " saved.";
}
