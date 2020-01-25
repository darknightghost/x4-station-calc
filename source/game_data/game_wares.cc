#include <QtCore/QDebug>

#include <game_data/game_wares.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	vfs				Virtual filesystem of the game.
 * @param[in]	texts			Game texts.
 * @param[in]	setTextFunc		Callback to set text.
 */
GameWares::GameWares(::std::shared_ptr<GameVFS>             vfs,
                     ::std::shared_ptr<GameTexts>           texts,
                     ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_WARES"));
    qDebug() << "Loading wares...";

    // Open group file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/libraries/waregroups.xml");
    if (file == nullptr) {
        return;
    }
    QByteArray       data = file->readAll();
    QXmlStreamReader groupReader(data);

    // Parse group file
    auto context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameWares::onStartElementInGroupRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, texts));
    XMLLoader loader;
    if (! loader.parse(groupReader, ::std::move(context))) {
        return;
    }

    // Open ware file.
    file = vfs->open("/libraries/wares.xml");
    if (file == nullptr) {
        return;
    }
    data = file->readAll();
    QXmlStreamReader waresReader(data);

    // Parse ware file
    context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameWares::onStartElementInWaresRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4, texts));
    if (! loader.parse(waresReader, ::std::move(context))) {
        return;
    }

    this->setGood();
}

/**
 * @brief	Get ware group information.
 *
 * @return	Information of ware group.
 */
const ::std::shared_ptr<GameWares::WareGroup>
    GameWares::wareGroup(const QString &id)
{
    auto iter = m_wareGroups.find(id);
    if (iter == m_wareGroups.end()) {
        return nullptr;

    } else {
        return iter.value();
    }
}

/**
 * @brief	Get ware information.
 *
 * @return	Information of ware.
 */
const ::std::shared_ptr<GameWares::Ware> GameWares::ware(const QString &id)
{
    auto iter = m_wares.find(id);
    if (iter == m_wares.end()) {
        return nullptr;
    } else {
        return iter.value();
    }
}

/**
 * @brief		Destructor.
 */
GameWares::~GameWares() {}

/**
 * @brief		Start element callback in root of group file.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	texts		Game texts.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInGroupRoot(XMLLoader &                   loader,
                                          XMLLoader::Context &          context,
                                          const QString &               name,
                                          const QMap<QString, QString> &attr,
                                          ::std::shared_ptr<GameTexts>  texts)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(attr);
    if (name == "groups") {
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInGroups, this, ::std::placeholders::_1,
            ::std::placeholders::_2, ::std::placeholders::_3,
            ::std::placeholders::_4, texts));
        loader.pushContext(::std::move(context));

    } else {
        loader.pushContext(XMLLoader::Context::create());
    }
    return true;
}

/**
 * @brief		Start element callback in groups.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	texts		Game texts.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInGroups(XMLLoader &                   loader,
                                       XMLLoader::Context &          context,
                                       const QString &               name,
                                       const QMap<QString, QString> &attr,
                                       ::std::shared_ptr<GameTexts>  texts)
{
    UNREFERENCED_PARAMETER(context);
    if (name == "group") {
        if (attr.find("id") == attr.end() || attr.find("name") == attr.end()
            || attr.find("tags") == attr.end()) {
            return false;
        }

        ::std::shared_ptr<WareGroup> group(new WareGroup(
            {attr["id"], attr["name"],
             attr["tags"].split(" ", QString::SplitBehavior::SkipEmptyParts)}));
        m_wareGroups[group->id] = group;

        qDebug() << "Ware group : id:" << group->id
                 << ", name:" << texts->text(group->name)
                 << ", tags: " << group->tags;
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}

/**
 * @brief		Start element callback in the root node of wares.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	texts		Game texts.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInWaresRoot(XMLLoader &                   loader,
                                          XMLLoader::Context &          context,
                                          const QString &               name,
                                          const QMap<QString, QString> &attr,
                                          ::std::shared_ptr<GameTexts>  texts)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(attr);
    if (name == "wares") {
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInWares, this, ::std::placeholders::_1,
            ::std::placeholders::_2, ::std::placeholders::_3,
            ::std::placeholders::_4, texts));
        loader.pushContext(::std::move(context));
    } else {
        loader.pushContext(XMLLoader::Context::create());
    }
    return true;
}

/**
 * @brief		Start element callback in wares.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	texts		Game texts.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInWares(XMLLoader &                   loader,
                                      XMLLoader::Context &          context,
                                      const QString &               name,
                                      const QMap<QString, QString> &attr,
                                      ::std::shared_ptr<GameTexts>  texts)
{
    if (name == "ware") {
        if (attr.find("id") == attr.end() && attr.find("name") == attr.end()
            && attr.find("description") == attr.end()
            && attr.find("group") == attr.end()
            && attr.find("transport") == attr.end()
            && attr.find("volume") == attr.end()
            && attr.find("tags") == attr.end()) {
            loader.pushContext(XMLLoader::Context::create());
            return true;
        }
        // Create ware
        ::std::shared_ptr<Ware> ware(new Ware(
            {attr["id"],
             attr["name"],
             attr["description"],
             attr["group"],
             attr["transport"],
             attr["volume"].toUInt(),
             attr["tags"].split(" ", QString::SplitBehavior::SkipEmptyParts),
             1,
             1,
             1,
             {}}));

        m_wares[ware->id] = ware;

        context.setOnStopElement(::std::bind(
            [this](XMLLoader &loader, XMLLoader::Context &context,
                   const QString &name, ::std::shared_ptr<GameTexts> texts,
                   ::std::shared_ptr<Ware> ware) -> bool {
                UNREFERENCED_PARAMETER(loader);
                if (name == "ware") {
                    // Append ware
                    context.setOnStopElement(nullptr);

                    qDebug() << "ware: {";
                    qDebug() << "    id          :" << ware->id;
                    qDebug() << "    name        :" << texts->text(ware->name);
                    qDebug() << "    description :"
                             << texts->text(ware->description);
                    qDebug() << "    group       :" << ware->group;
                    qDebug() << "    transport   :" << ware->transport;
                    qDebug() << "    volume      :" << ware->volume;
                    qDebug() << "    tags        :" << ware->tags;
                    qDebug() << "    minPrice    :" << ware->minPrice;
                    qDebug() << "    averagePrice:" << ware->averagePrice;
                    qDebug() << "    maxPrice    :" << ware->maxPrice;
                    qDebug() << "    productionInfos:{";
                    for (auto &info : ware->productionInfos) {
                        qDebug() << "        time      :" << info->time;
                        qDebug() << "        method    :" << info->method;
                        qDebug() << "        amount    :" << info->amount;
                        qDebug() << "        workEffect:" << info->workEffect;
                        qDebug() << "        resource  :{";
                        for (auto &res : info->resources) {
                            qDebug() << "            {" << res->id << ", "
                                     << res->amount << "},";
                        }
                        qDebug() << "        }";
                    }
                    qDebug() << "    }";
                    qDebug() << "}";
                }

                return true;
            },
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, texts, ware));
        // Push context
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInWare, this, ::std::placeholders::_1,
            ::std::placeholders::_2, ::std::placeholders::_3,
            ::std::placeholders::_4, ware));
        loader.pushContext(::std::move(context));
    } else {
        loader.pushContext(XMLLoader::Context::create());
    }
    return true;
}

/**
 * @brief		Start element callback in ware.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 * @param[in]	ware		Ware.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInWare(XMLLoader &                   loader,
                                     XMLLoader::Context &          context,
                                     const QString &               name,
                                     const QMap<QString, QString> &attr,
                                     ::std::shared_ptr<Ware>       ware)
{
    if (name == "price") {
        // Price
        auto iter = attr.find("min");
        if (iter != attr.end()) {
            ware->minPrice = iter.value().toUInt();
        }
        iter = attr.find("average");
        if (iter != attr.end()) {
            ware->averagePrice = iter.value().toUInt();
        }
        iter = attr.find("max");
        if (iter != attr.end()) {
            ware->maxPrice = iter.value().toUInt();
        }
        loader.pushContext(XMLLoader::Context::create());

    } else if (name == "production") {
        ::std::shared_ptr<ProductionInfo> info(
            new ProductionInfo({attr["time"].toUInt(),
                                attr["amount"].toUInt(),
                                attr["method"],
                                1,
                                {}}));

        ware->productionInfos.append(info);

        context.setOnStopElement(::std::bind(
            [](XMLLoader &loader, XMLLoader::Context &context,
               const QString &name, ::std::shared_ptr<Ware> ware,
               ::std::shared_ptr<ProductionInfo> info) -> bool {
                UNREFERENCED_PARAMETER(loader);
                if (name == "production") {
                    context.setOnStopElement(nullptr);
                }
                return true;
            },
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ware, info));

        // Push context
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInProduction, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4, info));
        loader.pushContext(::std::move(context));

    } else {
        loader.pushContext(XMLLoader::Context::create());
    }

    return true;
}

/**
 * @brief		Start element callback in production.
 *
 * @param[in]	loader			XML loader.
 * @param[in]	context			Context.
 * @param[in]	name			Name of the element.
 * @param[in]	attr			Attributes.
 * @param[in]	info			Proituction info.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInProduction(
    XMLLoader &                       loader,
    XMLLoader::Context &              context,
    const QString &                   name,
    const QMap<QString, QString> &    attr,
    ::std::shared_ptr<ProductionInfo> info)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(attr);
    if (name == "primary") {
        // Push context
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInPrimary, this, ::std::placeholders::_1,
            ::std::placeholders::_2, ::std::placeholders::_3,
            ::std::placeholders::_4, info));
        loader.pushContext(::std::move(context));

    } else if (name == "effects") {
        // Push context
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(::std::bind(
            &GameWares::onStartElementInEffects, this, ::std::placeholders::_1,
            ::std::placeholders::_2, ::std::placeholders::_3,
            ::std::placeholders::_4, info));
        loader.pushContext(::std::move(context));

    } else {
        loader.pushContext(XMLLoader::Context::create());
    }

    return true;
}

/**
 * @brief		Start element callback in primary.
 *
 * @param[in]	loader			XML loader.
 * @param[in]	context			Context.
 * @param[in]	name			Name of the element.
 * @param[in]	attr			Attributes.
 * @param[in]	info			Proituction info.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInPrimary(XMLLoader &                   loader,
                                        XMLLoader::Context &          context,
                                        const QString &               name,
                                        const QMap<QString, QString> &attr,
                                        ::std::shared_ptr<ProductionInfo> info)
{
    UNREFERENCED_PARAMETER(context);
    if (name == "ware") {
        info->resources.append(::std::shared_ptr<Resource>(
            new Resource({attr["ware"], attr["amount"].toUInt()})));
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}

/**
 * @brief		Start element callback in effects.
 *
 * @param[in]	loader			XML loader.
 * @param[in]	context			Context.
 * @param[in]	name			Name of the element.
 * @param[in]	attr			Attributes.
 * @param[in]	info			Proituction info.
 *
 * @return		Return \c true if the parsing should be continued.
 *				otherwise returns \c false.
 */
bool GameWares::onStartElementInEffects(XMLLoader &                   loader,
                                        XMLLoader::Context &          context,
                                        const QString &               name,
                                        const QMap<QString, QString> &attr,
                                        ::std::shared_ptr<ProductionInfo> info)
{
    UNREFERENCED_PARAMETER(context);
    if (name == "effect") {
        if (attr["type"] == "work") {
            info->workEffect = attr["product"].toDouble();
        }
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}
