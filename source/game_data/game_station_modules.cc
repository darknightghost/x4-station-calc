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
    GameData *gameData, ::std::function<void(const QString &)> setTextFunc)
{
    auto gameVfs = gameData->vfs();
    qDebug() << "Loading station modules...";
    setTextFunc(STR("STR_LOADING_STATION_MODULES"));

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles("libraries/modulegroups\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"libraries/modulegroups.xml\".";
        return;
    }

    auto                 xmlLoader = this->createModuleGroupsXMLLoader();
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

    // Scan files to load.
    xmlFiles = gameData->scanModuleFiles("libraries/modules\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"libraries/modules.xml\".";
        return;
    }

    xmlLoader = this->createModulesXMLLoader(gameData);
    iter      = xmlFiles.begin()->begin();
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
 * @brief       Create XML loader of module groups.
 */
::std::unique_ptr<XMLLoader> GameStationModules::createModuleGroupsXMLLoader()
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /groups/group
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/groups/group");
    elementLoader->setOnStartElement(
        [this](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
               const ::std::map<QString, QString> &attributes) -> bool {
            // Get name.
            auto attrIter = attributes.find("name");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"name\" in element "
                              "\"/groups/group\".";
                return false;
            }

            QString                               name = attrIter->second;
            ::std::shared_ptr<StationModuleGroup> moduleGroup;
            auto groupIter = m_moduleGroups.find(name);
            if (groupIter == m_moduleGroups.end()) {
                moduleGroup = ::std::shared_ptr<StationModuleGroup>(
                    new StationModuleGroup({name, {attrIter->second}}));
                m_moduleGroups[name] = moduleGroup;

            } else {
                moduleGroup = groupIter.value();
            }

            xmlLoader.values()["moduleGroup"] = moduleGroup;

            return true;
        });

    // /groups/group/select
    elementLoader = ret->elementLoader("/groups/group/select");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModuleGroup> moduleGroup
                = ::std::any_cast<::std::shared_ptr<StationModuleGroup>>(
                    xmlLoader.values()["moduleGroup"]);

            // Get macro.
            auto attrIter = attributes.find("macro");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"macro\" in element "
                              "\"/groups/group/select\".";
                return false;
            }

            moduleGroup->macros.insert(attrIter->second);

            return true;
        });

    return ret;
}

/**
 * @brief       Create XML loader of module groups.
 */
::std::unique_ptr<XMLLoader>
    GameStationModules::createModulesXMLLoader(GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /modules/module
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/modules/module");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Get id.
            auto attrIter = attributes.find("id");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"id\" in element "
                              "\"/modules/module\".";
                return false;
            }

            xmlLoader.values()["id"] = attrIter->second;

            // Get group.
            attrIter = attributes.find("group");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"group\" in element "
                              "\"/modules/module\".";
                return false;
            }

            xmlLoader.values()["group"] = attrIter->second;

            return true;
        });

    elementLoader->setOnStopElement([this, gameData](
                                        XMLLoader &xmlLoader,
                                        XMLLoader::XMLElementLoader &) -> bool {
        QString id    = ::std::any_cast<QString>(xmlLoader.values()["id"]);
        QString group = ::std::any_cast<QString>(xmlLoader.values()["group"]);
        QSet<QString> races
            = ::std::any_cast<QSet<QString>>(xmlLoader.values()["races"]);

        for (auto &macro : m_moduleGroups[group]->macros) {
            ::std::shared_ptr<StationModule> module(new StationModule);

            // Initialize properties.
            module->macro           = macro;
            module->group           = group;
            module->playerModule    = false;
            module->hull            = 0;
            module->explosiondamage = 0;
            module->races           = races;

            // Load macro.
            this->loadMacro(macro, module, gameData);
        }
        return true;
    });

    // /modules/module/category
    elementLoader = ret->elementLoader("/modules/module/category");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Get race.
            auto attrIter = attributes.find("race");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"name\" in element "
                              "\"/modules/module/category\".";
                return false;
            }

            QStringList racesList
                = QString(attrIter->second)
                      .replace("[", "")
                      .replace("]", "")
                      .split(",", Qt::SplitBehaviorFlags::SkipEmptyParts);
            QSet<QString> races;
            for (auto &race : racesList) {
                races.insert(race.replace(" ", ""));
            }

            xmlLoader.values()["races"] = races;

            return true;
        });

    return ret;
}

/**
 * @brief		Load macro.
 */
void GameStationModules::loadMacro(const QString &                  macro,
                                   ::std::shared_ptr<StationModule> module,
                                   GameData *                       gameData)
{
    if (m_modulesIndex.find(macro) != m_modulesIndex.end()) {
        return;
    }

    auto gameVfs    = gameData->vfs();
    auto gameMacros = gameData->macros();

    qDebug() << "Loading station module macro" << macro << "...";
    QString macroXml = gameMacros->macro(macro);
    if (macroXml.startsWith('/')) {
        macroXml = macroXml.right(macroXml.length() - 1);
    }
    QString macroXmlExp = macroXml + "\\.xml";
    macroXml            = macroXml + ".xml";

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles(macroXmlExp);
    if (xmlFiles.empty()) {
        qWarning() << "Missing" << macroXml << ".";
        return;
    }

    auto xmlLoader = this->createMacroXMLLoader(module, gameData);
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
            qWarning() << QString("Failed to patch file \"%1\", line: %2, "
                                  "col: %3 : %4.")
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
}

/**
 * @brief       Create XML loader of macros.
 */
::std::unique_ptr<XMLLoader> GameStationModules::createMacroXMLLoader(
    ::std::shared_ptr<StationModule> module, GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);
    ret->values()["module"] = module;

    // /macros/macro
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/macros/macro");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Name.
            auto attrIter = attributes.find("name");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"name\" in element "
                              "\"/macros/macro\".";
                return false;
            }

            QString name = attrIter->second;

            // Class.
            attrIter = attributes.find("class");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"class\" in element "
                              "\"/macros/macro\".";
                return false;
            }

            auto classIter = _classMap.find(attrIter->second);
            StationModule::StationModuleClass moduleClass;
            if (classIter != _classMap.end()) {
                moduleClass = classIter.value();

            } else {
                moduleClass = StationModule::StationModuleClass::Unknow;
            }

            // Set module class.
            module->moduleClass = moduleClass;

            return true;
        });
    elementLoader->setOnStopElement([this, gameData](
                                        XMLLoader &xmlLoader,
                                        XMLLoader::XMLElementLoader &) -> bool {
        ::std::shared_ptr<StationModule> module
            = ::std::any_cast<::std::shared_ptr<StationModule>>(
                xmlLoader.values()["module"]);

        if (module->playerModule) {
            ::std::shared_ptr<GameTexts> gameTexts = gameData->texts();

            // Add module.
            m_modulesIndex[module->macro] = module;
            m_modules.push_back(module);

            // Print information
            qDebug() << "module :{";
            qDebug() << "    "
                     << "id              :" << module->macro;
            qDebug() << "    "
                     << "component       :" << module->component;
            qDebug() << "    "
                     << "name            :" << gameTexts->text(module->name);
            qDebug() << "    "
                     << "description     :"
                     << gameTexts->text(module->description);
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

                case StationModule::StationModuleClass::ConnectionModule:
                    qDebug() << "    "
                             << "class           : "
                             << "ConnectionModule";
                    break;

                case StationModule::StationModuleClass::DefenceModule:
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
            qDebug() << "    "
                     << "races           : " << module->races;
            qDebug() << "    "
                     << "hull            : " << module->hull;
            qDebug() << "    "
                     << "explosiondamage : " << module->explosiondamage;
            qDebug() << "    "
                     << "propertues      : {";
            for (auto &baseProperty : module->properties) {
                switch (baseProperty->type) {
                    case Property::Type::MTurret: {
                        ::std::shared_ptr<HasMTurret> property
                            = ::std::static_pointer_cast<HasMTurret>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "m turret           : " << property->count;
                    } break;

                    case Property::Type::MShield: {
                        ::std::shared_ptr<HasMShield> property
                            = ::std::static_pointer_cast<HasMShield>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "m shield           : " << property->count;
                    } break;

                    case Property::Type::LTurret: {
                        ::std::shared_ptr<HasLTurret> property
                            = ::std::static_pointer_cast<HasLTurret>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "l turret           : " << property->count;
                    } break;

                    case Property::Type::LShield: {
                        ::std::shared_ptr<HasLShield> property
                            = ::std::static_pointer_cast<HasLShield>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "l shield           : " << property->count;
                    } break;

                    case Property::Type::SDock: {
                        ::std::shared_ptr<HasSDock> property
                            = ::std::static_pointer_cast<HasSDock>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "s docking bay      : " << property->count;
                    } break;

                    case Property::Type::SShipCargo: {
                        ::std::shared_ptr<HasSShipCargo> property
                            = ::std::static_pointer_cast<HasSShipCargo>(
                                baseProperty);
                        qDebug()
                            << "    "
                            << "    "
                            << "s ship cargo       : " << property->capacity;
                    } break;

                    case Property::Type::MDock: {
                        ::std::shared_ptr<HasMDock> property
                            = ::std::static_pointer_cast<HasMDock>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "m docking bay      : " << property->count;
                    } break;

                    case Property::Type::MShipCargo: {
                        ::std::shared_ptr<HasMShipCargo> property
                            = ::std::static_pointer_cast<HasMShipCargo>(
                                baseProperty);
                        qDebug()
                            << "    "
                            << "    "
                            << "m ship cargo       : " << property->capacity;
                    } break;

                    case Property::Type::LDock: {
                        ::std::shared_ptr<HasLDock> property
                            = ::std::static_pointer_cast<HasLDock>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "l docking bay      : " << property->count;
                    } break;

                    case Property::Type::XLDock: {
                        ::std::shared_ptr<HasXLDock> property
                            = ::std::static_pointer_cast<HasXLDock>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "xl docking bay     : " << property->count;
                    } break;

                    case Property::Type::LXLDock: {
                        ::std::shared_ptr<HasLXLDock> property
                            = ::std::static_pointer_cast<HasLXLDock>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "l/xl docking bay   : " << property->count;
                    } break;

                    case Property::Type::SLaunchTube: {
                        ::std::shared_ptr<HasSLaunchTube> property
                            = ::std::static_pointer_cast<HasSLaunchTube>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "s launch tube      : " << property->count;
                    } break;

                    case Property::Type::MLaunchTube: {
                        ::std::shared_ptr<HasMLaunchTube> property
                            = ::std::static_pointer_cast<HasMLaunchTube>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "m launch tube      : " << property->count;
                    } break;

                    case Property::Type::SupplyWorkforce: {
                        ::std::shared_ptr<SupplyWorkforce> property
                            = ::std::static_pointer_cast<SupplyWorkforce>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "supply workforce   : {";
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "workforce : " << property->workforce;
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "supplies  : [";
                        for (auto &resource : property->supplyInfo->resources) {
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
                                << ::round(
                                       (double)(((long double)resource->amount)
                                                * property->workforce * 3600
                                                / property->supplyInfo->amount
                                                / property->supplyInfo->time))
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
                            = ::std::static_pointer_cast<RequireWorkforce>(
                                baseProperty);
                        qDebug()
                            << "    "
                            << "    "
                            << "workforce required : " << property->workforce;
                    } break;

                    case Property::Type::SupplyProduct: {
                        ::std::shared_ptr<SupplyProduct> property
                            = ::std::static_pointer_cast<SupplyProduct>(
                                baseProperty);
                        qDebug() << "    "
                                 << "    "
                                 << "product            : {";
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "id               :" << property->product;
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "time per round   :"
                                 << property->productionInfo->time << "s";

                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "amount per round :"
                                 << property->productionInfo->amount << " - "
                                 << ::round(property->productionInfo->amount
                                                * property->productionInfo
                                                      ->workEffect.product
                                            + 1.0);
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
                                << ::round(
                                       (double)(((long double)resource->amount)
                                                * 3600
                                                / property->productionInfo
                                                      ->time))
                                << "/h - "
                                << ::round(
                                       (double)(((long double)resource->amount)
                                                * 3600
                                                / property->productionInfo
                                                      ->time)
                                       * (1.0
                                          + property->productionInfo->workEffect
                                                .product))
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
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "type : " << property->cargoType;
                        qDebug() << "    "
                                 << "    "
                                 << "    "
                                 << "size : " << property->cargoSize << " m^3";
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
        return true;
    });

    // /macros/macro/component
    elementLoader = ret->elementLoader("/macros/macro/component");
    elementLoader->setOnStartElement(
        [this,
         gameData](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
                   const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Ref.
            auto attrIter = attributes.find("ref");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"ref\" in element "
                              "\"/macros/macro/component\".";
                return false;
            }

            QString ref = attrIter->second;

            // Load component.
            module->component = ref;
            auto iter         = m_componentTmpIndex.find(ref);
            if (iter == m_componentTmpIndex.end()) {
                m_componentTmpIndex[module->component] = {module};
            } else {
                m_componentTmpIndex[module->component].push_back(module);
            }

            this->loadComponent(ref, module, xmlLoader, gameData);

            return true;
        });

    // /macros/macro/properties/identification
    elementLoader
        = ret->elementLoader("/macros/macro/properties/identification");
    elementLoader->setOnStartElement(
        [gameData](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
                   const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);
            auto gameRaces = gameData->races();

            // Name.
            auto attrIter = attributes.find("name");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"name\" in element "
                              "\"/macros/macro/properties/identification\".";
                return false;
            }
            module->name = GameTexts::IDPair(attrIter->second);

            // Makerrace.
            attrIter = attributes.find("makerrace");
            if (attrIter != attributes.end()) {
                module->races.insert(attrIter->second);
            }

            // Description.
            attrIter = attributes.find("description");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"description\" in element "
                              "\"/macros/macro/properties/identification\".";
                return false;
            }
            module->description = GameTexts::IDPair(attrIter->second);

            return true;
        });

    // /macros/macro/properties/build/sets/set
    elementLoader
        = ret->elementLoader("/macros/macro/properties/build/sets/set");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Ref.
            auto attrIter = attributes.find("ref");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"ref\" in element "
                              "\"/macros/macro/properties/build/sets/set\".";
                return true;
            }

            if (attrIter->second == "headquarters_player") {
                module->playerModule = true;
            }
            return true;
        });

    // /macros/macro/properties/explosiondamage
    elementLoader
        = ret->elementLoader("/macros/macro/properties/explosiondamage");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Value.
            auto attrIter = attributes.find("value");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"ref\" in element "
                              "\"/macros/macro/properties/explosiondamage\".";
                return false;
            }

            module->explosiondamage = attrIter->second.toUInt();

            return true;
        });

    // /macros/macro/properties/hull
    elementLoader = ret->elementLoader("/macros/macro/properties/hull");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // max.
            auto attrIter = attributes.find("max");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"max\" in element "
                              "\"/macros/macro/properties/hull\".";
                return false;
            }

            module->hull = attrIter->second.toUInt();

            return true;
        });

    // /macros/macro/properties/workforce
    elementLoader = ret->elementLoader("/macros/macro/properties/workforce");
    elementLoader->setOnStartElement(
        [gameData](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
                   const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Max.
            auto attrIter = attributes.find("max");

            if (attrIter == attributes.end()) {
                // Supply.
                ::std::shared_ptr<SupplyWorkforce> property;
                auto                               iter
                    = module->properties.find(Property::Type::SupplyWorkforce);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<SupplyWorkforce>(
                        new SupplyWorkforce);
                    module->properties[property->type] = property;
                } else {
                    property
                        = ::std::static_pointer_cast<SupplyWorkforce>(*iter);
                }

                attrIter = attributes.find("capacity");
                if (attrIter == attributes.end()) {
                    qWarning() << "Missing attribute \"capacity\" in element "
                                  "\"/macros/macro/properties/workforce\".";
                    return false;
                }
                quint64 workforce = attrIter->second.toULong();

                attrIter = attributes.find("race");
                if (attrIter == attributes.end()) {
                    qWarning() << "Missing attribute \"race\" in element "
                                  "\"/macros/macro/properties/workforce\".";
                    return false;
                }
                QString race = attrIter->second;

                property->workforce = workforce;

                // Supply
                auto gameWares = gameData->wares();
                auto gameTexts = gameData->texts();
                const ::std::shared_ptr<::GameWares::Ware> workunit
                    = gameWares->ware("workunit_busy", gameTexts);
                for (auto &info : workunit->productionInfos) {
                    if (info->method == race || info->method == "default") {
                        ::std::shared_ptr<::GameWares::ProductionInfo>
                            supplyInfo(new ::GameWares::ProductionInfo());
                        supplyInfo->id     = "";
                        supplyInfo->time   = info->time;
                        supplyInfo->amount = workforce;
                        supplyInfo->method = race;
                        supplyInfo->workEffect
                            = {GameWares::WorkEffect::Type::Work, 0};
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
                        if (info->method == race) {
                            break;
                        }
                    }
                }
                if (property->supplyInfo == nullptr) {
                    module->properties.remove(Property::Type::SupplyWorkforce);
                }
            } else {
                quint64 workforce = attrIter->second.toULong();

                // Require.
                ::std::shared_ptr<RequireWorkforce> property;
                auto                                iter
                    = module->properties.find(Property::Type::RequireWorkforce);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<RequireWorkforce>(
                        new RequireWorkforce);
                    module->properties[property->type] = property;
                } else {
                    property
                        = ::std::static_pointer_cast<RequireWorkforce>(*iter);
                }
                property->workforce = workforce;
            }

            return true;
        });

    // /macros/macro/properties/production/queue
    elementLoader
        = ret->elementLoader("/macros/macro/properties/production/queue");
    elementLoader->setOnStartElement(
        [gameData](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
                   const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);
            auto gameWares = gameData->wares();
            auto gameTexts = gameData->texts();

            // Ware.
            auto attrIter = attributes.find("ware");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"ware\" in element "
                              "\"/macros/macro/properties/production/queue\".";
                return false;
            }

            QString attrWare = attrIter->second;

            // Method.
            QString attrMethod = "default";
            attrIter           = attributes.find("method");
            if (attrIter != attributes.end()) {
                attrMethod = attrIter->second;
            }

            ::std::shared_ptr<GameWares::Ware> ware
                = gameWares->ware(attrWare, gameTexts);

            auto productionInfoIter = ware->productionInfos.find(attrMethod);
            if (productionInfoIter == ware->productionInfos.end()) {
                productionInfoIter = ware->productionInfos.find("default");
            }

            if (productionInfoIter != ware->productionInfos.end()) {
                ::std::shared_ptr<SupplyProduct>             property;
                ::std::shared_ptr<GameWares::ProductionInfo> productionInfo
                    = *productionInfoIter;
                auto iter
                    = module->properties.find(Property::Type::SupplyProduct);
                if (iter == module->properties.end()) {
                    property
                        = ::std::shared_ptr<SupplyProduct>(new SupplyProduct);
                    module->properties[property->type] = property;
                } else {
                    property = ::std::static_pointer_cast<SupplyProduct>(*iter);
                }

                property->product        = attrWare;
                property->productionInfo = productionInfo;
            }

            return true;
        });

    // /macros/macro/properties/cargo
    elementLoader = ret->elementLoader("/macros/macro/properties/cargo");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Max.
            auto attrIter = attributes.find("max");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"max\" in element "
                              "\"/macros/macro/properties/cargo\".";
                return false;
            }

            quint32 max = attrIter->second.toUInt();

            // Tags.
            attrIter = attributes.find("tags");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"tags\" in element "
                              "\"/macros/macro/properties/cargo\".";
                return false;
            }

            QStringList tags = attrIter->second.split(
                " ", Qt::SplitBehaviorFlags::SkipEmptyParts);

            ::std::shared_ptr<HasCargo> property;
            auto iter = module->properties.find(Property::Type::Cargo);
            if (iter == module->properties.end()) {
                property = ::std::shared_ptr<HasCargo>(new HasCargo);
                module->properties[property->type] = property;
            } else {
                property = ::std::static_pointer_cast<HasCargo>(*iter);
            }

            for (QString tag : tags) {
                auto tagIter = GameWares::_transportTypeMap.find(tag);
                if (tagIter == GameWares::_transportTypeMap.end()) {
                    property->cargoType = *tagIter;

                } else {
                    property->cargoType = GameWares::TransportType::Unknow;
                }
            }

            if (property->cargoType == GameWares::TransportType::Unknow) {
                module->properties.remove(Property::Type::Cargo);
            }

            property->cargoSize = max;

            return true;
        });

    // /macros/macro/connections/connection/macro
    elementLoader
        = ret->elementLoader("/macros/macro/connections/connection/macro");
    elementLoader->setOnStartElement(
        [this,
         gameData](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
                   const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Ref.
            auto attrIter = attributes.find("ref");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"ref\" in element "
                              "\"/macros/macro/connections/connection/macro\".";
                return false;
            }

            QString ref = attrIter->second;

            QRegExp sTubeExp("launchtube_\\w+_s_\\w+_macro");
            sTubeExp.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
            QRegExp mTubeExp("launchtube_\\w+_m_\\w+_macro");
            mTubeExp.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
            if (sTubeExp.exactMatch(ref)) {
                ::std::shared_ptr<HasSLaunchTube> property;
                auto                              iter
                    = module->properties.find(Property::Type::SLaunchTube);
                if (iter == module->properties.end()) {
                    property
                        = ::std::shared_ptr<HasSLaunchTube>(new HasSLaunchTube);
                    module->properties[property->type] = property;
                } else {
                    property
                        = ::std::static_pointer_cast<HasSLaunchTube>(*iter);
                }

                ++(property->count);
            } else if (mTubeExp.exactMatch(ref)) {
                ::std::shared_ptr<HasMLaunchTube> property;
                auto                              iter
                    = module->properties.find(Property::Type::MLaunchTube);
                if (iter == module->properties.end()) {
                    property
                        = ::std::shared_ptr<HasMLaunchTube>(new HasMLaunchTube);
                    module->properties[property->type] = property;
                } else {
                    property
                        = ::std::static_pointer_cast<HasMLaunchTube>(*iter);
                }

                ++(property->count);
            } else {
                this->loadConnectionMacro(ref, module, gameData);
            }

            return true;
        });

    return ret;
}

/**
 * @brief		Load component.
 */
void GameStationModules::loadComponent(const QString &component,
                                       ::std::shared_ptr<StationModule> module,
                                       XMLLoader &macroLoader,
                                       GameData * gameData)
{
    if (m_modulesIndex.find(component) != m_modulesIndex.end()) {
        return;
    }

    auto gameVfs        = gameData->vfs();
    auto gameComponents = gameData->components();

    qDebug() << "Loading station module component" << component << "...";
    QString componentXml = gameComponents->component(component);
    if (componentXml.startsWith('/')) {
        componentXml = componentXml.right(componentXml.length() - 1);
    }
    QString componentXmlExp = componentXml + "\\.xml";
    componentXml            = componentXml + ".xml";

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles(componentXmlExp);
    if (xmlFiles.empty()) {
        qWarning() << "Missing" << componentXml << ".";
        return;
    }

    auto xmlLoader = this->createComponentXMLLoader(module, macroLoader);
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
            qWarning() << QString("Failed to patch file \"%1\", line: %2, "
                                  "col: %3 : %4.")
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
}

/**
 * @brief       Create XML loader of components.
 */
::std::unique_ptr<XMLLoader> GameStationModules::createComponentXMLLoader(
    ::std::shared_ptr<StationModule> module, XMLLoader &macroLoader)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);
    ret->values()["module"] = module;

    // /components/component/properties
    XMLLoader::XMLElementLoader *elementLoader = ret->copyElementLoader(
        macroLoader.elementLoader("/macros/macro/properties"),
        "/components/component/properties");

    // /components/component/connections/connection
    elementLoader
        = ret->elementLoader("/components/component/connections/connection");
    elementLoader->setOnStartElement([](XMLLoader &xmlLoader,
                                        XMLLoader::XMLElementLoader &,
                                        const ::std::map<QString, QString>
                                            &attributes) -> bool {
        ::std::shared_ptr<StationModule> module
            = ::std::any_cast<::std::shared_ptr<StationModule>>(
                xmlLoader.values()["module"]);

        // Tags.
        auto attrIter = attributes.find("tags");
        if (attrIter == attributes.end()) {
            return true;
        }

        auto tags = attrIter->second.split(
            " ", Qt::SplitBehaviorFlags::SkipEmptyParts);
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
                auto iter = module->properties.find(Property::Type::MTurret);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<HasMTurret>(new HasMTurret);
                    module->properties[property->type] = property;
                } else {
                    property = ::std::static_pointer_cast<HasMTurret>(*iter);
                }
                ++(property->count);
            } else if (foundLarge) {
                // L turret.
                ::std::shared_ptr<HasLTurret> property;
                auto iter = module->properties.find(Property::Type::LTurret);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<HasLTurret>(new HasLTurret);
                    module->properties[property->type] = property;
                } else {
                    property = ::std::static_pointer_cast<HasLTurret>(*iter);
                }
                ++(property->count);
            }
        } else if (foundLarge) {
            if (foundMedium) {
                // M shield.
                ::std::shared_ptr<HasMShield> property;
                auto iter = module->properties.find(Property::Type::MShield);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<HasMShield>(new HasMShield);
                    module->properties[property->type] = property;
                } else {
                    property = ::std::static_pointer_cast<HasMShield>(*iter);
                }
                ++(property->count);
            } else if (foundLarge) {
                // L shield.
                ::std::shared_ptr<HasLShield> property;
                auto iter = module->properties.find(Property::Type::LShield);
                if (iter == module->properties.end()) {
                    property = ::std::shared_ptr<HasLShield>(new HasLShield);
                    module->properties[property->type] = property;
                } else {
                    property = ::std::static_pointer_cast<HasLShield>(*iter);
                }
                ++(property->count);
            }
        }

        return true;
    });

    return ret;
}

/**
 * @brief		Load macro in connections of module macro.
 */
void GameStationModules::loadConnectionMacro(
    const QString &                                      macro,
    ::std::shared_ptr<GameStationModules::StationModule> module,
    GameData *                                           gameData)
{
    if (m_modulesIndex.find(macro) != m_modulesIndex.end()) {
        return;
    }

    auto gameVfs    = gameData->vfs();
    auto gameMacros = gameData->macros();

    qDebug() << "Loading station module macro" << macro << "...";
    QString macroXml = gameMacros->macro(macro);
    if (macroXml.startsWith('/')) {
        macroXml = macroXml.right(macroXml.length() - 1);
    }
    QString macroXmlExp = macroXml + "\\.xml";
    macroXml            = macroXml + ".xml";

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles(macroXmlExp);
    if (xmlFiles.empty()) {
        qWarning() << "Missing" << macroXml << ".";
        return;
    }

    auto xmlLoader = this->createConnectionMacroXMLLoader(module, gameData);
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
            qWarning() << QString("Failed to patch file \"%1\", line: %2, "
                                  "col: %3 : %4.")
                              .arg(*iter)
                              .arg(errorInfo.errorLine)
                              .arg(errorInfo.errorColumn)
                              .arg(errorInfo.errorMsg)
                              .toStdString()
                              .c_str();
        }
    }
    xmlLoader->parse();
}

/**
 * @brief       Create XML loader of connection macros.
 */
::std::unique_ptr<XMLLoader> GameStationModules::createConnectionMacroXMLLoader(
    ::std::shared_ptr<StationModule> module, GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);
    ret->values()["module"] = module;

    // /macros/macro
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/macros/macro");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<StationModule> module
                = ::std::any_cast<::std::shared_ptr<StationModule>>(
                    xmlLoader.values()["module"]);

            // Class.
            auto attrIter = attributes.find("class");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"class\" in element "
                              "\"/macros/macro\".";
                return false;
            }

            if (attrIter->second != "dockingbay") {
                return false;
            }

            return true;
        });

    // /macros/macro/properties
    elementLoader = ret->elementLoader("/macros/macro/properties");

    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &) -> bool {
            ::std::shared_ptr<TmpDockingBayInfo> info
                = ::std::shared_ptr<TmpDockingBayInfo>(
                    new TmpDockingBayInfo({0, 0, TmpDockingBayInfo::S}));
            xmlLoader.values()["info"] = info;

            return true;
        });
    elementLoader->setOnStopElement([](XMLLoader &xmlLoader,
                                       XMLLoader::XMLElementLoader &) -> bool {
        ::std::shared_ptr<StationModule> module
            = ::std::any_cast<::std::shared_ptr<StationModule>>(
                xmlLoader.values()["module"]);

        ::std::shared_ptr<TmpDockingBayInfo> info
            = ::std::any_cast<::std::shared_ptr<TmpDockingBayInfo>>(
                xmlLoader.values()["info"]);

        switch (info->type) {
            case TmpDockingBayInfo::S:
                if (info->count > 0) {
                    // S docking bay.
                    ::std::shared_ptr<HasSDock> property;
                    auto iter = module->properties.find(Property::Type::SDock);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasSDock>(new HasSDock);
                        module->properties[property->type] = property;
                    } else {
                        property = ::std::static_pointer_cast<HasSDock>(*iter);
                    }
                    property->count += info->count;
                }
                if (info->capacity > 0) {
                    // S ship cargo.
                    ::std::shared_ptr<HasSShipCargo> property;
                    auto                             iter
                        = module->properties.find(Property::Type::SShipCargo);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasSShipCargo>(
                            new HasSShipCargo);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasSShipCargo>(*iter);
                    }
                    property->capacity += info->capacity;
                }
                break;

            case TmpDockingBayInfo::M:
                if (info->count > 0) {
                    // M docking bay.
                    ::std::shared_ptr<HasMDock> property;
                    auto iter = module->properties.find(Property::Type::MDock);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasMDock>(new HasMDock);
                        module->properties[property->type] = property;
                    } else {
                        property = ::std::static_pointer_cast<HasMDock>(*iter);
                    }
                    property->count += info->count;
                }
                if (info->capacity > 0) {
                    // M ship cargo.
                    ::std::shared_ptr<HasMShipCargo> property;
                    auto                             iter
                        = module->properties.find(Property::Type::MShipCargo);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasMShipCargo>(
                            new HasMShipCargo);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasMShipCargo>(*iter);
                    }
                    property->capacity += info->capacity;
                }
                break;

            case TmpDockingBayInfo::L:
                if (info->count > 0) {
                    // L docking bay.
                    ::std::shared_ptr<HasLDock> property;
                    auto iter = module->properties.find(Property::Type::LDock);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasLDock>(new HasLDock);
                        module->properties[property->type] = property;
                    } else {
                        property = ::std::static_pointer_cast<HasLDock>(*iter);
                    }
                    property->count += info->count;
                }
                break;

            case TmpDockingBayInfo::XL:
                if (info->count > 0) {
                    // XL docking bay.
                    ::std::shared_ptr<HasXLDock> property;
                    auto iter = module->properties.find(Property::Type::XLDock);
                    if (iter == module->properties.end()) {
                        property = ::std::shared_ptr<HasXLDock>(new HasXLDock);
                        module->properties[property->type] = property;
                    } else {
                        property = ::std::static_pointer_cast<HasXLDock>(*iter);
                    }
                    property->count += info->count;
                }
                break;

            case TmpDockingBayInfo::L_XL:
                if (info->count > 0) {
                    // L/XL docking bay.
                    ::std::shared_ptr<HasLXLDock> property;
                    auto                          iter
                        = module->properties.find(Property::Type::LXLDock);
                    if (iter == module->properties.end()) {
                        property
                            = ::std::shared_ptr<HasLXLDock>(new HasLXLDock);
                        module->properties[property->type] = property;
                    } else {
                        property
                            = ::std::static_pointer_cast<HasLXLDock>(*iter);
                    }
                    property->count += info->count;
                }
                break;
        }
        return true;
    });

    // /macros/macro/properties/dock
    elementLoader = ret->elementLoader("/macros/macro/properties/dock");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<TmpDockingBayInfo> info
                = ::std::any_cast<::std::shared_ptr<TmpDockingBayInfo>>(
                    xmlLoader.values()["info"]);

            // External.
            auto attrIter = attributes.find("external");
            if (attrIter != attributes.end()) {
                info->count = attrIter->second.toUInt();
            }

            attrIter = attributes.find("capacity");
            if (attrIter != attributes.end()) {
                info->capacity = attrIter->second.toUInt();
            }

            return true;
        });

    // /macros/macro/properties/docksize
    elementLoader = ret->elementLoader("/macros/macro/properties/docksize");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            ::std::shared_ptr<TmpDockingBayInfo> info
                = ::std::any_cast<::std::shared_ptr<TmpDockingBayInfo>>(
                    xmlLoader.values()["info"]);

            // Ref.
            auto attrIter = attributes.find("tags");
            if (attrIter == attributes.end()) {
                qWarning() << "Missing attribute \"tags\" in element "
                              "\"/macros/macro/properties/docksize\".";
                return false;
            }

            QSet<QString> tags;
            for (auto &tag : attrIter->second.split(
                     " ", Qt::SplitBehaviorFlags::SkipEmptyParts)) {
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

            return true;
        });

    return ret;
}
