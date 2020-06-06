#include <QtCore/QString>
#include <QtCore/QStringList>

#include <save/save_version.h>

/**
 * @brief		Constructor.
 */
SaveVersion::SaveVersion(quint8 major, quint8 minor, quint8 maintenance) :
    m_major(major), m_minor(minor), m_maintenance(maintenance)
{
    this->setGood();
}

/**
 * @brief		Constructor.
 */
SaveVersion::SaveVersion(const QString &versionString)
{
    auto splitted
        = versionString.split(",", QString::SplitBehavior::SkipEmptyParts);
    if (splitted.size() != 3) {
        return;
    }
    m_major       = (quint8)splitted[0].toUShort();
    m_minor       = (quint8)splitted[1].toUShort();
    m_maintenance = (quint8)splitted[2].toUShort();

    this->setGood();
}

/**
 * @brief		Get major version.
 */
quint8 SaveVersion::major() const
{
    return m_major;
}

/**
 * @brief		Set major version.
 */
void SaveVersion::setMajor(quint8 major)
{
    m_major = major;
}

/**
 * @brief		Get minor version.
 */
quint8 SaveVersion::minor() const
{
    return m_minor;
}

/**
 * @brief		Set major version.
 */
void SaveVersion::setMinor(quint8 minor)
{
    m_minor = minor;
}

/**
 * @brief		Get maintenance version.
 */
quint8 SaveVersion::maintenance() const
{
    return m_maintenance;
}

/**
 * @brief		Set maintenance version.
 */
void SaveVersion::setMaintenance(quint8 maintenance)
{
    m_maintenance = maintenance;
}

/**
 * @brief		Destructor.
 */
SaveVersion::~SaveVersion() {}

/**
 * @brief		Convert to \c QString.
 */
SaveVersion::operator QString() const
{
    return QString("%1.%2.%3")
        .arg((int)m_major)
        .arg((int)m_minor)
        .arg((int)m_maintenance);
}

/**
 * @brief		Compare if \c *this > version.
 */
bool SaveVersion::operator>(const SaveVersion &version) const
{
    if (m_major != version.m_major) {
        return m_major > version.m_major;
    }

    if (m_minor != version.m_minor) {
        return m_minor > version.m_minor;
    }

    if (m_maintenance != version.m_maintenance) {
        return m_maintenance > version.m_maintenance;
    }

    return false;
}

/**
 * @brief		Compare if \c *this >= version.
 */
bool SaveVersion::operator>=(const SaveVersion &version) const
{
    if (m_major != version.m_major) {
        return m_major > version.m_major;
    }

    if (m_minor != version.m_minor) {
        return m_minor > version.m_minor;
    }

    if (m_maintenance != version.m_maintenance) {
        return m_maintenance > version.m_maintenance;
    }

    return true;
}

/**
 * @brief		Compare if \c *this < version.
 */
bool SaveVersion::operator<(const SaveVersion &version) const
{
    if (m_major != version.m_major) {
        return m_major < version.m_major;
    }

    if (m_minor != version.m_minor) {
        return m_minor < version.m_minor;
    }

    if (m_maintenance != version.m_maintenance) {
        return m_maintenance < version.m_maintenance;
    }

    return false;
}

/**
 * @brief		Compare if \c *this <= version.
 */
bool SaveVersion::operator<=(const SaveVersion &version) const
{
    if (m_major != version.m_major) {
        return m_major < version.m_major;
    }

    if (m_minor != version.m_minor) {
        return m_minor < version.m_minor;
    }

    if (m_maintenance != version.m_maintenance) {
        return m_maintenance < version.m_maintenance;
    }

    return true;
}

/**
 * @brief		Compare if \c *this == version.
 */
bool SaveVersion::operator==(const SaveVersion &version) const
{
    return (m_major == version.m_major) && (m_minor == version.m_minor)
           && (m_maintenance == version.m_maintenance);
}

/**
 * @brief		Compare if \c *this != version.
 */
bool SaveVersion::operator!=(const SaveVersion &version) const
{
    return (m_major != version.m_major) || (m_minor != version.m_minor)
           || (m_maintenance != version.m_maintenance);
}
