#pragma once

#include <functional>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QSet>
#include <QtCore/QVector>

#include <game_data/game_components.h>
#include <game_data/game_macros.h>
#include <game_data/game_races.h>
#include <game_data/game_station_modules.h>
#include <game_data/game_texts.h>
#include <game_data/game_vfs.h>
#include <game_data/game_wares.h>
#include <interfaces/i_load_factory_func.h>
#include <interfaces/i_singleton.h>
#include <ui/splash/splash_widget.h>

/// Minimum number of cat files.
#define MIN_CAT_FILE_NUM 9

/**
 * @brief   Game datas.
 */
class GameData : public QObject, public ISingleton<GameData, SplashWidget *> {
    Q_OBJECT
  private:
    SIGNLETON_OBJECT(GameData, SplashWidget *)

  public:
    /**
     * @brief       Game modules.
     */
    struct GameModule {
        QString                id;           ///< Module ID.
        QMap<QString, QString> name;         ///< Module name.
        QMap<QString, QString> description;  ///< Module description.
        QMap<QString, QString> author;       ///< Module author.
        uint32_t               version;      ///< Version.
        QSet<QString>          dependencies; ///< Dependencies.
        QString                path;         ///< Module path.
    };

  private:
    QString                           m_gamePath;   ///< Game path.
    QString                           m_userPath;   ///< User path.
    ::std::shared_ptr<GameVFS>        m_vfs;        ///< Game VFS
    ::std::shared_ptr<GameTexts>      m_texts;      ///< Game texts.
    ::std::shared_ptr<GameMacros>     m_macros;     ///< Game macros.
    ::std::shared_ptr<GameComponents> m_components; ///< Game components.
    ::std::shared_ptr<GameRaces>      m_races;      ///< Game races.
    ::std::shared_ptr<GameWares>      m_wares;      ///< Game wares
    ::std::shared_ptr<GameStationModules>
        m_stationModules; ///< Station modules.

    QMap<QString, ::std::shared_ptr<GameModule>>
                     m_gameModules;     ///< Game modules.
    QVector<QString> m_moduleLoadOrder; ///< Load order of modules.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	splashWidget		Splash widget.
     *
     */
    GameData(SplashWidget *splashWidget);

  public:
    /**
     * @brief		Check path of game.
     *
     * @param[in]	path	Path of the game.
     *
     * @return		True if the path of game is available, otherwise returns
     *				false.
     *
     */
    bool checkGamePath(const QString &path);

    /**
     * @brief		Check path of game.
     *
     * @param[in]	path	Path of the game.
     *
     * @return		True if the path of game is available, otherwise returns
     * false.
     *
     */
    bool setGamePath(const QString &path);

    /**
     * @brief Destructor.
     */
    virtual ~GameData();

  public:
    /*
     * @brief	Get game VFS.
     *
     * @return	Game VFS.
     */
    ::std::shared_ptr<GameVFS> vfs();

    /*
     * @brief	Get game texts.
     *
     * @return	Game texts.
     */
    ::std::shared_ptr<GameTexts> texts();

    /*
     * @brief	Get game macros.
     *
     * @return	Game macros.
     */
    ::std::shared_ptr<GameMacros> macros();

    /*
     * @brief	Get game components.
     *
     * @return	Game components.
     */
    ::std::shared_ptr<GameComponents> components();

    /*
     * @brief	Get game races.
     *
     * @return	Game races.
     */
    ::std::shared_ptr<GameRaces> races();

    /*
     * @brief	Get game wares
     *
     * @return	Game wares
     */
    ::std::shared_ptr<GameWares> wares();

    /*
     * @brief	Get station modules.
     *
     * @return	Station modules.
     */
    ::std::shared_ptr<GameStationModules> stationModules();

  private:
    /**
     * @brief		Check path of game.
     *
     * @param[in]	path		Path of the game.
     * @param[out]	catFiles	Cat files found.
     *
     * @return		True if the path of game is available, otherwise returns
     *				false.
     *
     */
    bool checkGamePath(const QString &                      path,
                       QMap<QString, GameVFS::CatFileInfo> &catFiles);

    /**
     * @brief		Ask game path.
     *
     * @return		True if the path of game is selected, otherwise returns
     *				false.
     */
    bool askGamePath();

    /**
     * @brief       Scan user paths.
     *
     * @param[in]	splashWidget		Splash widget.
     * @param[out]	catFiles	        Cat files found.
     */
    void scanUserPaths(SplashWidget *                       splashWidget,
                       QMap<QString, GameVFS::CatFileInfo> &catFiles);

    /**
     * @brief       Get the blacklist of mods.
     *
     * @return      Blacklist.
     */
    QSet<QString> getModBlacklist();

    /**
     * @brief       Scan game modules.
     */
    void scanGameModules();

    /**
     * @brief       Sort game modules.
     */
    void sortGameModules();
};
