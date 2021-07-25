#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_data.h>
#include <game_data/game_races.h>
#include <game_data/xml_loader/xml_loader.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameRaces::GameRaces(GameData *                             gameData,
                     ::std::function<void(const QString &)> setTextFunc) :
    m_playerRaces({"argon"})
{
    auto vfs = gameData->vfs();
    setTextFunc(STR("STR_LOADING_RACES"));
    qDebug() << "Loading races...";

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles("libraries/races\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"libraries/races.xml\".";
        return;
    }

    auto                 xmlLoader = this->createXMLLoader(gameData);
    XMLLoader::ErrorInfo errorInfo;
    auto                 iter = xmlFiles.begin()->begin();
    qDebug() << "Loading data file" << (*iter) << ".";
    if (! xmlLoader->loadData(gameData->vfs()->open(*iter)->readAll(),
                              errorInfo)) {
        qWarning() << QString(
                          "Failed to load file \"%1\", line: %2, col: %3 : %4.")
                          .arg(*iter)
                          .arg(errorInfo.errorLine)
                          .arg(errorInfo.errorColumn)
                          .arg(errorInfo.errorMsg)
                          .toStdString()
                          .c_str();
        return;
    }
    for (++iter; iter != xmlFiles.begin()->end(); ++iter) {
        qDebug() << "Loading patch file" << (*iter) << ".";
        if (! xmlLoader->loadPatch(gameData->vfs()->open(*iter)->readAll(),
                                   errorInfo)) {
            qWarning()
                << QString(
                       "Failed to patch file \"%1\", line: %2, col: %3 : %4.")
                       .arg(*iter)
                       .arg(errorInfo.errorLine)
                       .arg(errorInfo.errorColumn)
                       .arg(errorInfo.errorMsg)
                       .toStdString()
                       .c_str();
        }
    }
    if (! xmlLoader->parse()) {
        qWarning() << QString("Failed to parse file \"%1\".")
                          .arg(xmlFiles.begin().key())
                          .toStdString()
                          .c_str();
        return;
    }

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
    return m_playerRaces;
}

/**
 * @brief	    Add player races.
 */
void GameRaces::addPlayerRace(const QString &id)
{
    if (m_races.find(id) != m_races.end()) {
        m_playerRaces.insert(id);
    }
}

/**
 * @brief		Destructor.
 */
GameRaces::~GameRaces() {}

/**
 * @brief       Create XML loader.
 */
::std::unique_ptr<XMLLoader> GameRaces::createXMLLoader(GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /races/race
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/races/race");
    elementLoader->setOnStartElement([this, gameData](
                                         XMLLoader &,
                                         XMLLoader::XMLElementLoader &,
                                         const ::std::map<QString, QString>
                                             &attributes) -> bool {
        auto gameTexts = gameData->texts();

        // ID.
        auto iter = attributes.find("id");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"id\" in element \"/index/entry\".";
        }
        QString id = iter->second;

        // Name.
        iter = attributes.find("name");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"name\" in element \"/index/entry\".";
        }
        QString name = iter->second;

        // Description.
        iter = attributes.find("description");
        if (iter == attributes.end()) {
            qWarning() << "Missing attribute \"description\" in element "
                          "\"/index/entry\".";
        }
        QString description = iter->second;

        Race race = {
            id,          //< ID.
            name,        //< Name.
            description, //< Description.
        };

        m_races[race.id] = race;

        qDebug() << QString("Race \"%1\" loaded, name = \"%2\", "
                            "description = \"%3\".")
                        .arg(race.id)
                        .arg(gameTexts->text(race.name))
                        .arg(gameTexts->text(race.description))
                        .toStdString()
                        .c_str();

        return true;
    });

    return ret;
}
