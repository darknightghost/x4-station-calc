#pragma once

#include <QtCore/QObject>

#include <design_model/singleton.h>

/**
 * @brief   String table.
 */
class StringTable : public QObject, public Singleton<StringTable> {
        Q_OBJECT
        SIGNLETON_OBJECT(StringTable)
    protected:
        /**
         * @brief   Constructor.
         */
        StringTable();

    public slots:
        /**
         * @brief       Set current locale.
         *  Set current locale to locale. If the locale does not supported,
         *  current locale will be set to default locale.
         *
         * @param[in]   locale      Locale.
         *
         */
        void        setLocale(const QString locale);

    signals:
        /**
         * @brief   Emit when locale changes.
         */
        void        updateString();

    public:
        /**
         * @brief Destructor.
         */
        virtual ~StringTable();
};
