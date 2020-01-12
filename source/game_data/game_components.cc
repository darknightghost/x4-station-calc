#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_components.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	vfs				Virtual filesystem of the game.
 * @param[in]	setTextFunc		Callback to set text.
 */
GameComponents::GameComponents(
    ::std::shared_ptr<GameVFS>             vfs,
    ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_COMPONENTS"));
    qDebug() << "Loading components...";

    // Open file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/index/components.xml");
    QByteArray       data = file->readAll();
    QXmlStreamReader reader(data);

    // Find entery
    if (! reader.readNextStartElement() || reader.name() != "index") {
        return;
    }

    // Load
    quint64 level = 1;
    while (level > 0 && ! reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();
        switch (type) {
            case QXmlStreamReader::TokenType::StartElement:
                if (level == 1 && reader.name() == "entry") {
                    QXmlStreamAttributes attrs = reader.attributes();
                    if (attrs.hasAttribute("name")
                        && attrs.hasAttribute("value")) {
                        QString name  = attrs.value("name").toString();
                        QString value = "/";
                        value.append(
                            attrs.value("value").toString().replace('\\', '/'));
                        m_components[name] = value;
                        qDebug() << "Component " << name << "=" << value << ".";
                    }
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
 * @brief	Get component.
 *
 * @return	Value of component.
 */
QString GameComponents::component(const QString &id)
{
    return m_components[id];
}

/**
 * @brief		Destructor.
 */
GameComponents::~GameComponents() {}
