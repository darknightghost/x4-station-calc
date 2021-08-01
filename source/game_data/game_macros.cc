#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_data.h>
#include <game_data/game_macros.h>
#include <game_data/xml_loader/xml_loader.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameMacros::GameMacros(GameData *                             gameData,
                       ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_MACROS"));
    qDebug() << "Loading macros...";

    // Scan files to load.
    QMap<QString, QVector<QString>> xmlFiles
        = gameData->scanModuleFiles("index/macros\\.xml");
    if (xmlFiles.empty()) {
        qWarning() << "Missing \"index/macros.xml\".";
        return;
    }

    auto                 xmlLoader = this->createXMLLoader();
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

    this->setInitialized();
}

/**
 * @brief	Get macro.
 */
QString GameMacros::macro(const QString &name)
{
    return m_macros[name];
}

/**
 * @brief		Destructor.
 */
GameMacros::~GameMacros() {}

/**
 * @brief       Create XML loader.
 */
::std::unique_ptr<XMLLoader> GameMacros::createXMLLoader()
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /index/entry
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/index/entry");
    elementLoader->setOnStartElement([this](XMLLoader &,
                                            XMLLoader::XMLElementLoader &,
                                            const ::std::map<QString, QString>
                                                &attributes) -> bool {
        auto iter = attributes.find("name");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"name\" in element \"/index/entry\".";
        }
        QString name = iter->second;

        iter = attributes.find("value");
        if (iter == attributes.end()) {
            qWarning()
                << "Missing attribute \"value\" in element \"/index/entry\".";
        }
        QString value = "/";
        value.append(iter->second);
        value.replace('\\', '/');
        value.replace(QRegExp("/+"), "/");
        m_macros[name] = value;
        qDebug() << "Macro " << name << "=" << value << ".";

        return true;
    });

    return ret;
}
