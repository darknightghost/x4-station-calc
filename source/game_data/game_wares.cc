#include <QtCore/QDebug>
#include <QtCore/QRegExp>

#include <game_data/game_data.h>
#include <game_data/game_wares.h>

/**
 * @brief		Constructor.
 */
GameWares::GameWares(::std::shared_ptr<GameVFS>             vfs,
                     ::std::shared_ptr<GameTexts>           texts,
                     ::std::function<void(const QString &)> setTextFunc) :
    m_unknowWareIndex(0),
    m_unknowWareGroupIndex(0)
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
    /*
    auto context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameWares::onStartElementInGroupRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    XMLLoader loader;
    loader["texts"] = texts;
    if (! loader.parse(groupReader, ::std::move(context))) {
        return;
    }
    */

    // Open ware file.
    file = vfs->open("/libraries/wares.xml");
    if (file == nullptr) {
        return;
    }
    data = file->readAll();
    QXmlStreamReader waresReader(data);

    // Parse ware file
    /*
    context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameWares::onStartElementInWaresRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    loader["texts"] = texts;
    if (! loader.parse(waresReader, ::std::move(context))) {
        return;
    }
    */

    // Parse extension ware files
    ::std::shared_ptr<::GameVFS::DirReader> extensionsDir
        = vfs->openDir("/extensions");
    if (extensionsDir != nullptr) {
        for (auto iter = extensionsDir->begin(); iter != extensionsDir->end();
             ++iter) {
            if (iter->type == ::GameVFS::DirReader::EntryType::Directory) {
                file = vfs->open(QString("/extensions/%1/libraries/wares.xml")
                                     .arg(iter->name));
                if (file == nullptr) {
                    continue;
                }
                data = file->readAll();
                QXmlStreamReader waresReader(data);

                // Parse ware file
                /*
                context = XMLLoader::Context::create();
                context->setOnStartElement(::std::bind(
                    &GameWares::onStartElementInExtensionsWaresRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
                loader["texts"] = texts;
                loader.parse(waresReader, ::std::move(context));
                */
            }
        }
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
