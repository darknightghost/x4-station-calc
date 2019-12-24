#pragma once

#include <functional>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

#include <design_model/singleton.h>
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
    /**
     * @brief	Information of cat file.
     *
     */
    struct CatFileInfo {
        QString cat;    //< Name of cat file.
        QString catSig; //< Name of sig file of cat file.
        QString dat;    //< Name of dat file.
        QString datSig; //< Name of sig file of dat file.
    };

  private:
    QString                m_gamePath; //< Path of game.
    QMap<int, CatFileInfo> m_catFiles; //< Cat files.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	splash			Splash widget.
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
    bool checkGamePath(const QString &path, QMap<int, CatFileInfo> &catFiles);

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
};
