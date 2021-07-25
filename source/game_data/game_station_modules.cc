#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QSet>

#include <game_data/game_data.h>
#include <game_data/game_station_modules.h>

QMap<QString, GameStationModules::StationModule::StationModuleClass>
    GameStationModules::_classMap = {
        {
            "buildmodule",
            GameStationModules::StationModule::StationModuleClass::BuildModule,
        },
        {
            "connectionmodule",
            GameStationModules::StationModule::StationModuleClass::
                ConnectionModule,
        },
        {
            "defencemodule",
            GameStationModules::StationModule::StationModuleClass::
                DefenceModule,
        },
        {
            "dockarea",
            GameStationModules::StationModule::StationModuleClass::Dockarea,
        },
        {
            "pier",
            GameStationModules::StationModule::StationModuleClass::Dockarea,
        },
        {
            "habitation",
            GameStationModules::StationModule::StationModuleClass::Habitation,
        },
        {
            "production",
            GameStationModules::StationModule::StationModuleClass::Production,
        },
        {
            "storage",
            GameStationModules::StationModule::StationModuleClass::Storage,
        },
};

/**
 * @brief		Constructor.
 */
GameStationModules::GameStationModules(
    ::std::shared_ptr<GameVFS>             vfs,
    ::std::shared_ptr<GameMacros>          macros,
    ::std::shared_ptr<GameTexts>           texts,
    ::std::shared_ptr<GameWares>           wares,
    ::std::shared_ptr<GameComponents>      components,
    ::std::function<void(const QString &)> setTextFunc)
{
    qDebug() << "Loading station modules...";
    setTextFunc(STR("STR_LOADING_STATION_MODULES"));
    // Open
    ::std::shared_ptr<GameVFS::FileReader> fileReader
        = vfs->open("/libraries/modulegroups.xml");

    // Parse xml
    QXmlStreamReader reader(fileReader->readAll());
    /*
    XMLLoader                             loader;
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameStationModules::onStartElementInRootOfModuleGroups,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    loader["vfs"]        = vfs;
    loader["macros"]     = macros;
    loader["texts"]      = texts;
    loader["wares"]      = wares;
    loader["components"] = components;
    loader.parse(reader, ::std::move(context));
    */

    // Parse extension files
    ::std::shared_ptr<::GameVFS::DirReader> extensionsDir
        = vfs->openDir("/extensions");
    if (extensionsDir != nullptr) {
        for (auto iter = extensionsDir->begin(); iter != extensionsDir->end();
             ++iter) {
            if (iter->type == ::GameVFS::DirReader::EntryType::Directory) {
                fileReader = vfs->open(
                    QString("/extensions/%1/libraries/modulegroups.xml")
                        .arg(iter->name));
                if (fileReader == nullptr) {
                    continue;
                }

                QXmlStreamReader reader(fileReader->readAll());

                // Parse file
                /*
                ::std::unique_ptr<XMLLoader::Context> context
                    = XMLLoader::Context::create();
                context->setOnStartElement(::std::bind(
                    &GameStationModules::onStartElementInRootOfModuleGroups,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
                loader.parse(reader, ::std::move(context));
                */
            }
        }
    }

    m_componentTmpIndex.clear();
    this->setInitialized();
}

/**
 * @brief		Get modules.
 *
 * @return		Modules.
 */
const QVector<::std::shared_ptr<GameStationModules::StationModule>> &
    GameStationModules::modules() const
{
    return m_modules;
}

/**
 * @brief		Get modules.
 *
 * @return		Modules.
 */
::std::shared_ptr<GameStationModules::StationModule>
    GameStationModules::module(const QString &macro)
{
    auto iter = m_modulesIndex.find(macro);
    if (iter != m_modulesIndex.end()) {
        return *iter;
    } else {
        return nullptr;
    }
}

/**
 * @brief		Destructor.
 */
GameStationModules::~GameStationModules() {}
