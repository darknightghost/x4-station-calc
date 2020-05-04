#pragma once

#include <cstdint>
#include <memory>

#include <QtCore/QJsonDocument>
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <design_models/singleton.h>
#include <global.h>

/**
 * @brief   Config reader/writer.
 */
class Config : public Singleton<Config> {
    SIGNLETON_OBJECT(Config)
  public:
    /**
     * @brief   Value types.
     */
    enum ValueType {
        None   = 0x00, ///< The value does not exists.
        Bool   = 0x01, ///< The value is a boolean.
        Number = 0x02, ///< The value is a number.
        String = 0x03, ///< The value is a string.
        Node   = 0x04, ///< The value is a config node.
    };

  private:
    QJsonDocument             m_doc;        ///< Document.
    QReadWriteLock            m_lock;       ///< Lock.
    ::std::shared_ptr<Global> m_globalInfo; ///< Global information.

  protected:
    /**
     * @brief   Constructor.
     */
    Config();

    /**
     * @brief       Construct a config from an existing object.
     *
     * @param[in]   key     Key of the node.
     * @param[in]   root    Root json node.
     * @param[in]   parent  Parent config node.
     */
    Config(const QString &           key,
           QJsonObject &&            root,
           ::std::shared_ptr<Config> parent);

  public:
    // Valu types.
    /**
     * @brief       Get value type.
     *
     * @param[in]   key         Key of the value.
     *
     * @return      Type of the value, if the value does not exists,
     *              \c ValueType::None is returned.
     */
    ValueType valueType(const QString &key);

    // Getters
    /**
     * @brief       Get boolean value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   defaultVal  Default value.
     *
     * @return      On success, the value of the key is returned. If the
     *              value does not exists or the type of value does not
     *              match, the default value is returned.
     */
    bool getBool(const QString &key, bool defaultVal);

    /**
     * @brief       Get number value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   defaultVal  Default value.
     *
     * @return      On success, the value of the key is returned. If the
     *              value does not exists or the type of value does not
     *              match, the default value is returned.
     */
    double getNumber(const QString &key, double defaultVal);

    /**
     * @brief       Get string value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   defaultVal  Default value.
     *
     * @return      On success, the value of the key is returned. If the
     *              value does not exists or the type of value does not
     *              match, the default value is returned.
     */
    QString getString(const QString &key, const QString &defaultVal);

    // Setters
    /**
     * @brief       Set boolean value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   value       Value to set.
     */
    void setBool(const QString &key, bool value);

    /**
     * @brief       Set number value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   value       Value to set.
     */
    void setNumber(const QString &key, double value);

    /**
     * @brief       Set string value.
     *
     * @param[in]   key         Key of the value.
     * @param[in]   value       Value to set.
     */
    void setString(const QString &key, const QString &value);

    /**
     * @brief   Destructor.
     */
    virtual ~Config();

  private:
    /**
     * @brief       Split key.
     *
     * @param[in]   key         Key to split.
     * @param[out]  splitedKey  Splited key.
     *
     * @return      On success, true is returned, otherwose returns false.
     */
    bool splitKey(const QString &key, QVector<QString> &splitedKey);

    /**
     * @brief       Join key.
     *
     * @param[out]  splitted	Splitted key.
     *
     * @return      Joined key.
     */
    QString joinKey(const QVector<QString> &splitted);

    /**
     * @brief       Search for node.
     *
     * @param[in]	key			Splited key.
     * @param[out]  ret			Node.
     *
     * @return      On success, true is returned, otherwose returns false.
     */
    bool findNode(const QVector<QString> &key, QJsonValue &ret);

    /**
     * @brief       Set node.
     *
     * @param[in]	key			Splited key.
     * @param[in]	node		Node to set.
     */
    void setNode(const QVector<QString> &key, QJsonValue &node);

    /**
     * @brief       Get value type.
     *
     * @param[in]	node		Node to set.
     *
     * @return		Type of value.
     */
    ValueType valueType(QJsonValue &node);
};
