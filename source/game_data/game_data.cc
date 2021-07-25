#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QStandardPaths>
#include <QtCore/QThread>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <game_data/game_data.h>
#include <game_data/game_texts.h>
#include <game_data/xml_loader/xml_loader.h>
#include <locale/string_table.h>
#include <ui/select_steam_id_dialog.h>

#define USER_GAME_DIR "EgoSoft/X4"
#define CONTENT_XML   "content.xml"

/**
 * @brief		Constructor.
 */
GameData::GameData(SplashWidget *splash) : QObject(nullptr)
{
    while (true) {
        splash->setText(STR("STR_CHECKING_GAME_PATH"));

        // Check game path
        m_gamePath = Config::instance()->getString("/gamePath", "");
        QMap<QString, GameVFS::CatFileInfo> catFiles;
        if (! checkGamePath(m_gamePath, catFiles)) {
            if (splash->callFunc(::std::function<bool()>(
                    ::std::bind(&GameData::askGamePath, this)))) {
                continue;
            } else {
                return;
            }
        }

        // Scan user mods.
        QMap<QString, GameVFS::CatFileInfo> userCatFiles;
        this->scanUserPaths(splash, userCatFiles);

        // Load vfs
        splash->setText(STR("STR_LOADING_VFS"));
        ::std::shared_ptr<GameVFS> vfs = GameVFS::create(
            m_gamePath, m_userPath, catFiles, userCatFiles,
            [&](const QString &s) -> void {
                splash->setText(STR("STR_LOADING_VFS") + "\n" + s);
            },
            [&](const QString &s) -> void {
                splash->callFunc(::std::function<void()>([&]() -> void {
                    QMessageBox::critical(splash, STR("STR_ERROR"), s);
                }));
            });
        if (vfs == nullptr) {
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_vfs = vfs;

        // Load game modules.
        splash->setText(STR("STR_SCAN_GAME_MODULES"));
        this->scanGameModules();
        this->sortGameModules();

        // Load text
        ::std::shared_ptr<GameTexts> texts
            = GameTexts::load(this, [&](const QString &s) -> void {
                  splash->setText(STR("STR_LOADING_TEXTS") + "\n" + s);
              });

        if (texts == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_STRINGS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_texts = texts;

        // Load game macros
        ::std::shared_ptr<GameMacros> macros
            = GameMacros::load(this, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (macros == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_MACROS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_macros = macros;

        // TODO
        return;

        // Load game components
        ::std::shared_ptr<GameComponents> components
            = GameComponents::load(vfs, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (components == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_COMPONENTS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_components = components;

        // Load game races
        ::std::shared_ptr<GameRaces> races
            = GameRaces::load(vfs, texts, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (races == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_RACES"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_races = races;

        // Load game wares
        ::std::shared_ptr<GameWares> wares
            = GameWares::load(vfs, texts, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (wares == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_WARES"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_wares = wares;

        // Load station modules
        ::std::shared_ptr<GameStationModules> stationModules
            = GameStationModules::load(vfs, macros, texts, wares, components,
                                       [&](const QString &s) -> void {
                                           splash->setText(s);
                                       });

        if (stationModules == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_STATION_MODULES"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }
        m_stationModules = stationModules;

        break;
    }

    this->setInitialized();
}

/**
 * @brief		Check path of game.
 */
bool GameData::checkGamePath(const QString &path)
{
    QMap<QString, GameVFS::CatFileInfo> catFiles;
    return this->checkGamePath(path, catFiles);
}

/**
 * @brief		Check path of game.
 */
bool GameData::setGamePath(const QString &path)
{
    QMap<QString, GameVFS::CatFileInfo> catFiles;

    if (! checkGamePath(path, catFiles)) {
        return false;
    }

    Config::instance()->setString("/gamePath", path);
    return true;
}

/**
 * @brief Destructor.
 */
GameData::~GameData() {}

/**
 * @brief   Get game modules.
 */
const QMap<QString, ::std::shared_ptr<GameData::GameModule>> &
    GameData::gameModules() const
{
    return m_gameModules;
}

/**
 * @brief   Get load order of modules.
 */
const QVector<QString> &GameData::moduleLoadOrder() const
{
    return m_moduleLoadOrder;
}

/**
 * @brief       Scan files in module.
 */
QMap<QString, QVector<QString>>
    GameData::scanModuleFiles(const QString &relpathExp) const
{
    // Split regular expression.
    QVector<QRegExp> expressions;
    {
        QString exp;
        bool    escapeFlag = false;
        for (auto &c : relpathExp) {
            if (c == '/' && ! escapeFlag) {
                expressions.push_back(
                    QRegExp(exp, Qt::CaseSensitivity::CaseInsensitive));
                exp = "";
            } else {
                exp.push_back(c);
                if (c == '\\' && ! escapeFlag) {
                    escapeFlag = true;
                } else {
                    escapeFlag = false;
                }
            }
        }
        expressions.push_back(
            QRegExp(exp, Qt::CaseSensitivity::CaseInsensitive));
    }
    if (expressions.size() == 0) {
        return {};
    }

    // Match.
    QMap<QString, QVector<QString>> ret;
    ::std::function<void(QVector<QRegExp> &, QVector<QRegExp>::iterator,
                         ::std::shared_ptr<GameVFS::DirReader>)>
        searchFunc([&](QVector<QRegExp> &                    expressions,
                       QVector<QRegExp>::iterator            expIter,
                       ::std::shared_ptr<GameVFS::DirReader> dir) -> void {
            auto nextExpIter = expIter;
            ++nextExpIter;
            if (nextExpIter == expressions.end()) {
                // Search file.
                for (auto fileIter = dir->begin(); fileIter != dir->end();
                     ++fileIter) {
                    if (fileIter->type == ::GameVFS::DirReader::EntryType::File
                        && expIter->exactMatch(fileIter->name)) {
                        QString lwrName    = fileIter->name.toLower();
                        auto    resultIter = ret.find(lwrName);
                        if (resultIter == ret.end()) {
                            ret[lwrName] = {};
                            resultIter   = ret.find(fileIter->name);
                        }
                        resultIter->push_back(dir->absPath(fileIter->name));
                    }
                }

            } else {
                // Search directory.
                for (auto fileIter = dir->begin(); fileIter != dir->end();
                     ++fileIter) {
                    if (fileIter->type
                            == ::GameVFS::DirReader::EntryType::Directory
                        && expIter->exactMatch(fileIter->name)) {
                        auto nextDir
                            = m_vfs->openDir(dir->absPath(fileIter->name));
                        searchFunc(expressions, nextExpIter, nextDir);
                    }
                }
            }
        });

    for (const QString &id : m_moduleLoadOrder) {
        auto moduleInfo = m_gameModules[id];

        // Search directory.
        auto dir = m_vfs->openDir(moduleInfo->path);
        searchFunc(expressions, expressions.begin(), dir);
    }

    return ret;
}

/**
 * @brief		Check path of game.
 */
bool GameData::checkGamePath(const QString &                      path,
                             QMap<QString, GameVFS::CatFileInfo> &catFiles)
{
    // Prepare
    QDir                                dir(path);
    bool                                execFound = false;
    QMap<QString, GameVFS::CatFileInfo> catsFound;
    qDebug() << "Checking game path...";

    // List files.
    // Filters
    QRegExp execFilter("x4|x4\\.exe");
    execFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp catFilter("\\d+\\.cat");
    catFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp extCatFilter("ext_\\d+\\.cat");
    extCatFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp datFilter("\\d+\\.dat");
    datFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp extDatFilter("ext_\\d+\\.dat");
    extDatFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    // Main
    for (auto &f :
         dir.entryInfoList(QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
        if (execFilter.exactMatch(f.fileName()) && ! f.isDir()) {
            qDebug() << "Game executable file :" << f.fileName() << ".";
            execFound = true;
        } else if (catFilter.exactMatch(f.fileName()) && ! f.isDir()) {
            qDebug() << "Found cat file file :" << f.fileName() << ".";
            QString key  = f.fileName().left(f.fileName().size() - 4);
            auto    iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].cat = f.fileName();
        } else if (datFilter.exactMatch(f.fileName()) && ! f.isDir()) {
            qDebug() << "Found dat file file :" << f.fileName() << ".";
            QString key  = f.fileName().left(f.fileName().size() - 4);
            auto    iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].dat = f.fileName();
        } else if (f.fileName() == "extensions" && f.isDir()) {
            // Extensions
            QDir extensionsDir(f.absoluteFilePath());
            for (auto &modEntry : extensionsDir.entryInfoList(
                     QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
                if (modEntry.isDir() && modEntry.fileName() != "."
                    && modEntry.fileName() != "..") {
                    QDir modDir(modEntry.absoluteFilePath());
                    for (auto &modFile : modDir.entryInfoList(
                             QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
                        if (extCatFilter.exactMatch(modFile.fileName())
                            && ! modFile.isDir()) {
                            QString filename = QString("extensions/%1/%2")
                                                   .arg(modEntry.fileName())
                                                   .arg(modFile.fileName());
                            qDebug()
                                << "Found cat file file :" << filename << ".";
                            QString key  = filename.left(filename.size() - 4);
                            auto    iter = catsFound.find(key);
                            if (iter == catsFound.end()) {
                                catsFound[key] = GameVFS::CatFileInfo();
                            }
                            catsFound[key].cat = filename;
                        } else if (extDatFilter.exactMatch(modFile.fileName())
                                   && ! modFile.isDir()) {
                            QString filename = QString("extensions/%1/%2")
                                                   .arg(modEntry.fileName())
                                                   .arg(modFile.fileName());
                            qDebug()
                                << "Found dat file file :" << filename << ".";
                            QString key  = filename.left(filename.size() - 4);
                            auto    iter = catsFound.find(key);
                            if (iter == catsFound.end()) {
                                catsFound[key] = GameVFS::CatFileInfo();
                            }
                            catsFound[key].dat = filename;
                        }
                    }
                }
            }
        }
    }

    // Check result
    if (! execFound) {
        qDebug() << "Missing main program.";
        qDebug() << "Failed.";
        return false;
    }

    if (catsFound.size() < MIN_CAT_FILE_NUM) {
        qDebug() << "Number of cat files si not enough.";
        qDebug() << "Failed.";
        return false;
    }

    for (auto &key : catsFound.keys()) {
        if (catsFound[key].cat == "" || catsFound[key].dat == "") {
            catsFound.remove(key);
        }
    }

    qDebug() << "OK.";
    catFiles = ::std::move(catsFound);
    return true;
}

/**
 * @brief		Ask game path.
 */
bool GameData::askGamePath()
{
    QFileDialog fileDialog(nullptr, STR("STR_TITLE_SELECT_GAME_PATH"),
                           m_gamePath, "*");
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::FileMode::Directory);
    fileDialog.setFilter(QDir::Filter::Dirs | QDir::Filter::Hidden
                         | QDir::Filter::System);
    if (fileDialog.exec() != QDialog::DialogCode::Accepted
        || fileDialog.selectedFiles().empty()) {
        return false;
    }
    QString str = QDir(fileDialog.selectedFiles()[0]).absolutePath();
    qDebug() << "Selected:" << str;

    m_gamePath = str;
    Config::instance()->setString("/gamePath", m_gamePath);

    return true;
}

/*
 * @brief	Get game VFS.
 */
::std::shared_ptr<GameVFS> GameData::vfs()
{
    return m_vfs;
}

/*
 * @brief	Get game texts.
 */
::std::shared_ptr<GameTexts> GameData::texts()
{
    return m_texts;
}

/*
 * @brief	Get game macros.
 */
::std::shared_ptr<GameMacros> GameData::macros()
{
    return m_macros;
}

/*
 * @brief	Get game components.
 */
::std::shared_ptr<GameComponents> GameData::components()
{
    return m_components;
}

/*
 * @brief	Get game races.
 */
::std::shared_ptr<GameRaces> GameData::races()
{
    return m_races;
}

/*
 * @brief	Get game wares
 */
::std::shared_ptr<GameWares> GameData::wares()
{
    return m_wares;
}

/*
 * @brief	Get station modules.
 */
::std::shared_ptr<GameStationModules> GameData::stationModules()
{
    return m_stationModules;
}

/**
 * @brief       Scan user paths.
 */
void GameData::scanUserPaths(SplashWidget *                       splashWidget,
                             QMap<QString, GameVFS::CatFileInfo> &catFiles)
{
#if defined(OS_WINDOWS)
    QStringList scanDirs = QStandardPaths::standardLocations(
        QStandardPaths::StandardLocation::DocumentsLocation);
#else
    QStringList scanDirs = QStandardPaths::standardLocations(
        QStandardPaths::StandardLocation::ConfigLocation);
#endif

    QMap<QString, QString> scanResult;

    // Scan directories.
    QRegExp nameExp("^\\d+$");
    for (QString &scanPath : scanDirs) {
        // Check path.
        QDir scanDir(scanPath);
        if (! scanDir.exists(USER_GAME_DIR)) {
            continue;
        }

        QString   userGameDirPath = scanDir.absoluteFilePath(USER_GAME_DIR);
        QFileInfo userGameDirInfo(userGameDirPath);
        if (! userGameDirInfo.isDir()) {
            continue;
        }
        QDir userGameDir(userGameDirPath);

        // Scan.
        for (auto &info : userGameDir.entryInfoList(QDir::Filter::Dirs)) {
            if (! nameExp.exactMatch(info.fileName())) {
                continue;
            }
            QDir dir = info.absoluteDir();
            dir.cd(info.fileName());
            if (! dir.exists("content.xml")) {
                continue;
            }
            scanResult[info.fileName()] = dir.canonicalPath();
            qDebug() << "Directory " << dir.canonicalPath() << "found.";
            break;
        }
    }

    // Select
    if (scanResult.size() <= 0) {
        return;

    } else if (scanResult.size() == 1) {
        Config::instance()->setString("/steamID3", scanResult.begin().key());
        m_userPath = scanResult.begin().value();
        qDebug() << "Directory " << m_userPath << "selected.";

    } else if (scanResult.size() > 1) {
        QString id = splashWidget->callFunc(
            ::std::function<QString()>(([&]() -> QString {
                return SelectSteamIDDialog::selectSteamIDs(scanResult.keys());
            })));

        Config::instance()->setString("/steamID3", id);
        m_userPath = scanResult[id];
        qDebug() << "Directory " << m_userPath << "selected.";
    }

    // Scan cat/dat files.
    QMap<QString, GameVFS::CatFileInfo> catsFound;

    QRegExp extCatFilter("ext_\\d+\\.cat");
    extCatFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp extDatFilter("ext_\\d+\\.dat");
    extDatFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QDir dir(m_userPath);
    if (dir.exists("extensions")) {
        // Extensions
        QDir extensionsDir(dir.absoluteFilePath("extensions"));
        for (auto &modEntry : extensionsDir.entryInfoList(
                 QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
            if (modEntry.isDir() && modEntry.fileName() != "."
                && modEntry.fileName() != "..") {
                QDir modDir(modEntry.absoluteFilePath());
                for (auto &modFile : modDir.entryInfoList(
                         QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
                    if (extCatFilter.exactMatch(modFile.fileName())
                        && ! modFile.isDir()) {
                        QString filename = QString("extensions/%1/%2")
                                               .arg(modEntry.fileName())
                                               .arg(modFile.fileName());
                        qDebug() << "Found cat file file :" << filename << ".";
                        QString key  = filename.left(filename.size() - 4);
                        auto    iter = catsFound.find(key);
                        if (iter == catsFound.end()) {
                            catsFound[key] = GameVFS::CatFileInfo();
                        }
                        catsFound[key].cat = filename;
                    } else if (extDatFilter.exactMatch(modFile.fileName())
                               && ! modFile.isDir()) {
                        QString filename = QString("extensions/%1/%2")
                                               .arg(modEntry.fileName())
                                               .arg(modFile.fileName());
                        qDebug() << "Found dat file file :" << filename << ".";
                        QString key  = filename.left(filename.size() - 4);
                        auto    iter = catsFound.find(key);
                        if (iter == catsFound.end()) {
                            catsFound[key] = GameVFS::CatFileInfo();
                        }
                        catsFound[key].dat = filename;
                    }
                }
            }
        }
    }

    for (auto &key : catsFound.keys()) {
        if (catsFound[key].cat == "" || catsFound[key].dat == "") {
            catsFound.remove(key);
        }
    }

    catFiles = ::std::move(catsFound);
}

/**
 * @brief       Get the blacklist of mods.
 */
QSet<QString> GameData::getModBlacklist()
{
    // Open content.xml.
    QDir dir(m_userPath);

    ::std::shared_ptr<GameVFS::FileReader> contentFile
        = m_vfs->open(QString("/%1").arg(CONTENT_XML));
    if (contentFile == nullptr) {
        return {};
    }

    // Load content.xml.
    XMLLoader            loader;
    XMLLoader::ErrorInfo info;
    if (! loader.loadData(contentFile->readAll(), info)) {
        qWarning() << "Failed to parse" << contentFile->path()
                   << "Line :" << info.errorLine
                   << ", col :" << info.errorColumn << ": " << info.errorMsg
                   << ".";
        return {};
    }

    // Parse.
    QSet<QString> ret;
    loader.elementLoader("/content/extension")
        ->setOnStartElement(
            [&](XMLLoader &, XMLLoader::XMLElementLoader &,
                const ::std::map<QString, QString> &attributes) -> bool {
                auto iterID      = attributes.find("id");
                auto iterEnabled = attributes.find("enabled");
                if (iterID != attributes.end()
                    && iterEnabled != attributes.end()) {
                    if (iterEnabled->second == "false") {
                        ret.insert(iterID->second);
                        qDebug() << "Mod" << iterID->second << "is disabled.";
                    }
                }
                return true;
            });

    loader.parse();

    return ret;
}

/**
 * @brief       Scan game modules.
 */
void GameData::scanGameModules()
{
    m_gameModules.clear();
    QSet<QString> blacklist = this->getModBlacklist();

    // Main program.
    // Game version.
    auto    versionFile = m_vfs->open("/version.dat");
    QString versionStr  = versionFile->readAll();
    versionStr          = versionStr.replace(QRegExp("\\s"), "");
    ::std::shared_ptr<GameModule> module = ::std::shared_ptr<GameModule>(
        new GameModule({"",
                        {{"en_US", "X4 Foundations"}},
                        {{"en_US", "X4 Foundations"}},
                        {{"en_US", "Egosoft GmbH"}},
                        (uint32_t)(versionStr.toUInt()),
                        {},
                        "/"}));
    m_gameModules[""] = module;

    // Scan modules.
    auto extensionsDir = m_vfs->openDir("/extensions");
    if (extensionsDir != nullptr) {
        for (auto iter = extensionsDir->begin(); iter != extensionsDir->end();
             ++iter) {
            if ((iter->type == GameVFS::DirReader::EntryType::Directory)
                && (iter->name != "..") && (iter->name != ".")) {
                auto contentPath = QString("/extensions/%1/%2")
                                       .arg(iter->name)
                                       .arg(CONTENT_XML);
                auto contentFile = m_vfs->open(contentPath);
                if (contentFile != nullptr) {
                    ::std::shared_ptr<GameModule> module
                        = ::std::shared_ptr<GameModule>(new GameModule());
                    module->path = QString("/extensions/%1").arg(iter->name);

                    // Initialize loader.
                    XMLLoader loader;
                    auto      elementLoader = loader.elementLoader("/content");
                    elementLoader->setOnStartElement(
                        [&](XMLLoader &, XMLLoader::XMLElementLoader &,
                            const ::std::map<QString, QString> &attributes)
                            -> bool {
                            // id.
                            auto iter = attributes.find("id");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing id in file "
                                           << contentPath << ".";
                                return false;
                            }
                            module->id = iter->second;

                            // name.
                            iter = attributes.find("name");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing name in file "
                                           << contentPath << ".";
                                return false;
                            }
                            module->name["en_US"] = iter->second;

                            // author.
                            iter = attributes.find("author");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing author in file "
                                           << contentPath << ".";
                                return false;
                            }
                            module->author["en_US"] = iter->second;

                            // version.
                            iter = attributes.find("version");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing version in file "
                                           << contentPath << ".";
                                return false;
                            }
                            module->version = iter->second.toUInt();

                            // description.
                            iter = attributes.find("description");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing description in file "
                                           << contentPath << ".";
                                return false;
                            }
                            module->description["en_US"] = iter->second;

                            return true;
                        });

                    elementLoader = loader.elementLoader("/content/text");
                    elementLoader->setOnStartElement(
                        [&](XMLLoader &, XMLLoader::XMLElementLoader &,
                            const ::std::map<QString, QString> &attributes)
                            -> bool {
                            // launguage.
                            auto iter = attributes.find("language");
                            if (iter == attributes.end()) {
                                qWarning() << "Missing language in file "
                                           << contentPath << ".";
                                return true;
                            }
                            QString language = StringTable::getLanugageByID(
                                iter->second.toUInt());

                            // name.
                            iter = attributes.find("name");
                            if (iter != attributes.end()) {
                                module->name[language] = iter->second;
                            }

                            // author.
                            iter = attributes.find("author");
                            if (iter != attributes.end()) {
                                module->author[language] = iter->second;
                            }

                            // description.
                            iter = attributes.find("description");
                            if (iter != attributes.end()) {
                                module->description[language] = iter->second;
                            }

                            return true;
                        });

                    elementLoader = loader.elementLoader("/content/dependency");
                    elementLoader->setOnStartElement(
                        [&](XMLLoader &, XMLLoader::XMLElementLoader &,
                            const ::std::map<QString, QString> &attributes)
                            -> bool {
                            // id.
                            auto iter = attributes.find("id");
                            if (iter == attributes.end()) {
                                return true;
                            }
                            module->dependencies.insert(iter->second);

                            return true;
                        });

                    // Parse data.
                    XMLLoader::ErrorInfo errInfo;
                    loader.loadData(contentFile->readAll(), errInfo);
                    if (loader.parse() && module->id != "") {
                        m_gameModules[module->id] = module;
                        qDebug() << "Module loaded. {";
                        qDebug() << "    id             =" << module->id << ",";
                        qDebug() << "    name           = {";
                        for (auto iter = module->name.begin();
                             iter != module->name.end(); ++iter) {
                            qDebug() << "        {" << iter.key() << ","
                                     << iter.value() << "},";
                        }
                        qDebug() << "    },";
                        qDebug() << "    author         = {";
                        for (auto iter = module->author.begin();
                             iter != module->author.end(); ++iter) {
                            qDebug() << "        {" << iter.key() << ","
                                     << iter.value() << "},";
                        }
                        qDebug() << "    },";
                        qDebug() << "    description    = {";
                        for (auto iter = module->description.begin();
                             iter != module->description.end(); ++iter) {
                            qDebug() << "        {" << iter.key() << ","
                                     << iter.value() << "},";
                        }
                        qDebug() << "    },";
                        qDebug() << "    path           ="
                                 << module->path.toStdString().c_str() << ",";
                        qDebug() << "}";
                    }
                }
            }
        }
    }
}

/**
 * @brief       Sort game modules.
 */
void GameData::sortGameModules()
{
    m_moduleLoadOrder = QVector<QString>({""});

    // Get blacklist.
    QSet<QString> blacklist = this->getModBlacklist();

    // Scan modules to load.
    QSet<QString> officialModulesToLoad;
    QSet<QString> otherModulesToLoad;
    for (auto &module : m_gameModules) {
        if (module->id != "" && blacklist.find(module->id) == blacklist.end()) {
            if (module->author["en_US"] == "Egosoft GmbH") {
                officialModulesToLoad.insert(module->id);
            } else {
                otherModulesToLoad.insert(module->id);
            }
        }
    }

    // Get load order.
    auto addToLoadList = [&](QSet<QString> &modules) -> void {
        while (! modules.empty()) {
            for (auto iter = modules.begin(); iter != modules.end();) {
                auto currentIter = iter;
                ++iter;
                auto module  = m_gameModules[*currentIter];
                bool cleared = true;
                for (auto &dep : module->dependencies) {
                    if (modules.find(dep) != modules.end()) {
                        cleared = false;
                        break;
                    }
                }
                if (cleared) {
                    m_moduleLoadOrder.push_back(module->id);
                    modules.erase(currentIter);
                }
            }
        }
    };
    addToLoadList(officialModulesToLoad);
    addToLoadList(otherModulesToLoad);
    qDebug() << "Load order of modules : {";
    for (auto &id : m_moduleLoadOrder) {
        auto           module   = m_gameModules[id];
        const QString &language = StringTable::instance()->language();
        auto           iter     = module->name.find(language);
        if (iter == module->name.end()) {
            iter = module->name.find("en_US");
        }
        qDebug() << "    {" << (module->id) << "," << (*iter) << "},";
    }
    qDebug() << "}";
}
