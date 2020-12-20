#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QSet>

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
    QXmlStreamReader                      reader(fileReader->readAll());
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
                ::std::unique_ptr<XMLLoader::Context> context
                    = XMLLoader::Context::create();
                context->setOnStartElement(::std::bind(
                    &GameStationModules::onStartElementInRootOfModuleGroups,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
                loader.parse(reader, ::std::move(context));
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

/**
 * @brief		Start element callback in groups.
 */
bool GameStationModules::onStartElementInGroupsOfModuleGroups(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    if (name == "group") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInGroupOfModuleGroups, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4));
    }
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in group.
 */
bool GameStationModules::onStartElementInRootOfModuleGroups(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    if (name == "groups") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInGroupsOfModuleGroups, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4));
    }
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in group.
 */
bool GameStationModules::onStartElementInGroupOfModuleGroups(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &               name,
    const QMap<QString, QString> &attr)
{
    ::std::shared_ptr<GameVFS> vfs
        = ::std::any_cast<::std::shared_ptr<GameVFS>>(loader["vfs"]);
    ::std::shared_ptr<GameMacros> macros
        = ::std::any_cast<::std::shared_ptr<GameMacros>>(loader["macros"]);
    ::std::shared_ptr<GameTexts> texts
        = ::std::any_cast<::std::shared_ptr<GameTexts>>(loader["texts"]);
    ::std::shared_ptr<GameWares> wares
        = ::std::any_cast<::std::shared_ptr<GameWares>>(loader["wares"]);
    ::std::shared_ptr<GameComponents> components
        = ::std::any_cast<::std::shared_ptr<GameComponents>>(
            loader["components"]);
    if (name == "select") {
        auto iter = attr.find("macro");
        if (iter != attr.end()) {
            this->loadMacro(*iter, vfs, macros, texts, wares, components);
        }
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}

/**
 * @brief		Load macro.
 */
void GameStationModules::loadMacro(const QString &                   macro,
                                   ::std::shared_ptr<GameVFS>        vfs,
                                   ::std::shared_ptr<GameMacros>     macros,
                                   ::std::shared_ptr<GameTexts>      texts,
                                   ::std::shared_ptr<GameWares>      wares,
                                   ::std::shared_ptr<GameComponents> components)
{
    if (m_modulesIndex.find(macro) != m_modulesIndex.end()) {
        return;
    }

    qDebug() << "Loading station module macro" << macro << "...";
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open(macros->macro(macro) + ".xml");
    if (file == nullptr) {
        return;
    }

    QXmlStreamReader                      reader(file->readAll());
    XMLLoader                             loader;
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameStationModules::onStartElementInRootOfModuleMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    loader["vfs"]        = vfs;
    loader["macros"]     = macros;
    loader["texts"]      = texts;
    loader["wares"]      = wares;
    loader["components"] = components;
    loader.parse(reader, ::std::move(context));
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInRootOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macros") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInMacrosOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInMacrosOfModuleMacro(
    XMLLoader &                   loader,
    XMLLoader::Context &          currentContext,
    const QString &               name,
    const QMap<QString, QString> &attr)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macro") {
        ::std::shared_ptr<StationModule> module(new StationModule);

        // Initialize properties.
        module->macro           = attr["name"];
        module->playerModule    = false;
        module->hull            = 0;
        module->explosiondamage = 0;
        module->racialLimited   = false;

        // Get module class.
        auto classIter = _classMap.find(attr["class"]);
        if (classIter != _classMap.end()) {
            module->moduleClass = *classIter;

            context->setOnStartElement(::std::bind(
                &GameStationModules::onStartElementInMacroOfModuleMacro, this,
                ::std::placeholders::_1, ::std::placeholders::_2,
                ::std::placeholders::_3, ::std::placeholders::_4, module));
            currentContext.setOnStopElement([this, module](
                                                XMLLoader &loader,
                                                XMLLoader::Context
                                                    &          currentContext,
                                                const QString &name) -> bool {
                if (name == "macro") {
                    if (module->playerModule) {
                        // Add module.
                        m_modulesIndex[module->macro] = module;
                        m_modules.push_back(module);

                        // Print information
                        ::std::shared_ptr<GameTexts> texts
                            = ::std::any_cast<::std::shared_ptr<GameTexts>>(
                                loader["texts"]);
                        qDebug() << "module :{";
                        qDebug() << "    "
                                 << "macro           :" << module->macro;
                        qDebug() << "    "
                                 << "component       :" << module->component;
                        qDebug()
                            << "    "
                            << "name            :" << texts->text(module->name);
                        qDebug() << "    "
                                 << "description     :"
                                 << texts->text(module->description);
                        switch (module->moduleClass) {
                            case StationModule::StationModuleClass::Unknow:
                                qDebug() << "    "
                                         << "class           : "
                                         << "Unknow";
                                break;

                            case StationModule::StationModuleClass::BuildModule:
                                qDebug() << "    "
                                         << "class           : "
                                         << "BuildModule";
                                break;

                            case StationModule::StationModuleClass::
                                ConnectionModule:
                                qDebug() << "    "
                                         << "class           : "
                                         << "ConnectionModule";
                                break;

                            case StationModule::StationModuleClass::
                                DefenceModule:
                                qDebug() << "    "
                                         << "class           : "
                                         << "DefenceModule";
                                break;

                            case StationModule::StationModuleClass::Dockarea:
                                qDebug() << "    "
                                         << "class           : "
                                         << "Dockarea";
                                break;

                            case StationModule::StationModuleClass::Habitation:
                                qDebug() << "    "
                                         << "class           : "
                                         << "Habitation";
                                break;

                            case StationModule::StationModuleClass::Production:
                                qDebug() << "    "
                                         << "class           : "
                                         << "Production";
                                break;

                            case StationModule::StationModuleClass::Storage:
                                qDebug() << "    "
                                         << "class           : "
                                         << "Storage";
                                break;
                        }
                        if (module->racialLimited) {
                            qDebug() << "    "
                                     << "races           : " << module->races;
                        } else {
                            qDebug() << "    "
                                     << "races           : "
                                     << "generic";
                        }
                        qDebug() << "    "
                                 << "hull            : " << module->hull;
                        qDebug()
                            << "    "
                            << "explosiondamage : " << module->explosiondamage;
                        qDebug() << "    "
                                 << "propertues      : {";
                        for (auto &baseProperty : module->properties) {
                            switch (baseProperty->type) {
                                case Property::Type::MTurret: {
                                    ::std::shared_ptr<HasMTurret> property
                                        = ::std::static_pointer_cast<
                                            HasMTurret>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "m turret           : "
                                             << property->count;
                                } break;

                                case Property::Type::MShield: {
                                    ::std::shared_ptr<HasMShield> property
                                        = ::std::static_pointer_cast<
                                            HasMShield>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "m shield           : "
                                             << property->count;
                                } break;

                                case Property::Type::LTurret: {
                                    ::std::shared_ptr<HasLTurret> property
                                        = ::std::static_pointer_cast<
                                            HasLTurret>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "l turret           : "
                                             << property->count;
                                } break;

                                case Property::Type::LShield: {
                                    ::std::shared_ptr<HasLShield> property
                                        = ::std::static_pointer_cast<
                                            HasLShield>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "l shield           : "
                                             << property->count;
                                } break;

                                case Property::Type::SDock: {
                                    ::std::shared_ptr<HasSDock> property
                                        = ::std::static_pointer_cast<HasSDock>(
                                            baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "s docking bay      : "
                                             << property->count;
                                } break;

                                case Property::Type::SShipCargo: {
                                    ::std::shared_ptr<HasSShipCargo> property
                                        = ::std::static_pointer_cast<
                                            HasSShipCargo>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "s ship cargo       : "
                                             << property->capacity;
                                } break;

                                case Property::Type::MDock: {
                                    ::std::shared_ptr<HasMDock> property
                                        = ::std::static_pointer_cast<HasMDock>(
                                            baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "m docking bay      : "
                                             << property->count;
                                } break;

                                case Property::Type::MShipCargo: {
                                    ::std::shared_ptr<HasMShipCargo> property
                                        = ::std::static_pointer_cast<
                                            HasMShipCargo>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "m ship cargo       : "
                                             << property->capacity;
                                } break;

                                case Property::Type::LDock: {
                                    ::std::shared_ptr<HasLDock> property
                                        = ::std::static_pointer_cast<HasLDock>(
                                            baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "l docking bay      : "
                                             << property->count;
                                } break;

                                case Property::Type::XLDock: {
                                    ::std::shared_ptr<HasXLDock> property
                                        = ::std::static_pointer_cast<HasXLDock>(
                                            baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "xl docking bay     : "
                                             << property->count;
                                } break;

                                case Property::Type::LXLDock: {
                                    ::std::shared_ptr<HasLXLDock> property
                                        = ::std::static_pointer_cast<
                                            HasLXLDock>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "l/xl docking bay   : "
                                             << property->count;
                                } break;

                                case Property::Type::SLaunchTube: {
                                    ::std::shared_ptr<HasSLaunchTube> property
                                        = ::std::static_pointer_cast<
                                            HasSLaunchTube>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "s launch tube      : "
                                             << property->count;
                                } break;

                                case Property::Type::MLaunchTube: {
                                    ::std::shared_ptr<HasMLaunchTube> property
                                        = ::std::static_pointer_cast<
                                            HasMLaunchTube>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "m launch tube      : "
                                             << property->count;
                                } break;

                                case Property::Type::SupplyWorkforce: {
                                    ::std::shared_ptr<SupplyWorkforce> property
                                        = ::std::static_pointer_cast<
                                            SupplyWorkforce>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "supply workforce   : {";
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "workforce : "
                                             << property->workforce;
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "supplies  : [";
                                    for (auto &resource :
                                         property->supplyInfo->resources) {
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "{";
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "id     : " << resource->id;
                                        qDebug()
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "amount : "
                                            << ::round((
                                                   double)(((long double)
                                                                resource
                                                                    ->amount)
                                                           * property->workforce
                                                           * 3600
                                                           / property
                                                                 ->supplyInfo
                                                                 ->amount
                                                           / property
                                                                 ->supplyInfo
                                                                 ->time))
                                            << "/h";
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "}";
                                    }
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "]";
                                    qDebug() << "    "
                                             << "    "
                                             << "}";
                                } break;

                                case Property::Type::RequireWorkforce: {
                                    ::std::shared_ptr<RequireWorkforce> property
                                        = ::std::static_pointer_cast<
                                            RequireWorkforce>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "workforce required : "
                                             << property->workforce;
                                } break;

                                case Property::Type::SupplyProduct: {
                                    ::std::shared_ptr<SupplyProduct> property
                                        = ::std::static_pointer_cast<
                                            SupplyProduct>(baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "product            : {";
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "id               :"
                                             << property->product;
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "time per round   :"
                                             << property->productionInfo->time
                                             << "s";
                                    qDebug()
                                        << "    "
                                        << "    "
                                        << "    "
                                        << "amount per round :"
                                        << property->productionInfo->amount
                                        << " - "
                                        << ::round(
                                               property->productionInfo->amount
                                               * (property->productionInfo
                                                      ->workEffect
                                                  + 1.0));
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "resources        : [";
                                    for (auto &resource :
                                         property->productionInfo->resources) {
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "{";
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "id     : " << resource->id;
                                        qDebug()
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "    "
                                            << "amount : "
                                            << ::round((
                                                   double)(((long double)
                                                                resource
                                                                    ->amount)
                                                           * 3600
                                                           / property
                                                                 ->productionInfo
                                                                 ->time))
                                            << "/h - "
                                            << ::round(
                                                   (double)(((long double)
                                                                 resource
                                                                     ->amount)
                                                            * 3600
                                                            / property
                                                                  ->productionInfo
                                                                  ->time)
                                                   * (1.0
                                                      + property->productionInfo
                                                            ->workEffect))
                                            << "/h";
                                        qDebug() << "    "
                                                 << "    "
                                                 << "    "
                                                 << "    "
                                                 << "}";
                                    }
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "]";
                                    qDebug() << "    "
                                             << "    "
                                             << "}";
                                } break;

                                case Property::Type::Cargo: {
                                    ::std::shared_ptr<HasCargo> property
                                        = ::std::static_pointer_cast<HasCargo>(
                                            baseProperty);
                                    qDebug() << "    "
                                             << "    "
                                             << "cargo              : {";
                                    switch (property->cargoType) {
                                        case GameWares::TransportType::
                                            Container:
                                            qDebug() << "    "
                                                     << "    "
                                                     << "    "
                                                     << "type : Container";
                                            break;

                                        case GameWares::TransportType::Solid:
                                            qDebug() << "    "
                                                     << "    "
                                                     << "    "
                                                     << "type : Solid";
                                            break;

                                        case GameWares::TransportType::Liquid:
                                            qDebug() << "    "
                                                     << "    "
                                                     << "    "
                                                     << "type : Liquid";
                                            break;

                                        case GameWares::TransportType::Unknow:
                                            qDebug() << "    "
                                                     << "    "
                                                     << "    "
                                                     << "type : Unknow";
                                            break;
                                    }
                                    qDebug() << "    "
                                             << "    "
                                             << "    "
                                             << "size : " << property->cargoSize
                                             << " m^3";
                                    qDebug() << "    "
                                             << "    "
                                             << "}";
                                } break;
                            }
                        }
                        qDebug() << "    "
                                 << "}";
                        qDebug() << "}";
                    }
                    currentContext.setOnStopElement(nullptr);
                }
                return true;
            });
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInMacroOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    // Get environemnt.
    ::std::shared_ptr<GameVFS> vfs
        = ::std::any_cast<::std::shared_ptr<GameVFS>>(loader["vfs"]);
    ::std::shared_ptr<GameMacros> macros
        = ::std::any_cast<::std::shared_ptr<GameMacros>>(loader["macros"]);
    ::std::shared_ptr<GameTexts> texts
        = ::std::any_cast<::std::shared_ptr<GameTexts>>(loader["texts"]);
    ::std::shared_ptr<GameWares> wares
        = ::std::any_cast<::std::shared_ptr<GameWares>>(loader["wares"]);
    ::std::shared_ptr<GameComponents> components
        = ::std::any_cast<::std::shared_ptr<GameComponents>>(
            loader["components"]);

    // Parse node.
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "component") {
        module->component = attr["ref"];
        auto iter         = m_componentTmpIndex.find(attr["ref"]);
        if (iter == m_componentTmpIndex.end()) {
            m_componentTmpIndex[module->component] = {module};

        } else {
            m_componentTmpIndex[module->component].push_back(module);
        }

        this->loadComponent(attr["ref"], module, vfs, macros, texts, wares,
                            components);

    } else if (name == "properties") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInPropertiesOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));

    } else if (name == "connections") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInConnectionsOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInPropertiesOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "identification") {
        module->name        = GameTexts::IDPair(attr["name"]);
        module->description = GameTexts::IDPair(attr["description"]);
        auto iter           = attr.find("makerrace");
        if (iter == attr.end()) {
            module->races         = GameRaces::playerRaces();
            module->racialLimited = false;
        } else {
            module->races         = {*iter};
            module->racialLimited = true;
        }
        for (auto &otherModule : m_componentTmpIndex[module->component]) {
            if (otherModule == module) {
                continue;
            }
            if (module->racialLimited && (! otherModule->racialLimited)) {
                for (auto &race : module->races) {
                    auto iter = otherModule->races.find(race);
                    if (iter != otherModule->races.end()) {
                        otherModule->races.erase(iter);
                    }
                }
            }
            if ((! module->racialLimited) && otherModule->racialLimited) {
                for (auto &race : otherModule->races) {
                    auto iter = module->races.find(race);
                    if (iter != module->races.end()) {
                        module->races.erase(iter);
                    }
                }
            }
        }

    } else if (name == "build") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInBuildOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));

    } else if (name == "hull") {
        module->hull = attr["max"].toUInt();

    } else if (name == "explosiondamage") {
        module->explosiondamage = attr["value"].toUInt();

    } else if (name == "workforce") {
        if (attr.find("max") == attr.end()) {
            // Supply.
            ::std::shared_ptr<SupplyWorkforce> property;
            auto                               iter
                = module->properties.find(Property::Type::SupplyWorkforce);
            if (iter == module->properties.end()) {
                property
                    = ::std::shared_ptr<SupplyWorkforce>(new SupplyWorkforce);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<SupplyWorkforce>(*iter);
            }
            property->workforce = attr["capacity"].toUInt();

            /// Supply
            ::std::shared_ptr<GameWares> wares
                = ::std::any_cast<::std::shared_ptr<GameWares>>(
                    loader["wares"]);
            const ::std::shared_ptr<::GameWares::Ware> workunit
                = wares->ware("workunit_busy");
            for (auto &info : workunit->productionInfos) {
                if (info->method == attr["race"] || info->method == "default") {
                    ::std::shared_ptr<::GameWares::ProductionInfo> supplyInfo(
                        new ::GameWares::ProductionInfo());
                    supplyInfo->id         = "";
                    supplyInfo->time       = info->time;
                    supplyInfo->amount     = attr["capacity"].toULong();
                    supplyInfo->method     = attr["race"];
                    supplyInfo->workEffect = 0;
                    for (auto &res : info->resources) {
                        ::std::shared_ptr<GameWares::Resource> resource(
                            new GameWares::Resource);
                        resource->id     = res->id;
                        resource->amount = (quint32)::round(
                            ((long double)res->amount) * supplyInfo->amount
                            / info->amount);
                        supplyInfo->resources[resource->id] = resource;
                    }

                    property->supplyInfo = supplyInfo;
                    if (info->method == attr["race"]) {
                        break;
                    }
                }
            }
            if (property->supplyInfo == nullptr) {
                module->properties.remove(Property::Type::SupplyWorkforce);
            }

        } else {
            // Require.
            ::std::shared_ptr<RequireWorkforce> property;
            auto                                iter
                = module->properties.find(Property::Type::RequireWorkforce);
            if (iter == module->properties.end()) {
                property
                    = ::std::shared_ptr<RequireWorkforce>(new RequireWorkforce);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<RequireWorkforce>(*iter);
            }

            property->workforce = attr["max"].toUInt();
        }

    } else if (name == "production") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInProductionOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));

    } else if (name == "cargo") {
        ::std::shared_ptr<HasCargo> property;
        auto iter = module->properties.find(Property::Type::Cargo);
        if (iter == module->properties.end()) {
            property = ::std::shared_ptr<HasCargo>(new HasCargo);
            module->properties[property->type] = property;

        } else {
            property = ::std::static_pointer_cast<HasCargo>(*iter);
        }

        auto tags
            = attr["tags"].split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
        for (QString tag : tags) {
            if (tag == "container") {
                property->cargoType = GameWares::TransportType::Container;

            } else if (tag == "solid") {
                property->cargoType = GameWares::TransportType::Solid;

            } else if (tag == "liquid") {
                property->cargoType = GameWares::TransportType::Liquid;
            }
        }

        if (property->cargoType == GameWares::TransportType::Unknow) {
            module->properties.remove(Property::Type::Cargo);
        }

        property->cargoSize = attr["max"].toUInt();
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInBuildOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "sets") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInSetsOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInSetsOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "set") {
        if (attr["ref"] == "headquarters_player") {
            module->playerModule = true;
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInProductionOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "queue") {
        ::std::shared_ptr<GameWares> wares
            = ::std::any_cast<::std::shared_ptr<GameWares>>(loader["wares"]);

        QString method = "default";
        auto    iter   = attr.find("method");
        if (iter != attr.end()) {
            method = *iter;
        }
        ::std::shared_ptr<GameWares::Ware> ware = wares->ware(attr["ware"]);

        auto productionInfoIter = ware->productionInfos.find(method);
        if (productionInfoIter == ware->productionInfos.end()) {
            productionInfoIter = ware->productionInfos.find("default");
        }

        if (productionInfoIter != ware->productionInfos.end()) {
            ::std::shared_ptr<SupplyProduct>             property;
            ::std::shared_ptr<GameWares::ProductionInfo> productionInfo
                = *productionInfoIter;
            auto iter = module->properties.find(Property::Type::SupplyProduct);
            if (iter == module->properties.end()) {
                property = ::std::shared_ptr<SupplyProduct>(new SupplyProduct);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<SupplyProduct>(*iter);
            }

            property->product        = attr["ware"];
            property->productionInfo = productionInfo;
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInConnectionsOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "connection") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInConnectionOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module macro.
 */
bool GameStationModules::onStartElementInConnectionOfModuleMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macro") {
        QString reference = attr["ref"];
        QRegExp sTubeExp("launchtube_\\w+_s_\\w+_macro");
        sTubeExp.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
        QRegExp mTubeExp("launchtube_\\w+_m_\\w+_macro");
        mTubeExp.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
        if (sTubeExp.exactMatch(reference)) {
            ::std::shared_ptr<HasSLaunchTube> property;
            auto iter = module->properties.find(Property::Type::SLaunchTube);
            if (iter == module->properties.end()) {
                property
                    = ::std::shared_ptr<HasSLaunchTube>(new HasSLaunchTube);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<HasSLaunchTube>(*iter);
            }

            ++(property->count);

        } else if (mTubeExp.exactMatch(reference)) {
            ::std::shared_ptr<HasMLaunchTube> property;
            auto iter = module->properties.find(Property::Type::MLaunchTube);
            if (iter == module->properties.end()) {
                property
                    = ::std::shared_ptr<HasMLaunchTube>(new HasMLaunchTube);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<HasMLaunchTube>(*iter);
            }

            ++(property->count);

        } else {
            ::std::shared_ptr<GameVFS> vfs
                = ::std::any_cast<::std::shared_ptr<GameVFS>>(loader["vfs"]);
            ::std::shared_ptr<GameMacros> macros
                = ::std::any_cast<::std::shared_ptr<GameMacros>>(
                    loader["macros"]);
            ::std::shared_ptr<GameTexts> texts
                = ::std::any_cast<::std::shared_ptr<GameTexts>>(
                    loader["texts"]);
            ::std::shared_ptr<GameWares> wares
                = ::std::any_cast<::std::shared_ptr<GameWares>>(
                    loader["wares"]);
            ::std::shared_ptr<GameComponents> components
                = ::std::any_cast<::std::shared_ptr<GameComponents>>(
                    loader["components"]);
            this->loadConnectionMacro(reference, module, vfs, macros, texts,
                                      wares, components);
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Load macro in connections of module macro.
 */
void GameStationModules::loadConnectionMacro(
    const QString &                   macro,
    ::std::shared_ptr<StationModule>  module,
    ::std::shared_ptr<GameVFS>        vfs,
    ::std::shared_ptr<GameMacros>     macros,
    ::std::shared_ptr<GameTexts>      texts,
    ::std::shared_ptr<GameWares>      wares,
    ::std::shared_ptr<GameComponents> components)
{
    qDebug() << "Loading macro in connection" << macro << "...";
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open(macros->macro(macro) + ".xml");
    if (file == nullptr) {
        return;
    }

    QXmlStreamReader                      reader(file->readAll());
    XMLLoader                             loader;
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameStationModules::onStartElementInRootOfConnectionMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
    loader["vfs"]        = vfs;
    loader["macros"]     = macros;
    loader["texts"]      = texts;
    loader["wares"]      = wares;
    loader["components"] = components;
    loader.parse(reader, ::std::move(context));
}

/**
 * @brief		Start element callback in connection macro.
 */
bool GameStationModules::onStartElementInRootOfConnectionMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macros") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInMacrosOfConnectionMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in connection macro.
 */
bool GameStationModules::onStartElementInMacrosOfConnectionMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macro") {
        auto iter = attr.find("class");

        if (iter != attr.end() && *iter == "dockingbay") {
            context->setOnStartElement(::std::bind(
                &GameStationModules::onStartElementInMacroOfConnectionMacro,
                this, ::std::placeholders::_1, ::std::placeholders::_2,
                ::std::placeholders::_3, ::std::placeholders::_4, module));
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in connection macro.
 */
bool GameStationModules::onStartElementInMacroOfConnectionMacro(
    XMLLoader &         loader,
    XMLLoader::Context &currentContext,
    const QString &     name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "properties") {
        ::std::shared_ptr<TmpDockingBayInfo> info
            = ::std::shared_ptr<TmpDockingBayInfo>(
                new TmpDockingBayInfo({0, 0, TmpDockingBayInfo::S}));
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInnPropertiesOfConnectionMacro,
            this, ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module, info));

        currentContext.setOnStopElement(::std::bind(
            [](XMLLoader &, XMLLoader::Context &, const QString &name,
               ::std::shared_ptr<StationModule>     module,
               ::std::shared_ptr<TmpDockingBayInfo> info) -> bool {
                if (name == "properties") {
                    switch (info->type) {
                        case TmpDockingBayInfo::S:
                            if (info->count > 0) {
                                // S docking bay.
                                ::std::shared_ptr<HasSDock> property;
                                auto iter = module->properties.find(
                                    Property::Type::SDock);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasSDock>(
                                        new HasSDock);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property
                                        = ::std::static_pointer_cast<HasSDock>(
                                            *iter);
                                }
                                property->count += info->count;
                            }
                            if (info->capacity > 0) {
                                // S ship cargo.
                                ::std::shared_ptr<HasSShipCargo> property;
                                auto iter = module->properties.find(
                                    Property::Type::SShipCargo);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasSShipCargo>(
                                        new HasSShipCargo);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property = ::std::static_pointer_cast<
                                        HasSShipCargo>(*iter);
                                }
                                property->capacity += info->capacity;
                            }
                            break;

                        case TmpDockingBayInfo::M:
                            if (info->count > 0) {
                                // M docking bay.
                                ::std::shared_ptr<HasMDock> property;
                                auto iter = module->properties.find(
                                    Property::Type::MDock);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasMDock>(
                                        new HasMDock);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property
                                        = ::std::static_pointer_cast<HasMDock>(
                                            *iter);
                                }
                                property->count += info->count;
                            }
                            if (info->capacity > 0) {
                                // M ship cargo.
                                ::std::shared_ptr<HasMShipCargo> property;
                                auto iter = module->properties.find(
                                    Property::Type::MShipCargo);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasMShipCargo>(
                                        new HasMShipCargo);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property = ::std::static_pointer_cast<
                                        HasMShipCargo>(*iter);
                                }
                                property->capacity += info->capacity;
                            }
                            break;

                        case TmpDockingBayInfo::L:
                            if (info->count > 0) {
                                // L docking bay.
                                ::std::shared_ptr<HasLDock> property;
                                auto iter = module->properties.find(
                                    Property::Type::LDock);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasLDock>(
                                        new HasLDock);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property
                                        = ::std::static_pointer_cast<HasLDock>(
                                            *iter);
                                }
                                property->count += info->count;
                            }
                            break;

                        case TmpDockingBayInfo::XL:
                            if (info->count > 0) {
                                // XL docking bay.
                                ::std::shared_ptr<HasXLDock> property;
                                auto iter = module->properties.find(
                                    Property::Type::XLDock);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasXLDock>(
                                        new HasXLDock);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property
                                        = ::std::static_pointer_cast<HasXLDock>(
                                            *iter);
                                }
                                property->count += info->count;
                            }
                            break;

                        case TmpDockingBayInfo::L_XL:
                            if (info->count > 0) {
                                // L/XL docking bay.
                                ::std::shared_ptr<HasLXLDock> property;
                                auto iter = module->properties.find(
                                    Property::Type::LXLDock);
                                if (iter == module->properties.end()) {
                                    property = ::std::shared_ptr<HasLXLDock>(
                                        new HasLXLDock);
                                    module->properties[property->type]
                                        = property;
                                } else {
                                    property = ::std::static_pointer_cast<
                                        HasLXLDock>(*iter);
                                }
                                property->count += info->count;
                            }
                            break;
                    }
                }
                return true;
            },
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, module, info));
    }

    loader.pushContext(::std::move(context));

    return true;
}

/**
 * @brief		Start element callback in connection macro.
 */
bool GameStationModules::onStartElementInnPropertiesOfConnectionMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &               name,
    const QMap<QString, QString> &attr,
    ::std::shared_ptr<StationModule>,
    ::std::shared_ptr<TmpDockingBayInfo> info)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    if (name == "dock") {
        auto iter = attr.find("external");
        if (iter != attr.end()) {
            info->count = iter->toUInt();
        }

        iter = attr.find("capacity");
        if (iter != attr.end()) {
            info->capacity = iter->toUInt();
        }
    } else if (name == "docksize") {
        QSet<QString> tags;
        for (auto &tag :
             attr["tags"].split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts)) {
            tags.insert(tag);
        }
        if (tags.find("dock_s") != tags.end()) {
            info->type = TmpDockingBayInfo::S;

        } else if (tags.find("dock_m") != tags.end()) {
            info->type = TmpDockingBayInfo::M;

        } else if (tags.find("dock_l") != tags.end()) {
            info->type = TmpDockingBayInfo::L;

            if (tags.find("dock_xl") != tags.end()) {
                info->type = TmpDockingBayInfo::L_XL;

            } else {
                info->type = TmpDockingBayInfo::L;
            }
        } else {
            info->type = TmpDockingBayInfo::XL;
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Load component.
 */
void GameStationModules::loadComponent(
    const QString &                   component,
    ::std::shared_ptr<StationModule>  module,
    ::std::shared_ptr<GameVFS>        vfs,
    ::std::shared_ptr<GameMacros>     macros,
    ::std::shared_ptr<GameTexts>      texts,
    ::std::shared_ptr<GameWares>      wares,
    ::std::shared_ptr<GameComponents> components)
{
    qDebug() << "Loading component" << component << "...";
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open(components->component(component) + ".xml");
    if (file == nullptr) {
        return;
    }

    QXmlStreamReader                      reader(file->readAll());
    XMLLoader                             loader;
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameStationModules::onStartElementInRootOfModuleComponent,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
    loader["vfs"]        = vfs;
    loader["macros"]     = macros;
    loader["texts"]      = texts;
    loader["wares"]      = wares;
    loader["components"] = components;
    loader.parse(reader, ::std::move(context));
}

/**
 * @brief		Start element callback in module component.
 */
bool GameStationModules::onStartElementInRootOfModuleComponent(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "components") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInComponentsOfModuleComponent,
            this, ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module component.
 */
bool GameStationModules::onStartElementInComponentsOfModuleComponent(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "component") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInComponentOfModuleComponent,
            this, ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module component.
 */
bool GameStationModules::onStartElementInComponentOfModuleComponent(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "properties") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInPropertiesOfModuleMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    } else if (name == "connections") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInConnectionsOfModuleComponent,
            this, ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in module component.
 */
bool GameStationModules::onStartElementInConnectionsOfModuleComponent(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "connection") {
        auto iter = attr.find("tags");
        if (iter != attr.end()) {
            auto tags
                = iter->split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
            bool foundTurret = false;
            bool foundShield = false;
            bool foundMedium = false;
            bool foundLarge  = false;
            for (auto &tag : tags) {
                if (tag == "turret") {
                    foundTurret = true;

                } else if (tag == "shield") {
                    foundShield = true;

                } else if (tag == "medium") {
                    foundMedium = true;

                } else if (tag == "large") {
                    foundLarge = true;
                }
            }
            if (foundTurret) {
                if (foundMedium) {
                    // M turret.
                    ::std::shared_ptr<HasMTurret> property;
                    auto                          iter
                        = module->properties.find(Property::Type::MTurret);
                    if (iter == module->properties.end()) {
                        property
                            = ::std::shared_ptr<HasMTurret>(new HasMTurret);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasMTurret>(*iter);
                    }
                    ++(property->count);

                } else if (foundLarge) {
                    // L turret.
                    ::std::shared_ptr<HasLTurret> property;
                    auto                          iter
                        = module->properties.find(Property::Type::LTurret);
                    if (iter == module->properties.end()) {
                        property
                            = ::std::shared_ptr<HasLTurret>(new HasLTurret);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasLTurret>(*iter);
                    }
                    ++(property->count);
                }

            } else if (foundLarge) {
                if (foundMedium) {
                    // M shield.
                    ::std::shared_ptr<HasMShield> property;
                    auto                          iter
                        = module->properties.find(Property::Type::MShield);
                    if (iter == module->properties.end()) {
                        property
                            = ::std::shared_ptr<HasMShield>(new HasMShield);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasMShield>(*iter);
                    }
                    ++(property->count);

                } else if (foundLarge) {
                    // L shield.
                    ::std::shared_ptr<HasLShield> property;
                    auto                          iter
                        = module->properties.find(Property::Type::LShield);
                    if (iter == module->properties.end()) {
                        property
                            = ::std::shared_ptr<HasLShield>(new HasLShield);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasLShield>(*iter);
                    }
                    ++(property->count);
                }
            }
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}
