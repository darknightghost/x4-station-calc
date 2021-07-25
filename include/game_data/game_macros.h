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

class GameVFS;
class GameData;
class XMLLoader;

/**
 * @brief	Macros in game.
 */
class GameMacros :
    public ILoadFactoryFunc<GameMacros,
                            GameData *,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameMacros, GameData *, ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_macros; ///< Macros

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gameData        Game data.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameMacros(GameData *                             gameData,
               ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief	Get macro.
     *
     * @return	Value of macro.
     */
    QString macro(const QString &id);

    /**
     * @brief		Destructor.
     */
    virtual ~GameMacros();

  private:
    /**
     * @brief       Create XML loader.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createXMLLoader();
};
