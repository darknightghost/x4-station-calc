#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <interface/i_load_factory_func.h>
#include <locale/string_table.h>

class GameVFS;

/**
 * @brief	Macros in game.
 */
class GameMacro :
    public ILoadFactoryFunc<GameMacro(::std::shared_ptr<GameVFS>,
                                      ::std::function<void(const QString &)>)> {
    LOAD_FUNC(GameMacro,
              ::std::shared_ptr<GameVFS>,
              ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_macros; //< Macros

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameMacro(::std::shared_ptr<GameVFS>             vfs,
              ::std::function<void(const QString &)> setTextFunc);

    /**
     * @brief	Get macro.
     *
     * @return	Value of macro.
     */
    QString macro(const QString &id);

  public:
    /**
     * @brief		Destructor.
     */
    virtual ~GameMacro();
};

#include <game_data/game_vfs.h>
