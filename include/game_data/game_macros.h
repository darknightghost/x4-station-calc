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

/**
 * @brief	Macros in game.
 */
class GameMacros :
    public ILoadFactoryFunc<GameMacros,
                            ::std::shared_ptr<GameVFS>,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameMacros,
              ::std::shared_ptr<GameVFS>,
              ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_macros; ///< Macros

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameMacros(::std::shared_ptr<GameVFS>             vfs,
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

  protected:
};

#include <game_data/game_vfs.h>
