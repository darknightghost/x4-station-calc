#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

#include <design_model/singleton.h>

/**
 * @brief   String table.
 */
class StringTable : public QObject, public Singleton<StringTable> {
    Q_OBJECT
    SIGNLETON_OBJECT(StringTable)
  private:
    QReadWriteLock                        m_lock;     //< Lock;
    QMap<QString, QMap<QString, QString>> m_strings;  //< Strings.
    QString                               m_language; //< Language.
  private:
    static QMap<int, QString> _languageTable; //< Convert qt language to locale.

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
    void setLocale(const QString locale);

  public:
  signals:
    /**
     * @brief   Emit when locale changes.
     */
    void updateString();

  public:
    /**
     * @brief Destructor.
     */
    virtual ~StringTable();

  private:
    /**
     * @brief	Get system locale.
     *
     * @return	System locale.
     */
    QString systemLanguage();
};
