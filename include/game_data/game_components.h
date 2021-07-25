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

class GameData;
class XMLLoader;

/**
 * @brief	Components in game.
 */
class GameComponents :
    public ILoadFactoryFunc<GameComponents,
                            GameData *,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameComponents,
              GameData *,
              ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_components; ///< Components

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gameData        Game data.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameComponents(GameData *                             gameData,
                   ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief	    Get component.
     *
     * @param[in]   name        Name of the macro.
     *
     * @return	    Value of component.
     */
    QString component(const QString &name);

    /**
     * @brief		Destructor.
     */
    virtual ~GameComponents();

  private:
    /**
     * @brief       Create XML loader.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createXMLLoader();
};

#include <game_data/game_vfs.h>
