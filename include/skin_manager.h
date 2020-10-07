#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <interfaces/i_singleton.h>

/**
 * @brief   Skin manager.
 */
class SkinManager : public QObject, public ISingleton<SkinManager> {
    SIGNLETON_OBJECT(SkinManager)
    Q_OBJECT;

  private:
    QMap<QString, QString> m_skins;       ///< Skins.
    QString                m_currentSkin; ///< Current skin.

  private:
    /**
     * @brief       Constructor.
     */
    SkinManager();

  public:
    /**
     * @brief       Get the names of all skins.
     *
     * @return      Names of all skins.
     */
    QVector<QString> skins() const;

    /**
     * @brief       Get the name of  current skin.
     *
     * @return      Name of current skin.
     */
    const QString &currentSkin() const;

    /**
     * @brief       Get the stype sheet of  current skin.
     *
     * @return      Stype sheet of the current skin.
     */
    QString currentSkinStyleSheet() const;

    /**
     * @brief       Set current skin.
     *
     * @param[in]   skin        Name of the skin.
     */
    void setCurrentSkin(const QString &skin);

    /**
     * @brief       Destructor.
     */
    virtual ~SkinManager();

  signals:
    /**
     * @brief       Signal emmited when current skin changed.
     *
     * @param[in]   currentSkin     Name of current skin.
     */
    void skinChanged(const QString &currentSkin);
};
