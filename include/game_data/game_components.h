#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

class GameVFS;

/**
 * @brief	Components in game.
 */
class GameComponents :
    public ILoadFactoryFunc<GameComponents(
        ::std::shared_ptr<GameVFS>, ::std::function<void(const QString &)>)> {
    LOAD_FUNC(GameComponents,
              ::std::shared_ptr<GameVFS>,
              ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_components; //< Components

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameComponents(::std::shared_ptr<GameVFS>             vfs,
                   ::std::function<void(const QString &)> setTextFunc);

    /**
     * @brief	Get component.
     *
     * @return	Value of component.
     */
    QString component(const QString &id);

  public:
    /**
     * @brief		Destructor.
     */
    virtual ~GameComponents();
};

#include <game_data/game_vfs.h>
