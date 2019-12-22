#pragma once

#include <functional>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

#include <design_model/singleton.h>

/**
 * @brief   String table.
 */
class GameData :
    public QObject,
    public Singleton<GameData, ::std::function<void(QString)>> {
    Q_OBJECT
    SIGNLETON_OBJECT(GameData, ::std::function<void(QString)>)
  private:
  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	showStatusFunc	Callback to print status.
     *
     */
    GameData(::std::function<void(QString)> showStatusFunc);

  public:
    /**
     * @brief		Check path of game.
     *
     * @param[in]	path	Path of the game.
     *
     * @return		True if the path of game is available, otherwise returns
     * false.
     *
     */
    static bool checkGamePath(const QString &path);

    /**
     * @brief		Check path of game.
     *
     * @param[in]	path	Path of the game.
     *
     * @return		True if the path of game is available, otherwise returns
     * false.
     *
     */
    static bool setGamePath(const QString &path);

  public:
    /**
     * @brief Destructor.
     */
    virtual ~GameData();
};
