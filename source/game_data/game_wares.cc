#include <QtCore/QDebug>
#include <QtCore/QRegExp>

#include <game_data/game_data.h>
#include <game_data/game_wares.h>
#include <game_data/xml_loader/xml_loader.h>

// Transport type map.
QMap<QString, GameWares::TransportType> GameWares::_transportTypeMap
    = {{"container", TransportType::Container},
       {"liquid", TransportType::Liquid},
       {"solid", TransportType::Solid},
       {"inventory", TransportType::Inventory},
       {"equipment", TransportType::Equipment},
       {"workunit", TransportType::WorkUnit},
       {"research", TransportType::Research},
       {"passenger", TransportType::Passenger},
       {"ship", TransportType::Ship},
       {"software", TransportType::Software}};

/**
 * @brief		Constructor.
 */
GameWares::GameWares(GameData *                             gameData,
                     ::std::function<void(const QString &)> setTextFunc) :
    m_unknowWareIndex(0),
    m_unknowWareGroupIndex(0)
{
    setTextFunc(STR("STR_LOADING_WARES"));
    qDebug() << "Loading wares groups...";

    // Ware groups.
    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles("libraries/waregroups\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"libraries/waregroups.xml\".";
        return;
    }

    auto                 xmlLoader = this->createWareGroupXMLLoader(gameData);
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

    qDebug() << "Loading wares...";

    // Ware groups.
    // Scan files to load.
    xmlFiles = gameData->scanModuleFiles("libraries/wares\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"libraries/wares.xml\".";
        return;
    }

    xmlLoader = this->createWareXMLLoader(gameData);
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
 * @brief	Get ware group information.
 */
::std::shared_ptr<GameWares::WareGroup>
    GameWares::wareGroup(const QString &id, ::std::shared_ptr<GameTexts> texts)
{
    if (texts == nullptr) {
        texts = GameData::instance()->texts();
    }

    auto iter = m_wareGroups.find(id);
    if (iter == m_wareGroups.end()) {
        ::std::shared_ptr<GameWares::WareGroup> unknowWareGroup(new WareGroup(
            {id,
             texts->addText(
                 QString("UNKNOW_WARE_GROUP_%1")
                     .arg(m_unknowWareGroupIndex.fetchAndAddAcquire(1))),
             {}}));
        m_wareGroups[id] = unknowWareGroup;
        qWarning() << "Unknow ware group, id =" << id << ".";
        return unknowWareGroup;
    } else {
        return iter.value();
    }
}

/**
 * @brief	Get ware information.
 */
::std::shared_ptr<GameWares::Ware>
    GameWares::ware(const QString &id, ::std::shared_ptr<GameTexts> texts)
{
    if (texts == nullptr) {
        texts = GameData::instance()->texts();
    }

    auto iter = m_wares.find(id);
    if (iter == m_wares.end()) {
        // Generate an unknow ware.
        ::std::shared_ptr<GameWares::Ware> unknowWare(new Ware(
            {id,
             texts->addText(QString("UNKNOW_WARE_%1")
                                .arg(m_unknowWareIndex.fetchAndAddAcquire(1))),
             QString(""),
             QString(""),
             TransportType::Unknow,
             0,
             {},
             1,
             1,
             1,
             {}}));
        m_wares[id] = unknowWare;
        qWarning() << "Unknow ware, id =" << id << ".";
        return unknowWare;
    } else {
        return iter.value();
    }
}

/**
 * @brief		Destructor.
 */
GameWares::~GameWares() {}

/**
 * @brief       Create XML loader of ware groups.
 */
::std::unique_ptr<XMLLoader>
    GameWares::createWareGroupXMLLoader(GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /races/race
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/groups/group");
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
                << "Missing attribute \"id\" in element \"/groups/group\".";
        }
        QString id = iter->second;
        // Name.
        iter = attributes.find("name");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"name\" in element \"/groups/group\".";
        }
        QString name = iter->second;
        // Tags.
        iter = attributes.find("tags");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"tags\" in element \"/groups/group\".";
        }
        QString tags = iter->second;

        ::std::shared_ptr<WareGroup> group(new WareGroup(
            {id, name,
             tags.split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts)}));
        m_wareGroups[group->id] = group;

        qDebug() << "Ware group : id:" << group->id
                 << ", name:" << gameTexts->text(group->name)
                 << ", tags: " << group->tags;
        return true;
    });

    return ret;
}

/**
 * @brief       Create XML loader of wares.
 */
::std::unique_ptr<XMLLoader> GameWares::createWareXMLLoader(GameData *gameData)
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /wares/ware
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/wares/ware");
    elementLoader->setOnStartElement([this](XMLLoader &xmlLoader,
                                            XMLLoader::XMLElementLoader &,
                                            const ::std::map<QString, QString>
                                                &attributes) -> bool {
        // ID.
        auto iter = attributes.find("id");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"id\" in element \"/wares/ware\".";
            return true;
        }
        QString id = iter->second;

        // Name.
        iter = attributes.find("name");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"name\" in element \"/wares/ware\".";
            return true;
        }
        QString name = iter->second;

        // Description.
        iter                = attributes.find("description");
        QString description = "";
        if (iter != attributes.end()) {
            description = iter->second;
        }

        // Group.
        iter          = attributes.find("group");
        QString group = "";
        if (iter != attributes.end()) {
            group = iter->second;
        }

        // Transport.
        iter = attributes.find("transport");
        if (iter == attributes.end()) {
            qWarning() << "Missing attribute \"transport\" in element "
                          "\"/wares/ware\".";
            return true;
        }
        TransportType transType;
        auto          transTypeIter = _transportTypeMap.find(iter->second);
        if (transTypeIter == _transportTypeMap.end()) {
            transType = TransportType::Unknow;

        } else {
            transType = transTypeIter.value();
        }

        // Volume.
        iter = attributes.find("volume");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"volume\" in element \"/wares/ware\".";
            return true;
        }
        QString volume = iter->second;

        // Tags.
        iter = attributes.find("tags");
        QStringList tags;
        if (iter != attributes.end()) {
            tags = iter->second.split(" ",
                                      Qt::SplitBehaviorFlags::SkipEmptyParts);
        }

        // Create ware.
        ::std::shared_ptr<Ware> ware(new Ware({id,
                                               name,
                                               description,
                                               group,
                                               transType,
                                               volume.toUInt(),
                                               tags,
                                               0,
                                               0,
                                               0,
                                               {}}));
        m_wares[ware->id]          = ware;
        xmlLoader.values()["ware"] = ware;

        return true;
    });

    elementLoader->setOnStopElement([gameData](
                                        XMLLoader &xmlLoader,
                                        XMLLoader::XMLElementLoader &) -> bool {
        // Load saved values.
        auto wareIter = xmlLoader.values().find("ware");
        if (wareIter == xmlLoader.values().end()) {
            return true;
        }
        ::std::shared_ptr<Ware> ware
            = ::std::any_cast<::std::shared_ptr<Ware>>(wareIter->second);
        auto gameTexts = gameData->texts();

        qDebug() << "ware: {";
        qDebug() << "    id          :" << ware->id;
        qDebug() << "    name        :" << gameTexts->text(ware->name);
        qDebug() << "    description :" << gameTexts->text(ware->description);
        qDebug() << "    group       :" << ware->group;
        qDebug() << "    transport   :" << ware->transportType;
        qDebug() << "    volume      :" << ware->volume;
        qDebug() << "    tags        :" << ware->tags;
        qDebug() << "    minPrice    :" << ware->minPrice;
        qDebug() << "    averagePrice:" << ware->averagePrice;
        qDebug() << "    maxPrice    :" << ware->maxPrice;
        qDebug() << "    productionInfos:{";
        for (auto &info : ware->productionInfos) {
            qDebug() << "        name      :" << gameTexts->text(info->name);
            qDebug() << "        time      :" << info->time;
            qDebug() << "        method    :" << info->method;
            qDebug() << "        amount    :" << info->amount;
            for (auto &property : info->properties) {
                switch (property->propertyType) {
                    case ProductionInfoProperty::PropertyType::Effect: {
                        ::std::shared_ptr<Effect> effect
                            = ::std::static_pointer_cast<Effect>(property);
                        switch (effect->type) {
                            case Effect::Type::Efficiency:
                                qDebug() << "        effectType:"
                                         << "Efficiency";
                                break;

                            case Effect::Type::Work:
                                qDebug() << "        effectType:"
                                         << "Work";
                                break;

                            default:
                                qDebug() << "        effectType:"
                                         << "Unknow";
                                break;
                        };
                        qDebug() << "        effectProduct:" << effect->product;
                    } break;

                    default:
                        break;
                }
            }
            qDebug() << "        resource  :{";
            for (auto &res : info->resources) {
                qDebug() << "            {" << res->id << ", " << res->amount
                         << "},";
            }
            qDebug() << "        }";
        }
        qDebug() << "    }";
        qDebug() << "}";

        xmlLoader.values().erase("ware");
        return true;
    });

    // /wares/ware/price
    elementLoader = ret->elementLoader("/wares/ware/price");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Load saved values.
            auto wareIter = xmlLoader.values().find("ware");
            if (wareIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<Ware> ware
                = ::std::any_cast<::std::shared_ptr<Ware>>(wareIter->second);

            // Price
            auto iter = attributes.find("min");
            if (iter != attributes.end()) {
                ware->minPrice = iter->second.toUInt();
            }

            iter = attributes.find("average");
            if (iter != attributes.end()) {
                ware->averagePrice = iter->second.toUInt();
            }

            iter = attributes.find("max");
            if (iter != attributes.end()) {
                ware->maxPrice = iter->second.toUInt();
            }

            return true;
        });

    // /wares/ware/production
    elementLoader = ret->elementLoader("/wares/ware/production");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Load saved values.
            auto wareIter = xmlLoader.values().find("ware");
            if (wareIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<Ware> ware
                = ::std::any_cast<::std::shared_ptr<Ware>>(wareIter->second);

            // Name.
            auto iter = attributes.find("name");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"name\" in element "
                              "\"/wares/ware/production\".";
                return true;
            }
            QString name = iter->second;

            // Time.
            iter = attributes.find("time");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"time\" in element "
                              "\"/wares/ware/production\".";
                return true;
            }
            quint32 time = iter->second.toUInt();

            // Amount.
            iter = attributes.find("amount");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"amount\" in element "
                              "\"/wares/ware/production\".";
                return true;
            }
            quint32 amount = iter->second.toUInt();

            // Method.
            iter = attributes.find("method");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"Method\" in element "
                              "\"/wares/ware/production\".";
                return true;
            }
            QString method = iter->second;

            ::std::shared_ptr<ProductionInfo> info(new ProductionInfo(
                {name, ware->id, time, amount, method, {}, {}}));

            ware->productionInfos[method]    = info;
            xmlLoader.values()["production"] = info;

            return true;
        });
    elementLoader->setOnStopElement([](XMLLoader &xmlLoader,
                                       XMLLoader::XMLElementLoader &) -> bool {
        if (xmlLoader.values().find("production") == xmlLoader.values().end()) {
            return true;
        }
        xmlLoader.values().erase("production");

        return true;
    });

    // /wares/ware/production/primary/ware
    elementLoader = ret->elementLoader("/wares/ware/production/primary/ware");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Load saved values.
            auto wareIter = xmlLoader.values().find("ware");
            if (wareIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<Ware> ware
                = ::std::any_cast<::std::shared_ptr<Ware>>(wareIter->second);

            auto productionIter = xmlLoader.values().find("production");
            if (productionIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<ProductionInfo> productionInfo
                = ::std::any_cast<::std::shared_ptr<ProductionInfo>>(
                    productionIter->second);

            // Ware.
            auto iter = attributes.find("ware");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"ware\" in element "
                              "\"/wares/ware/production/primary/ware\".";
                return true;
            }
            QString resourceWare = iter->second;

            // Amount.
            iter = attributes.find("amount");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"amount\" in element "
                              "\"/wares/ware/production/primary/ware\".";
                return true;
            }
            quint32 amount = iter->second.toUInt();

            productionInfo->resources[resourceWare]
                = ::std::shared_ptr<Resource>(
                    new Resource({resourceWare, amount}));

            return true;
        });

    // /wares/ware/production/effects/effect
    elementLoader = ret->elementLoader("/wares/ware/production/effects/effect");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            // Load saved values.
            auto wareIter = xmlLoader.values().find("ware");
            if (wareIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<Ware> ware
                = ::std::any_cast<::std::shared_ptr<Ware>>(wareIter->second);

            auto productionIter = xmlLoader.values().find("production");
            if (productionIter == xmlLoader.values().end()) {
                return true;
            }
            ::std::shared_ptr<ProductionInfo> productionInfo
                = ::std::any_cast<::std::shared_ptr<ProductionInfo>>(
                    productionIter->second);

            // Type.
            auto iter = attributes.find("type");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"type\" in element "
                              "\"/wares/ware/production/effects/effect\".";
                return true;
            }
            Effect::Type type;
            if (iter->second == "efficiency") {
                type = Effect::Type::Efficiency;

            } else if (iter->second == "work") {
                type = Effect::Type::Work;

            } else {
                type = Effect::Type::Unknow;
            }

            // Product.
            iter = attributes.find("product");
            if (iter == attributes.end()) {
                qWarning() << "Missing attribute \"product\" in element "
                              "\"/wares/ware/production/effects/effect\".";
                return true;
            }
            double product = iter->second.toDouble();

            productionInfo
                ->properties[ProductionInfoProperty::PropertyType::Effect]
                = ::std::shared_ptr<Effect>(new Effect(type, product));

            return true;
        });

    return ret;
}
