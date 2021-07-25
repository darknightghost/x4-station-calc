#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_data.h>
#include <game_data/game_races.h>
#include <locale/string_table.h>

/// Player races.
QSet<QString> GameRaces::_playerRaces = {
    "argon",
    //"boron",
    "paranid", "split", "teladi",
    //"terran"
};

/**
 * @brief		Constructor.
 */
GameRaces::GameRaces(::std::shared_ptr<GameVFS>             vfs,
                     ::std::shared_ptr<GameTexts>           texts,
                     ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_RACES"));
    qDebug() << "Loading races...";

    // Open file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/libraries/races.xml");
    if (file == nullptr) {
        return;
    }
    QByteArray       data = file->readAll();
    QXmlStreamReader reader(data);

    // Parse file

    this->setInitialized();
}

/**
 * @brief	Get race information.
 */
const GameRaces::Race &GameRaces::race(const QString &id)
{
    return m_races[id];
}

/**
 * @brief	Get player races.
 */
const QSet<QString> &GameRaces::playerRaces()
{
    return _playerRaces;
}

/**
 * @brief		Destructor.
 */
GameRaces::~GameRaces() {}
