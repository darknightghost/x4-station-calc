#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <common.h>
#include <game_data/game_texts.h>
#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

class GameData;
class GameText;

/**
 * @brief	Races in game.
 */
class GameRaces :
    public ILoadFactoryFunc<GameRaces,
                            GameData *,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameRaces, GameData *, ::std::function<void(const QString &)>);

  public:
    /**
     * @brief	Information of race.
     */
    struct Race {
        QString           id;          ///< Race ID.
        GameTexts::IDPair name;        ///< Name.
        GameTexts::IDPair description; ///< Description.
    };

  private:
    QMap<QString, Race> m_races;       ///< Macros
    QSet<QString>       m_playerRaces; ///< Player races.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gameData        Game data.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameRaces(GameData *                             gameData,
              ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief	Get race information.
     *
     * @return	Information of race.
     */
    const Race &race(const QString &id);

    /**
     * @brief	Get player races.
     *
     * @return	Player races.
     */
    const QSet<QString> &playerRaces();

    /**
     * @brief	    Add player races.
     *
     * @param[in]	id      Race ID.
     */
    void addPlayerRace(const QString &id);

    /**
     * @brief		Destructor.
     */
    virtual ~GameRaces();

  private:
    /**
     * @brief       Create XML loader.
     *
     * @param[in]	gameData        Game data.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createXMLLoader(GameData *gameData);

    /**
     * @brief       Create XML loader of factions.xml.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createFactionsXMLLoader();
};
