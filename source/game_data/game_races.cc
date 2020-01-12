#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_races.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	vfs				Virtual filesystem of the game.
 * @param[in]	texts			Game texts.
 * @param[in]	setTextFunc		Callback to set text.
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
    QByteArray       data = file->readAll();
    QXmlStreamReader reader(data);

    // Find entery
    if (! reader.readNextStartElement() || reader.name() != "races") {
        return;
    }

    // Load
    quint64 level = 1;
    while (level > 0 && ! reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();
        switch (type) {
            case QXmlStreamReader::TokenType::StartElement:
                if (level == 1 && reader.name() == "race") {
                    QXmlStreamAttributes attrs = reader.attributes();
                    Race                 race  = {
                        attrs.value("id").toString(),   //< ID.
                        attrs.value("name").toString(), //< Name.
                        attrs.value("description").toString() //< Description.
                    };

                    m_races[race.id] = race;

                    qDebug() << QString("Race \"%1\" loaded, name = \"%2\", "
                                        "description = \"%3\".")
                                    .arg(race.id)
                                    .arg(texts->text(race.name))
                                    .arg(texts->text(race.description))
                                    .toStdString()
                                    .c_str();
                }
                ++level;
                break;

            case QXmlStreamReader::TokenType::EndElement:
                --level;
                break;

            default:
                break;
        }
    }

    this->setGood();
}

/**
 * @brief	Get race information.
 *
 * @return	Information of race.
 */
const GameRaces::Race &GameRaces::race(const QString &id)
{
    return m_races[id];
}

/**
 * @brief		Destructor.
 */
GameRaces::~GameRaces() {}
