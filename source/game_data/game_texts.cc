#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_texts.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameTexts::GameTexts(::std::shared_ptr<GameVFS>             vfs,
                     ::std::function<void(const QString &)> setTextFunc) :
    m_unknowIndex(0)
{
    QStringList textFiles;
    QRegExp     nameFilter("\\d+-L\\d+\\.xml");
    nameFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    // Master files
    ::std::shared_ptr<GameVFS::DirReader> dirReader = vfs->openDir("t");
    for (auto iter = dirReader->begin(); iter != dirReader->end(); ++iter) {
        if (iter->type == ::GameVFS::DirReader::EntryType::File
            && nameFilter.exactMatch(iter->name)) {
            textFiles.append(dirReader->absPath(iter->name));
        }
    }

    // Extension files
    ::std::shared_ptr<::GameVFS::DirReader> extensionsDir
        = vfs->openDir("/extensions");
    if (extensionsDir != nullptr) {
        for (auto iter = extensionsDir->begin(); iter != extensionsDir->end();
             ++iter) {
            if (iter->type == ::GameVFS::DirReader::EntryType::Directory) {
                ::std::shared_ptr<GameVFS::DirReader> dirReader
                    = vfs->openDir(QString("/extensions/%1/t").arg(iter->name));
                if (dirReader == nullptr) {
                    continue;
                }
                for (auto iter = dirReader->begin(); iter != dirReader->end();
                     ++iter) {
                    if (iter->type == ::GameVFS::DirReader::EntryType::File
                        && nameFilter.exactMatch(iter->name)) {
                        textFiles.append(dirReader->absPath(iter->name));
                    }
                }
            }
        }
    }

    // Search file
    auto                   allFileIter = textFiles.begin();
    QMutex                 allFileIterLock;
    ::std::atomic<quint64> finishedCount;
    size_t                 total = textFiles.count();
    finishedCount                = 0;
    MultiRun loadTask(::std::function<void()>([&]() -> void {
        QStringList::iterator fileIter;
        while (true) {
            // Get file
            {
                QMutexLocker locker(&allFileIterLock);
                if (allFileIter == textFiles.end()) {
                    return;
                } else {
                    fileIter = allFileIter;
                    allFileIter += 1;
                }
            }

            // Load file
            qDebug() << "Loading file" << *fileIter << ".";

            // Open
            ::std::shared_ptr<GameVFS::FileReader> fileReader
                = vfs->open(*fileIter);

            // Parse xml
            /*
            QXmlStreamReader                      reader(fileReader->readAll());
            XMLLoader                             loader;
            ::std::unique_ptr<XMLLoader::Context> context
                = XMLLoader::Context::create();
            context->setOnStartElement(
                ::std::bind(&GameTexts::onStartElementInRoot, this,
                            ::std::placeholders::_1, ::std::placeholders::_2,
                            ::std::placeholders::_3, ::std::placeholders::_4));
            loader.parse(reader, ::std::move(context));

            finishedCount += 1;
            setTextFunc(
                STR("STR_LOADING_TEXT_FILE").arg(finishedCount).arg(total));
                */
        }
    }));

    setTextFunc(STR("STR_LOADING_TEXT_FILE").arg(finishedCount).arg(total));
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
