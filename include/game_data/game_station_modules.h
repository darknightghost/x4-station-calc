#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <common.h>
#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

#include <game_data/game_vfs.h>

#include <game_data/game_components.h>
#include <game_data/game_races.h>
#include <game_data/game_texts.h>
#include <game_data/game_wares.h>

class GameVFS;

class GameComponents;
class GameRaces;
class GameTexts;
class GameWares;

/**
 * @brief	Macros in game.
 */
class GameStationModules :
    public ILoadFactoryFunc<GameStationModules(
        ::std::shared_ptr<GameVFS>,
        ::std::shared_ptr<GameComponents>,
        ::std::shared_ptr<GameRaces>,
        ::std::shared_ptr<GameTexts>,
        ::std::shared_ptr<GameWares>,
        ::std::function<void(const QString &)>)> {
    LOAD_FUNC(GameStationModules,
              ::std::shared_ptr<GameVFS>,
              ::std::shared_ptr<GameComponents>,
              ::std::shared_ptr<GameRaces>,
              ::std::shared_ptr<GameTexts>,
              ::std::shared_ptr<GameWares>,
              ::std::function<void(const QString &)>);

  public:
    /**
     * @brief	Class of station module.
     */
    enum StationModuleClass {
        BuildModule,      ///< Build module.
        ConnectionModule, ///< Connection module.
        DefenceModule,    ///< Defence module.
        Dockarea,         ///< Dockarea.
        Habitation,       ///< Habitation,
        Production,       ///< Production,
        Storage           ///< Storage.
    };

    /**
     * @brief	Information of station module.
     */
    struct StationModule {
        QString            macro;       ///< Macro of the
        GameTexts::IDPair  name;        ///< Module name.
        StationModuleClass MuduleClass; ///< Class of station module.
        GameTexts::IDPair  description; ///< Module desctiptio.
        QVector<QString>   races;       ///< Module races.
        quint32            hull;        ///< Hull.
        quint32            workforce;   ///< Workforce.
        quint32            sShield;     ///< Count of S shield.
        quint32            sTurret;     ///< Count of S turret.
        quint32            mShield;     ///< Count of M shield.
        quint32            mTurret;     ///< Count of M turret.
        quint32            lShield;     ///< Count of L shield.
        quint32            lTurret;     ///< Count of L turret.
    };

  private:
    QMap<QString,
         ::std::shared_ptr<StationModule>>
        m_modules; ///< Station modules.

    QMap<QString,
         ::std::shared_ptr<StationModule>>
        m_raceIndex; ///< Station modules.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	components		Game components.
     * @param[in]	races			Game races.
     * @param[in]	texts			Game texts.
     * @param[in]	wares			Game wares.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameStationModules(::std::shared_ptr<GameVFS>             vfs,
                       ::std::shared_ptr<GameComponents>      components,
                       ::std::shared_ptr<GameRaces>           races,
                       ::std::shared_ptr<GameTexts>           texts,
                       ::std::shared_ptr<GameWares>           wares,
                       ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief		Destructor.
     */
    virtual ~GameStationModules();

  private:
    /**
     * @brief		Start element callback in root.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	texts		Game texts.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRoot(XMLLoader &                   loader,
                              XMLLoader::Context &          context,
                              const QString &               name,
                              const QMap<QString, QString> &attr,
                              ::std::shared_ptr<GameTexts>  texts);

    /**
     * @brief		Start element callback in races.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	texts		Game texts.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRaces(XMLLoader &                   loader,
                               XMLLoader::Context &          context,
                               const QString &               name,
                               const QMap<QString, QString> &attr,
                               ::std::shared_ptr<GameTexts>  texts);
};
