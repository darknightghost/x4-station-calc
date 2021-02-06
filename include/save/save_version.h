#pragma once

#include <QtCore/QString>

#include <interfaces/i_initialized.h>

/**
 * @brief		Version of save file.
 */
class SaveVersion : virtual public IInitialized
{
  protected:
    quint8 m_major;       ///< Major version.
    quint8 m_minor;       ///< Minor version.
    quint8 m_maintenance; ///< Maintenance version.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	major			Major version.
     * @param[in]	minor           Minor version.
     * @param[in]	maintenance     Maintenance version.
     */
    SaveVersion(quint8 major, quint8 minor, quint8 maintenance);

    /**
     * @brief		Constructor.
     *
     * @param[in]	versionString		Versionj string.
     */
    SaveVersion(const QString &versionString);

    /**
     * @brief		Get major version.
     *
     * @return		Major version.
     */
    quint8 major() const;

    /**
     * @brief		Set major version.
     *
     * @param[in]	major		Major version.
     */
    void setMajor(quint8 major);

    /**
     * @brief		Get minor version.
     *
     * @return		Minor version.
     */
    quint8 minor() const;

    /**
     * @brief		Set major version.
     *
     * @param[in]	minor		Minor version.
     */
    void setMinor(quint8 minor);

    /**
     * @brief		Get maintenance version.
     *
     * @return		Maintenance version.
     */
    quint8 maintenance() const;

    /**
     * @brief		Set maintenance version.
     *
     * @param[in]	maintenance	Maintenance version.
     */
    void setMaintenance(quint8 maintenance);

    /**
     * @brief		Destructor.
     */
    virtual ~SaveVersion();

    /**
     * @brief		Convert to \c QString.
     */
    operator QString() const;

    /**
     * @brief		Compare if \c *this > version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator>(const SaveVersion &version) const;

    /**
     * @brief		Compare if \c *this >= version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator>=(const SaveVersion &version) const;

    /**
     * @brief		Compare if \c *this < version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator<(const SaveVersion &version) const;

    /**
     * @brief		Compare if \c *this <= version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator<=(const SaveVersion &version) const;

    /**
     * @brief		Compare if \c *this == version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator==(const SaveVersion &version) const;

    /**
     * @brief		Compare if \c *this != version.
     *
     * @param[in]	version		Another version.
     *
     * @return		Compare result.
     */
    bool operator!=(const SaveVersion &version) const;
};
