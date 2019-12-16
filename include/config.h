#pragma once

#include <memory>
#include <cstdint>

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QJsonObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include <design_model/singleton.h>
#include <global.h>

/**
 * @brief   Config reader/writer.
 */
class Config : public Singleton<Config> {
        SIGNLETON_OBJECT(Config)
    public:
        /**
         * @breif   Value types.
         */
        enum ValueType {
            None            = 0x00,     //< The value does not exists.
            Bool            = 0x01,     //< The value is a boolean.
            Int             = 0x02,     //< The value is an integer.
            Float           = 0x03,     //< The value is a float.
            String          = 0x04,     //< The value is a string.
            BoolArray       = 0x11,     //< The value is a boolean array.
            IntArray        = 0x12,     //< The value is an integer array.
            FloatArray      = 0x13,     //< The value is a float array.
            StringArray     = 0x14,     //< The value is a string array.
            ConfigArray     = 0x15      //< The value is a config object array.
        };

    private:
        QJsonObject                             m_root;         //< Root node.
        ::std::shared_ptr<Config>               m_parent;       //< Parent node.
        QReadWriteLock                          m_lock;         //< Lock.
        ::std::shared_ptr<Global>               m_globalInfo;   //< Global information.
        QMap<QString, ::std::weak_ptr<Config>>  m_children;     //< Child nodes.

    protected:
        /**
         * @brief   Constructor.
         */
        Config();

        /**
         * @brief       Construct a config from an existing object.
         *
         * @param[in]   root    Root json node.
         * @param[in]   parent  Parent config node.
         */
        Config(QJsonObject&& root, ::std::shared_ptr<Config> parent);

    public:
        /// Check value
        ValueType       valueType(const QString& key);
        bool            getBool(const QString& key, bool defaultVal);
        int64_t         getInt(const QString& key, int64_t defaultVal);
        double          getFloat(const QString& key, double defaultVal);
        QString         getString(const QString& key, const QString& defaultVal);

        void            setBool(const QString& key, bool value);
        void            setInt(const QString& key, int64_t value);
        void            setFloat(const QString& key, double value);
        void            setString(const QString& key, const QString& value);

        QVector<bool>       getBoolArray(const QString& key);
        QVector<int64_t>    getIntArray(const QString& key);
        QVector<double>     getFloatArray(const QString& key);
        QVector<QString>    getStringArray(const QString& key);
        QVector<::std::shared_ptr<Config>>  getConfigArray(const QString& key);

        void    setBoolArray(const QString& key, const QVector<bool>& value);
        void    setIntArray(const QString& key, const QVector<int64_t>& value);
        void    setFloatArray(const QString& key, const QVector<double>& value);
        void    setStringArray(const QString& key, const QVector<QString>& value);
        void    setConfigArray(const QString& key,
                               const QVector<::std::shared_ptr<Config>>& value);

        virtual ~Config();

};
