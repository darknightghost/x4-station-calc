#include <QtCore/QThread>
#include <config.h>
#include <game_data/game_data.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	showStatusFunc	Callback to print status.
 *
 */
GameData::GameData(::std::function<void(QString)> showStatusFunc) :
    QObject(nullptr)
{
    showStatusFunc(STR("STR_CHECKING_GAME_PATH"));
    QThread::sleep(2);
}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path	Path of the game.
 *
 * @return		True if the path of game is available, otherwise returns
 * false.
 *
 */
bool GameData::checkGamePath(const QString &path)
{
    (void)path;
    return true;
}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path	Path of the game.
 *
 * @return		True if the path of game is available, otherwise returns
 * false.
 *
 */
bool GameData::setGamePath(const QString &path)
{
    if (! checkGamePath(path)) {
        return false;
    }
    Config::instance()->setString("/gamePath", path);
    return true;
}

/**
 * @brief Destructor.
 */
GameData::~GameData() {}
