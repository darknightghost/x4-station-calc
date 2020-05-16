#include <cmath>

#include <QtCore/QDebug>

#include <game_data/game_station_modules.h>

/**
 * @brief		Constructor.
 */
GameStationModules::GameStationModules(
    ::std::shared_ptr<GameVFS>             vfs,
    ::std::shared_ptr<GameMacros>          macros,
    ::std::shared_ptr<GameTexts>           texts,
    ::std::shared_ptr<GameWares>           wares,
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
                    ::std::placeholders::_3, ::std::placeholders::_4, vfs,
                    macros, texts, wares));
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
                    ::std::placeholders::_3, ::std::placeholders::_4, vfs,
                    macros, texts, wares));
                loader.parse(reader, ::std::move(context));
            }
        }
    }

    this->setGood();
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
    const QMap<QString, QString> &,
    ::std::shared_ptr<GameVFS>    vfs,
    ::std::shared_ptr<GameMacros> macros,
    ::std::shared_ptr<GameTexts>  texts,
    ::std::shared_ptr<GameWares>  wares)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    if (name == "group") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInGroupOfModuleGroups, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, vfs, macros,
            texts, wares));
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
    const QMap<QString, QString> &,
    ::std::shared_ptr<GameVFS>    vfs,
    ::std::shared_ptr<GameMacros> macros,
    ::std::shared_ptr<GameTexts>  texts,
    ::std::shared_ptr<GameWares>  wares)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    if (name == "groups") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInGroupsOfModuleGroups, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, vfs, macros,
            texts, wares));
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
    const QMap<QString, QString> &attr,
    ::std::shared_ptr<GameVFS>    vfs,
    ::std::shared_ptr<GameMacros> macros,
    ::std::shared_ptr<GameTexts>  texts,
    ::std::shared_ptr<GameWares>  wares)
{
    if (name == "select") {
        auto iter = attr.find("macro");
        if (iter != attr.end()) {
            this->loadMacro(*iter, vfs, macros, texts, wares);
        }
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}

/**
 * @brief		Load macro.
 */
void GameStationModules::loadMacro(const QString &               macro,
                                   ::std::shared_ptr<GameVFS>    vfs,
                                   ::std::shared_ptr<GameMacros> macros,
                                   ::std::shared_ptr<GameTexts>  texts,
                                   ::std::shared_ptr<GameWares>  wares)
{
    qDebug() << "Loading station module macro" << macro;
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open(macros->macro(macro) + ".xml");
    if (file == nullptr) {
        return;
    }

    QXmlStreamReader                      reader(file->readAll());
    XMLLoader                             loader;
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    context->setOnStartElement(::std::bind(
        &GameStationModules::onStartElementInRootOfMacro, this,
        ::std::placeholders::_1, ::std::placeholders::_2,
        ::std::placeholders::_3, ::std::placeholders::_4, texts, wares));
    loader.parse(reader, ::std::move(context));
}

/**
 * @brief		Start element callback in macro.
 */
bool GameStationModules::onStartElementInRootOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<GameTexts> texts,
    ::std::shared_ptr<GameWares> wares)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macros") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInMacrosOfMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, texts, wares));
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in macro.
 */
bool GameStationModules::onStartElementInMacrosOfMacro(
    XMLLoader &                   loader,
    XMLLoader::Context &          currentContext,
    const QString &               name,
    const QMap<QString, QString> &attr,
    ::std::shared_ptr<GameTexts>  texts,
    ::std::shared_ptr<GameWares>  wares)

{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "macro") {
        auto iter = attr.find("class");
        if (iter == attr.end()) {
            loader.pushContext(::std::move(context));
            return true;
        }

        ::std::shared_ptr<StationModule> module;
        auto moduleiter = m_modulesIndex.find(attr["name"]);
        if (moduleiter == m_modulesIndex.end()) {
            if (*iter == "buildmodule") {
                ::std::shared_ptr<struct BuildModule> buildModule(
                    new struct BuildModule);
                buildModule->moduleClass = StationModuleClass::BuildModule;
                module                   = buildModule;

            } else if (*iter == "connectionmodule") {
                ::std::shared_ptr<struct ConnectionModule> connectionModule(
                    new struct ConnectionModule);
                connectionModule->moduleClass
                    = StationModuleClass::ConnectionModule;
                module = connectionModule;

            } else if (*iter == "defencemodule") {
                ::std::shared_ptr<struct DefenceModule> defenceModule(
                    new struct DefenceModule);
                defenceModule->moduleClass = StationModuleClass::DefenceModule;
                module                     = defenceModule;

            } else if (*iter == "dockarea" || *iter == "pier") {
                ::std::shared_ptr<struct DockareaModule> dockareaModule(
                    new struct DockareaModule);
                dockareaModule->moduleClass = StationModuleClass::Dockarea;
                module                      = dockareaModule;

            } else if (*iter == "habitation") {
                ::std::shared_ptr<struct HabitationModule> habitationModule(
                    new struct HabitationModule);
                habitationModule->moduleClass = StationModuleClass::Habitation;
                module                        = habitationModule;

            } else if (*iter == "production") {
                ::std::shared_ptr<struct ProductionModule> productionModule(
                    new struct ProductionModule);
                productionModule->moduleClass = StationModuleClass::Production;
                module                        = productionModule;

            } else if (*iter == "storage") {
                ::std::shared_ptr<struct StorageModule> storageModule(
                    new struct StorageModule);
                storageModule->moduleClass = StationModuleClass::Storage;
                module                     = storageModule;

            } else {
                loader.pushContext(::std::move(context));
                return true;
            }
        } else {
            module = *moduleiter;
        }

        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInMacroOfMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, wares, module));

        module->macro        = attr["name"];
        module->playerModule = false;
        currentContext.setOnStopElement([module, texts](
                                            XMLLoader &, XMLLoader::Context &,
                                            const QString &name) -> bool {
            if (name == "macro" && module->playerModule) {
                // Debug info.
                qDebug() << "Module : {";
                qDebug() << "    macro           : " << module->macro;
                qDebug() << "    name            : "
                         << texts->text(module->name);
                qDebug() << "    moduleClass     : " << module->moduleClass;
                qDebug() << "    desctiption     : "
                         << texts->text(module->description);
                qDebug() << "    races           : " << module->races;
                qDebug() << "    hull            : " << module->hull;
                qDebug() << "    explosiondamage : " << module->explosiondamage;
                qDebug() << "}";
            }
            return true;
        });
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in macro.
 */
bool GameStationModules::onStartElementInMacroOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &name,
    const QMap<QString, QString> &,
    ::std::shared_ptr<GameWares>     wares,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    if (name == "properties") {
        switch (module->moduleClass) {
            case StationModuleClass::BuildModule:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInBuiildModulePropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
                break;

            case StationModuleClass::ConnectionModule:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInConnectionModulePropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
                break;

            case StationModuleClass::DefenceModule:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInDefenceModulePropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
                break;

            case StationModuleClass::Dockarea:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInDockareaPropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));
                break;

            case StationModuleClass::Habitation:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInHabitationPropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, wares,
                    module));
                break;

            case StationModuleClass::Production:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInProductionPropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, wares,
                    module));
                break;

            case StationModuleClass::Storage:
                context->setOnStartElement(::std::bind(
                    &GameStationModules::
                        onStartElementInStoragePropertiesOfMacro,
                    this, ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, module));

                break;
        }
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Load common data in propterties.
 */
bool GameStationModules::loadCommonPropertiesOfMacro(
    XMLLoader::Context *             context,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    if (name == "identification") {
        module->name        = GameTexts::IDPair(attr["name"]);
        module->description = GameTexts::IDPair(attr["description"]);
        auto iter           = attr.find("makerrace");
        if (iter != attr.end()) {
            for (auto &s : iter->split(" ")) {
                module->races.insert(s);
            }
        }

    } else if (name == "build") {
        context->setOnStartElement(::std::bind(
            &GameStationModules::onStartElementInBuildOfMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));

    } else if (name == "hull") {
        module->hull = attr["max"].toUInt();
    }

    return true;
}

/**
 * @brief		Start element callback in build.
 */
bool GameStationModules::onStartElementInBuildOfMacro(
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
            &GameStationModules::onStartElementInSetsOfMacro, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, module));
    }

    loader.pushContext(::std::move(context));

    return true;
}

/**
 * @brief		Start element callback in sets.
 */
bool GameStationModules::onStartElementInSetsOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    if (name == "set") {
        auto iter = attr.find("ref");
        if (iter != attr.end() && *iter == "headquarters_player") {
            module->playerModule = true;
            if (m_modulesIndex.find(module->macro) == m_modulesIndex.end()) {
                m_modules.push_back(module);
                m_modulesIndex[module->macro] = module;
            }
        }
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInBuiildModulePropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    ::std::shared_ptr<struct BuildModule> buildModule
        = ::std::static_pointer_cast<struct BuildModule>(module);

    if (name == "workforce") {
        buildModule->workforce = attr["max"].toUInt();

    } else {
        this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    }

    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInConnectionModulePropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInDefenceModulePropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInDockareaPropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();
    this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInHabitationPropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<GameWares>     wares,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    ::std::shared_ptr<struct HabitationModule> habitationModule
        = ::std::static_pointer_cast<struct HabitationModule>(module);
    if (name == "workforce") {
        /// Workforce
        habitationModule->races.insert(attr["race"]);
        habitationModule->workforce = attr["capacity"].toULong();

        /// Supply
        const ::std::shared_ptr<::GameWares::Ware> workunit
            = wares->ware("workunit_busy");
        for (auto &info : workunit->productionInfos) {
            if (info->method == attr["race"]) {
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
                    resource->id = res->id;
                    resource->amount
                        = (quint32)::round(((long double)res->amount)
                                           * supplyInfo->amount / info->amount);
                    supplyInfo->resources.append(resource);
                }

                habitationModule->supplyInfo = supplyInfo;
                break;
            }
        }

    } else {
        this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    }
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	wares		Game wares.
 * @param[in]	module		Module.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */

bool GameStationModules::onStartElementInProductionPropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<GameWares>     wares,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    ::std::shared_ptr<struct ProductionModule> productionModule
        = ::std::static_pointer_cast<struct ProductionModule>(module);
    if (name == "production") {
        productionModule->product = attr["wares"];
        context->setOnStartElement(
            ::std::bind(&GameStationModules::onStartElementInProductionOfMacro,
                        this, ::std::placeholders::_1, ::std::placeholders::_2,
                        ::std::placeholders::_3, ::std::placeholders::_4,
                        productionModule, wares));

    } else {
        this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    }
    loader.pushContext(::std::move(context));
    return true;
}

/**
 * @brief		Start element callback in propterties.
 */
bool GameStationModules::onStartElementInStoragePropertiesOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                  name,
    const QMap<QString, QString> &   attr,
    ::std::shared_ptr<StationModule> module)
{
    ::std::unique_ptr<XMLLoader::Context> context
        = XMLLoader::Context::create();

    ::std::shared_ptr<struct StorageModule> storageModule
        = ::std::static_pointer_cast<struct StorageModule>(module);
    if (name == "cargo") {
        storageModule->cargoSize = attr["max"].toULong();
        if (attr["tags"] == "container") {
            storageModule->cargoType = GameWares::TransportType::Container;

        } else if (attr["tags"] == "liquid") {
            storageModule->cargoType = GameWares::TransportType::Liquid;

        } else if (attr["tags"] == "solid") {
            storageModule->cargoType = GameWares::TransportType::Solid;
        }

    } else {
        this->loadCommonPropertiesOfMacro(context.get(), name, attr, module);
    }
    loader.pushContext(::std::move(context));
    return true;
}
/**
 * @brief		Start element callback in production.
 */
bool GameStationModules::onStartElementInProductionOfMacro(
    XMLLoader &loader,
    XMLLoader::Context &,
    const QString &                            name,
    const QMap<QString, QString> &             attr,
    ::std::shared_ptr<struct ProductionModule> module,
    ::std::shared_ptr<GameWares>               wares)
{
    if (name == "queue") {
        QString method = "default";
        auto    iter   = attr.find("method");
        if (iter != attr.end()) {
            method = *iter;
        }
        ::std::shared_ptr<GameWares::Ware> ware = wares->ware(attr["ware"]);

        for (::std::shared_ptr<GameWares::ProductionInfo> productionInfo :
             ware->productionInfos) {
            if (productionInfo->method == method) {
                module->productionInfos.append(productionInfo);
            }
        }
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}
