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

class GameVFS;

/**
 * @brief	Races in game.
 */
class GameRaces :
    public ILoadFactoryFunc<GameRaces,
                            ::std::shared_ptr<GameVFS>,
                            ::std::shared_ptr<GameTexts>,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameRaces,
              ::std::shared_ptr<GameVFS>,
              ::std::shared_ptr<GameTexts>,
              ::std::function<void(const QString &)>);

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
    QMap<QString, Race>  m_races;      ///< Macros
    static QSet<QString> _playerRaces; ///< Player races.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	texts			Game texts.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameRaces(::std::shared_ptr<GameVFS>             vfs,
              ::std::shared_ptr<GameTexts>           texts,
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
    static const QSet<QString> &playerRaces();

    /**
     * @brief		Destructor.
     */
    virtual ~GameRaces();

  private:
    /**
     * @brief		Start element callback in root.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRoot(XMLLoader &                   loader,
                              XMLLoader::Context &          context,
                              const QString &               name,
                              const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in races.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRaces(XMLLoader &                   loader,
                               XMLLoader::Context &          context,
                               const QString &               name,
                               const QMap<QString, QString> &attr);
};

#include <game_data/game_vfs.h>
