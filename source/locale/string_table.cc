#include <locale/string_table.h>

/**
 * @brief   Constructor.
 */
StringTable::StringTable():
    QObject(nullptr)
{
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
void StringTable::setLocale(const QString locale)
{
}

/**
 * @brief Destructor.
 */
StringTable::~StringTable()
{
}
