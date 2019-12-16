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
        /**
         * @brief       Get value type.
         *
         * @param[in]   key         Key of the value.
         *
         * @return      Type of the value, if the value does not exists,
         *              ValueType::None is returned.
         */
        ValueType       valueType(const QString& key);

        /// Getters of simple values.
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
        bool            getBool(const QString& key, bool defaultVal);

        /**
         * @brief       Get integer value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   defaultVal  Default value.
         *
         * @return      On success, the value of the key is returned. If the
         *              value does not exists or the type of value does not
         *              match, the default value is returned.
         */
        int64_t         getInt(const QString& key, int64_t defaultVal);

        /**
         * @brief       Get float value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   defaultVal  Default value.
         *
         * @return      On success, the value of the key is returned. If the
         *              value does not exists or the type of value does not
         *              match, the default value is returned.
         */
        double          getFloat(const QString& key, double defaultVal);

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
        QString         getString(const QString& key, const QString& defaultVal);

        /// Setters of simple values
        /**
         * @brief       Set boolean value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   value       Value to set.
         */
        void            setBool(const QString& key, bool value);

        /**
         * @brief       Set integer value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   value       Value to set.
         */
        void            setInt(const QString& key, int64_t value);

        /**
         * @brief       Set float value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   value       Value to set.
         */
        void            setFloat(const QString& key, double value);

        /**
         * @brief       Set string value.
         *
         * @param[in]   key         Key of the value.
         * @param[in]   value       Value to set.
         */
        void            setString(const QString& key, const QString& value);

        /// Getters of array
        /**
         * @brief       Get boolean array.
         *
         * @param[in]   key         Key of the array.
         *
         * @return      On success, the value array of the key is returned. if
         *              the value does not exists or the type of value does not
         *              match, an empty vector is returned.
         */
        QVector<bool>       getBoolArray(const QString& key);

        /**
         * @brief       Get integer array.
         *
         * @param[in]   key         Key of the array.
         *
         * @return      On success, the value array of the key is returned. if
         *              the value does not exists or the type of value does not
         *              match, an empty vector is returned.
         */
        QVector<int64_t>    getIntArray(const QString& key);

        /**
         * @brief       Get float array.
         *
         * @param[in]   key         Key of the array.
         *
         * @return      On success, the value array of the key is returned. if
         *              the value does not exists or the type of value does not
         *              match, an empty vector is returned.
         */
        QVector<double>     getFloatArray(const QString& key);

        /**
         * @brief       Get string array.
         *
         * @param[in]   key         Key of the array.
         *
         * @return      On success, the value array of the key is returned. if
         *              the value does not exists or the type of value does not
         *              match, an empty vector is returned.
         */
        QVector<QString>    getStringArray(const QString& key);

        /**
         * @brief       Get config array.
         *
         * @param[in]   key         Key of the array.
         *
         * @return      On success, the value array of the key is returned. if
         *              the value does not exists or the type of value does not
         *              match, an empty vector is returned.
         */
        QVector<::std::shared_ptr<Config>>  getConfigArray(
                                            const QString& key);

        /// Setters of array
        /**
         * @brief       Set boolean array.
         *
         * @param[in]   key         Key of the array.
         * @param[in]   value       Array to set.
         */
        void    setBoolArray(const QString& key, const QVector<bool>& value);

        /**
         * @brief       Set integer array.
         *
         * @param[in]   key         Key of the array.
         * @param[in]   value       Array to set.
         */
        void    setIntArray(const QString& key, const QVector<int64_t>& value);

        /**
         * @brief       Set float array.
         *
         * @param[in]   key         Key of the array.
         * @param[in]   value       Array to set.
         */
        void    setFloatArray(const QString& key, const QVector<double>& value);

        /**
         * @brief       Set string array.
         *
         * @param[in]   key         Key of the array.
         * @param[in]   value       Array to set.
         */
        void    setStringArray(const QString& key, const QVector<QString>& value);

        /**
         * @brief       Set config array.
         *
         * @param[in]   key         Key of the array.
         * @param[in]   value       Array to set.
         */
        void    setConfigArray(const QString& key,
                               const QVector<::std::shared_ptr<Config>>& value);

        virtual ~Config();

    private:
        /**
         * @brief   Load data from config file.
         */
        void                loadFile();

        /**
         * @brief   Save data to config file.
         */
        void                saveFaile();

};
