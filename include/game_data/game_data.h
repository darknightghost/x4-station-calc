#pragma once

#include <functional>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

#include <design_model/singleton.h>
#include <game_data/game_text.h>
#include <game_data/game_vfs.h>
#include <ui/splash/splash_widget.h>

/// Minimum number of cat files.
#define MIN_CAT_FILE_NUM 9

/**
 * @brief   String table.
 */
class GameData : public QObject, public Singleton<GameData, SplashWidget *> {
    Q_OBJECT
  private:
    SIGNLETON_OBJECT(GameData, SplashWidget *)

  private:
    QString                     m_gamePath; //< Game path.
    ::std::shared_ptr<GameVFS>  m_vfs;      //< Game VFS
    ::std::shared_ptr<GameText> m_texts;    //< Game texts.

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
    bool checkGamePath(const QString &                  path,
                       QMap<int, GameVFS::CatFileInfo> &catFiles);

    /**
     * @brief		Ask game path.
     *
     * @return		True if the path of game is selected, otherwise returns
     *				false.
     */
    bool askGamePath();

    /**
     * @brief		Get number from string.
     *
     * @param[in]	str		String to get number.
     *
     * @return		Number got.
     */
    int getNumberFromStr(const QString &str);

  signals:
    /**
     * @brief	Game data reloaded.
     */
    void dataReloaded();
};
