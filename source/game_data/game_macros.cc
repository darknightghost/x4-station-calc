#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_macros.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameMacros::GameMacros(::std::shared_ptr<GameVFS>             vfs,
                       ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_MACROS"));
    qDebug() << "Loading macros...";

    // Open file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/index/macros.xml");
    if (file == nullptr) {
        return;
    }
    QByteArray       data = file->readAll();
    QXmlStreamReader reader(data);

    // Parse file
    auto context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameMacros::onStartElementInRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    XMLLoader loader;
    loader.parse(reader, ::std::move(context));

    this->setGood();
}

/**
 * @brief	Get macro.
 */
QString GameMacros::macro(const QString &id)
{
    return m_macros[id];
}

/**
 * @brief		Destructor.
 */
GameMacros::~GameMacros() {}

/**
 * @brief		Start element callback in root.
 */
bool GameMacros::onStartElementInRoot(XMLLoader &                   loader,
                                      XMLLoader::Context &          context,
                                      const QString &               name,
                                      const QMap<QString, QString> &attr)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(attr);
    if (name == "index") {
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(
            ::std::bind(&GameMacros::onStartElementInIndex, this,
                        ::std::placeholders::_1, ::std::placeholders::_2,
                        ::std::placeholders::_3, ::std::placeholders::_4));
        loader.pushContext(::std::move(context));

    } else {
        loader.pushContext(XMLLoader::Context::create());
    }
    return true;
}

/**
 * @brief		Start element callback in index.
 */
bool GameMacros::onStartElementInIndex(XMLLoader &                   loader,
                                       XMLLoader::Context &          context,
                                       const QString &               name,
                                       const QMap<QString, QString> &attr)
{
    UNREFERENCED_PARAMETER(context);
    if (name == "entry") {
        auto nameIter  = attr.find("name");
        auto valueIter = attr.find("value");
        if (nameIter != attr.end() && valueIter != attr.end()) {
            QString name  = nameIter.value();
            QString value = "/";
            value.append(valueIter.value());
            value.replace('\\', '/');
            m_macros[name] = value;
            qDebug() << "Macro " << name << "=" << value << ".";
        }
    }

    loader.pushContext(XMLLoader::Context::create());
    return true;
}
