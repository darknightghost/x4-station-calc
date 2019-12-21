#include <config.h>
#include <game_data/game_data.h>
#include <locale/string_table.h>

/**
 * @brief   Constructor.
 */
GameData::GameData() {}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path	Path of the game.
 *
 * @return		True if the path of game is available, otherwise returns
 * false.
 *
 */
bool GameData::checkGamePath(const QString &path) {}

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
