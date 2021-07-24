#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_data.h>
#include <game_data/game_texts.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameTexts::GameTexts(GameData *                             gameData,
                     ::std::function<void(const QString &)> setTextFunc) :
    m_unknowIndex(0)
{
    auto vfs         = gameData->vfs();
    auto gameModules = gameData->gameModules();
    auto loadOrder   = gameData->moduleLoadOrder();

    // Scan files to load.
    QRegExp nameFilter("\\d+-L\\d+\\.xml");
    nameFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QMap<QString, QVector<QString>> xmlFiles;

    for (const QString &id : loadOrder) {
        auto moduleInfo = gameModules[id];
        // Search directory.
        ::std::shared_ptr<::GameVFS::DirReader> dir
            = vfs->openDir(QString("%1/t").arg(moduleInfo->path));
        if (dir != nullptr) {
            for (auto iter = dir->begin(); iter != dir->end(); ++iter) {
                if (iter->type == ::GameVFS::DirReader::EntryType::File
                    && nameFilter.exactMatch(iter->name)) {
                    QString lwrName  = iter->name.toLower();
                    auto    pathIter = xmlFiles.find(lwrName);
                    if (pathIter == xmlFiles.end()) {
                        xmlFiles[lwrName] = {};
                        pathIter          = xmlFiles.find(iter->name);
                    }
                    pathIter->push_back(dir->absPath(iter->name));
                }
            }
        }
    }

    // Load.
    QMutex                taskLock;
    auto                  taskIter = xmlFiles.begin();
    ::std::atomic<qint64> finishedCount;
    volatile qint64       printedCount;
    size_t                total = static_cast<size_t>(xmlFiles.count());
    finishedCount               = 0;
    printedCount                = -1;
    MultiRun loadTask(::std::function<void()>([&]() -> void {
        // Create loader.
        ::std::unique_ptr<XMLLoader> loader = this->createXMLLoader();
        XMLLoader::ErrorInfo         errInfo;
        QMap<QString, QVector<QString>>::iterator currentTask;

        while (true) {
            {
                QMutexLocker locker(&taskLock);

                // Print.
                if (printedCount < finishedCount) {
                    setTextFunc(STR("STR_LOADING_TEXT_FILE")
                                    .arg(finishedCount)
                                    .arg(total));
                    printedCount = finishedCount;
                }

                // Get task.
                if (taskIter == xmlFiles.end()) {
                    return;
                }
                currentTask = taskIter;
                ++taskIter;
            }

            // Load xml.
            loader->clearData();
            auto fileIter = currentTask->begin();
            qDebug() << "Loading file" << (*fileIter) << ".";
            if (! loader->loadData(vfs->open(*fileIter)->readAll(), errInfo)) {
                qWarning() << QString("Failed to load XML file \"%1\", line: "
                                      "%2, col: %3: %4.")
                                  .arg(*fileIter)
                                  .arg(errInfo.errorLine)
                                  .arg(errInfo.errorColumn)
                                  .arg(errInfo.errorMsg);
                ++finishedCount;
                continue;
            }
            for (++fileIter; fileIter != currentTask->end(); ++fileIter) {
                qDebug() << "Loading patch" << (*fileIter) << ".";
                if (! loader->loadPatch(vfs->open(*fileIter)->readAll(),
                                        errInfo)) {
                    qWarning()
                        << QString("Failed to load XML file \"%1\", line: "
                                   "%2, col: %3: %4.")
                               .arg(*fileIter)
                               .arg(errInfo.errorLine)
                               .arg(errInfo.errorColumn)
                               .arg(errInfo.errorMsg);
                    continue;
                }
            }

            // Parse.
            qDebug() << "Parsing file" << currentTask.key() << ".";
            if (! loader->parse()) {
                qWarning() << QString("Failed to parse file \"%1\".")
                                  .arg(currentTask.key());
            }
            ++finishedCount;
        }
    }));

    loadTask.run();

    this->setInitialized();
}

/**
 * @brief		Get text.
 */
QString GameTexts::text(qint32 pageID, qint32 textID)
{
    QString ret      = "";
    auto    pageIter = m_textPages.find(pageID);
    if (pageIter == m_textPages.end()) {
        return "";
    }
    auto textIter = (*pageIter)->texts.find(textID);
    if (textIter == (*pageIter)->texts.end()) {
        return "";
    }

    auto linkIter
        = (*textIter)->links.find(StringTable::instance()->languageId());
    if (linkIter == (*textIter)->links.end()) {
        linkIter = (*textIter)->links.find(44);
        if (linkIter == (*textIter)->links.end()) {
            return "";
        }
    }

    for (auto &link : *linkIter) {
        if (link.isRef) {
            ret.append(this->text(link.refInfo.pageID, link.refInfo.textID));
        } else {
            ret.append(link.text);
        }
    }

    return ret;
}

/**
 * @brief		Get text.
 */
QString GameTexts::text(const IDPair &idPair)
{
    return this->text(idPair.pageID, idPair.textID);
}

/**
 * @brief		Add text.
 */
GameTexts::IDPair GameTexts::addText(const QString &str)
{
    // Get page.
    ::std::shared_ptr<TextPage> page;
    {
        QMutexLocker lock(&m_pageLock);
        auto         pageIter = m_textPages.find(-1);
        if (pageIter == m_textPages.end()) {
            page            = ::std::shared_ptr<TextPage>(new TextPage);
            page->pageID    = -1;
            m_textPages[-1] = page;
        } else {
            page = *pageIter;
        }
    }

    // set text;
    qint32 id = m_unknowIndex.fetchAndAddAcquire(1);
    {
        QMutexLocker            locker(&(page->lock));
        ::std::shared_ptr<Text> text;
        text            = ::std::shared_ptr<Text>(new Text);
        text->pageID    = page->pageID;
        text->textID    = id;
        text->links[44] = this->parseText(str);
        page->texts[id] = text;
    }

    return IDPair(static_cast<qint32>(-1), id);
}

/**
 * @brief		Destructor.
 */
GameTexts::~GameTexts() {}

/**
 * @brief		Parse text.
 */
QVector<GameTexts::TextLink> GameTexts::parseText(QString s)
{
    QVector<GameTexts::TextLink> ret;
    TextLink                     link;
    QRegExp                      ignoreExp("\\(.*\\)");
    QRegExp                      whiteSpacewExp("\\s");
    QRegExp referenceExp("\\{\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\}");
    QRegExp numExp("\\d+");

    s = s.replace(ignoreExp, "");
    while (s != "") {
        int index = referenceExp.indexIn(s);
        if (index == -1) {
            // No reference exists.
            link.isRef          = false;
            link.text           = this->parseEscape(s);
            link.refInfo.pageID = 0;
            link.refInfo.textID = 0;
            ret.append(link);
            break;
        } else {
            // Reference found.
            if (index > 0) {
                // Before
                link.isRef          = false;
                link.text           = this->parseEscape(s.left(index));
                link.refInfo.pageID = 0;
                link.refInfo.textID = 0;
                ret.append(link);
            }

            // Reference
            link.isRef          = true;
            link.text           = "";
            link.refInfo.pageID = referenceExp.cap(1).toInt();
            link.refInfo.textID = referenceExp.cap(2).toInt();
            ret.append(link);

            // After
            s = s.mid(index + referenceExp.cap().size());
        }
    }

    return ret;
}

/**
 * @brief		Parse excape characters.
 */
QString GameTexts::parseEscape(const QString &s)
{
    QString ret = "";
    for (auto iter = s.begin(); iter < s.end(); iter++) {
        if (*iter == '\\') {
            ++iter;
            if (iter == s.end()) {
                continue;
            }
            // Parse escape characters.
            switch (iter->unicode()) {
                case 'n':
                    // \n
                    ret.append('\n');
                    break;

                case 'r':
                    // \r
                    ret.append('\r');
                    break;

                case 't':
                    // \t
                    ret.append('\t');
                    break;

                case 'v':
                    // \v
                    ret.append('\v');
                    break;

                case 'a':
                    // \a
                    ret.append('\a');
                    break;

                case 'b':
                    // \b
                    ret.append('\b');
                    break;

                case 'f':
                    // \f
                    ret.append('\f');
                    break;

                case 'x':
                    // \xhh
                    {
                        ushort n = 0;
                        if (iter + 1 != s.end()
                            && between((iter + 1)->unicode(), '0', '9')) {
                            n = n * 0x10 + (iter->unicode() - '0');
                        } else if (iter + 1 != s.end()
                                   && between((iter + 1)->unicode(), 'a',
                                              'f')) {
                            n = n * 0x10 + (iter->unicode() - 'a');
                        } else if (iter + 1 != s.end()
                                   && between((iter + 1)->unicode(), 'A',
                                              'F')) {
                            n = n * 0x10 + (iter->unicode() - 'A');
                        } else {
                            break;
                        }

                        ++iter;
                        if (iter == s.end()) {
                            continue;
                        }

                        if (iter + 1 != s.end()
                            && between((iter + 1)->unicode(), '0', '9')) {
                            n = n * 0x10 + (iter->unicode() - '0');
                        } else if (iter + 1 != s.end()
                                   && between((iter + 1)->unicode(), 'a',
                                              'f')) {
                            n = n * 0x10 + (iter->unicode() - 'a');
                        } else if (iter + 1 != s.end()
                                   && between((iter + 1)->unicode(), 'A',
                                              'F')) {
                            n = n * 0x10 + (iter->unicode() - 'A');
                        }
                        ret.append(QChar(n));
                    }
                    break;

                case '0':
                    // \0
                    if ((iter + 1) == s.end() || ! (iter + 1)->isDigit()) {
                        ret.append('\0');
                        break;
                    }

                default:
                    if (iter->isDigit()) {
                        // \ddd
                        ushort n = iter->digitValue();
                        if (iter + 1 != s.end() && (iter + 1)->isDigit()) {
                            ++iter;
                            n = n * 010 + iter->digitValue();
                        } else {
                            ret.append(QChar(n));
                            break;
                        }
                        if (iter + 1 != s.end() && (iter + 1)->isDigit()) {
                            ++iter;
                            n = n * 010 + iter->digitValue();
                        }
                        ret.append(QChar(n));

                        break;
                    } else {
                        ret.append(*iter);
                    }
            }
        } else {
            // Copy character.
            ret.append(*iter);
        }
    }

    return ret;
}

/**
 * @brief       Create XML loader.
 */
::std::unique_ptr<XMLLoader> GameTexts::createXMLLoader()
{
    ::std::unique_ptr<XMLLoader> ret(new XMLLoader);

    // /language
    XMLLoader::XMLElementLoader *elementLoader
        = ret->elementLoader("/language");
    elementLoader->setOnStartElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &,
           const ::std::map<QString, QString> &attributes) -> bool {
            auto attrIter = attributes.find("id");
            if (attrIter == attributes.end()) {
                qWarning()
                    << "Missing attribute \"id\" in element \"/language\".";
                return false;
            }

            xmlLoader.values()["languageID"]
                = static_cast<quint32>(attrIter->second.toUInt());

            return true;
        });

    // /language/page
    elementLoader = ret->elementLoader("/language/page");
    elementLoader->setOnStartElement([this](XMLLoader &xmlLoader,
                                            XMLLoader::XMLElementLoader &,
                                            const ::std::map<QString, QString>
                                                &attributes) -> bool {
        auto attrIter = attributes.find("id");
        if (attrIter == attributes.end()) {
            qWarning()
                << "Missing attribute \"id\" in element \"/language/page\".";
            xmlLoader.values()["page"] = ::std::shared_ptr<TextPage>(nullptr);
            return true;
        }

        qint32 pageID = attrIter->second.toInt();

        // Get page
        ::std::shared_ptr<TextPage> page;
        {
            QMutexLocker locker(&m_pageLock);
            auto         pageIter = m_textPages.find(pageID);
            if (pageIter == m_textPages.end()) {
                page                = ::std::shared_ptr<TextPage>(new TextPage);
                page->pageID        = pageID;
                m_textPages[pageID] = page;
            } else {
                page = *pageIter;
            }
        }

        xmlLoader.values()["page"] = page;

        return true;
    });
    elementLoader->setOnStopElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &) -> bool {
            xmlLoader.values()["page"] = ::std::shared_ptr<TextPage>(nullptr);
            return true;
        });

    // /language/page/t
    elementLoader = ret->elementLoader("/language/page/t");
    elementLoader->setOnStartElement([](XMLLoader &xmlLoader,
                                        XMLLoader::XMLElementLoader &,
                                        const ::std::map<QString, QString>
                                            &attributes) -> bool {
        ::std::shared_ptr<TextPage> page
            = ::std::any_cast<::std::shared_ptr<TextPage>>(
                xmlLoader.values()["page"]);

        if (page == nullptr) {
            return true;
        }

        auto attrIter = attributes.find("id");
        if (attrIter == attributes.end()) {
            qWarning()
                << "Missing attribute \"id\" in element \"/language/page/t\".";
            xmlLoader.values()["text"] = ::std::shared_ptr<Text>(nullptr);
            return true;
        }

        qint32 id = attrIter->second.toInt();

        // Get text
        QMutexLocker            locker(&(page->lock));
        ::std::shared_ptr<Text> text;
        auto                    textIter = page->texts.find(id);
        if (textIter == page->texts.end()) {
            text            = ::std::shared_ptr<Text>(new Text);
            text->pageID    = page->pageID;
            text->textID    = id;
            page->texts[id] = text;
        } else {
            text = *textIter;
        }

        xmlLoader.values()["text"] = text;

        return true;
    });
    elementLoader->setOnElementText([this](XMLLoader &xmlLoader,
                                           XMLLoader::XMLElementLoader &,
                                           const QString &s) -> bool {
        ::std::shared_ptr<TextPage> page
            = ::std::any_cast<::std::shared_ptr<TextPage>>(
                xmlLoader.values()["page"]);
        ::std::shared_ptr<Text> text = ::std::any_cast<::std::shared_ptr<Text>>(
            xmlLoader.values()["text"]);
        quint32 languageID
            = ::std::any_cast<quint32>(xmlLoader.values()["languageID"]);

        if (page != nullptr && text != nullptr) {
            // Read and parse text.
            QVector<GameTexts::TextLink> links = this->parseText(s);

            // Set text.
            QMutexLocker locker(&(text->lock));
            text->links[languageID] = ::std::move(links);
        }

        return true;
    });
    elementLoader->setOnStopElement(
        [](XMLLoader &xmlLoader, XMLLoader::XMLElementLoader &) -> bool {
            xmlLoader.values()["text"] = ::std::shared_ptr<Text>(nullptr);
            return true;
        });

    return ret;
}
