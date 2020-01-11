#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_macros.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	vfs				Virtual filesystem of the game.
 * @param[in]	setTextFunc		Callback to set text.
 */
GameMacro::GameMacro(::std::shared_ptr<GameVFS>             vfs,
                     ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_MACROS"));
    qDebug() << "Loading macros...";

    // Open file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/index/macros.xml");
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
                        QString name   = attrs.value("name").toString();
                        QString value  = attrs.value("value").toString();
                        m_macros[name] = value;
                        qDebug() << "Macro " << name << "=" << value << ".";
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
 * @brief	Get macro.
 *
 * @return	Value of macro.
 */
QString GameMacro::macro(const QString &id)
{
    return m_macros[id];
}

/**
 * @brief		Destructor.
 */
GameMacro::~GameMacro() {}
