#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

#include <common.h>
#include <game_data/game_text.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	vfs				Virtual filesystem of the game.
 * @param[in]	setTextFunc		Callback to set text.
 */
GameText::GameText(::std::shared_ptr<GameVFS>             vfs,
                   ::std::function<void(const QString &)> setTextFunc)
{
    ::std::shared_ptr<GameVFS::DirReader> dirReader = vfs->openDir("t");

    /// Search file
    auto                   allFileIter = dirReader->begin();
    QMutex                 allFileIterLock;
    ::std::atomic<quint64> finishedCount;
    size_t                 total = dirReader->count();
    finishedCount                = 0;
    MultiRun loadTask(::std::function<void()>([&]() -> void {
        GameVFS::DirReader::iterator fileIter;
        while (true) {
            /// Get file
            {
                QMutexLocker locker(&allFileIterLock);
                if (allFileIter == dirReader->end()) {
                    return;
                } else {
                    fileIter = allFileIter;
                    allFileIter += 1;
                }
            }

            QRegExp nameFilter("\\d+-L\\d+\\.xml");
            nameFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
            if (fileIter->type == GameVFS::DirReader::EntryType::File
                && nameFilter.exactMatch(fileIter->name)) {
                /// Load file
                qDebug() << "Loading file" << fileIter->name << ".";

                /// Open
                ::std::shared_ptr<GameVFS::FileReader> fileReader
                    = vfs->open(dirReader->absPath(fileIter->name));

                /// Parse xml
                QXmlStreamReader reader(fileReader->readAll());
                if (! reader.readNextStartElement()
                    || reader.name() != "language") {
                    goto END_PARSE;
                }

                QXmlStreamAttributes attributes = reader.attributes();
                if (! attributes.hasAttribute("id")) {
                    goto END_PARSE;
                }
                this->readPage(reader, attributes.value("id").toInt());
            }

        END_PARSE:
            finishedCount += 1;
            setTextFunc(
                STR("STR_LOADING_TEXT_FILE").arg(finishedCount).arg(total));
        }
    }));

    setTextFunc(STR("STR_LOADING_TEXT_FILE").arg(finishedCount).arg(total));
    loadTask.run();

    this->setGood();
}

/**
 * @brief		Get text.
 *
 * @param[in]	pageID		Page ID of the text.
 * @param[in]	textID		ID of the text.
 *
 * @return		Text.
 */
QString GameText::text(qint32 pageID, qint32 textID)
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
        return "";
    }

    for (auto &link : *linkIter) {
        if (link.isRef) {
            ret.append(this->text(link.refInfo.pageID, link.refInfo.textId));
        } else {
            ret.append(link.text);
        }
    }

    return ret;
}

/**
 * @brief		Destructor.
 */
GameText::~GameText() {}

/**
 * @brief		Read text pages.
 *
 * @param[in]	reader		XML reader.
 * @param[in]	languageID	Language ID of the text.
 */
void GameText::readPage(QXmlStreamReader &reader, quint32 languageID)
{
    quint64 depth = 0;
    while (! reader.atEnd()) {
        switch (reader.readNext()) {
            case QXmlStreamReader::TokenType::StartElement:
                if (depth == 0) {
                    if (reader.name() == "page") {
                        /// Parse page node
                        QXmlStreamAttributes attributes = reader.attributes();
                        if (! attributes.hasAttribute("id")) {
                            depth += 1;
                            break;
                        }
                        qint32 id = attributes.value("id").toInt();

                        /// Get page
                        ::std::shared_ptr<TextPage> page;
                        {
                            QMutexLocker locker(&m_pageLock);
                            auto         pageIter = m_textPages.find(id);
                            if (pageIter == m_textPages.end()) {
                                page
                                    = ::std::shared_ptr<TextPage>(new TextPage);
                                page->pageID    = id;
                                m_textPages[id] = page;
                            } else {
                                page = *pageIter;
                            }
                        }

                        /// Read texts
                        this->readText(reader, languageID, page);
                    } else {
                        depth += 1;
                    }
                } else {
                    depth += 1;
                }
                break;

            case QXmlStreamReader::TokenType::EndElement:
                if (depth == 0) {
                    return;
                } else {
                    depth -= 1;
                }
                break;

            case QXmlStreamReader::TokenType::NoToken:
            case QXmlStreamReader::TokenType::Invalid:
            case QXmlStreamReader::TokenType::EndDocument:
                return;

            default:
                break;
        }
    }
}

/**
 * @brief		Read texts in the page.
 *
 * @param[in]	reader		XML reader.
 * @param[in]	languageID	Language ID of the text.
 * @param[in]	page		Page of the text.
 */
void GameText::readText(QXmlStreamReader &          reader,
                        quint32                     languageID,
                        ::std::shared_ptr<TextPage> page)
{
    quint64                 depth = 0;
    ::std::shared_ptr<Text> text;
    while (! reader.atEnd()) {
        switch (reader.readNext()) {
            case QXmlStreamReader::TokenType::StartElement:
                if (depth == 0) {
                    if (reader.name() == "t") {
                        /// Parse text node
                        QXmlStreamAttributes attributes = reader.attributes();
                        if (! attributes.hasAttribute("id")) {
                            depth += 1;
                            break;
                        }
                        qint32 id = attributes.value("id").toInt();

                        /// Get text
                        QMutexLocker locket(&(page->lock));
                        auto         textIter = page->texts.find(id);
                        if (textIter == page->texts.end()) {
                            text            = ::std::shared_ptr<Text>(new Text);
                            text->pageID    = page->pageID;
                            text->textId    = id;
                            page->texts[id] = text;
                        } else {
                            text = *textIter;
                        }
                    }
                } else {
                    text = nullptr;
                }
                depth += 1;
                break;

            case QXmlStreamReader::TokenType::EndElement:
                if (depth == 0) {
                    return;
                } else {
                    text = nullptr;
                    depth -= 1;
                }
                break;

            case QXmlStreamReader::TokenType::Characters:
                if (text != nullptr) {
                    /// Read and parse text.
                    QVector<GameText::TextLink> links
                        = this->parseText(reader.text().toString());

                    /// Set text.
                    QMutexLocker locker(&(text->lock));
                    text->links[languageID] = ::std::move(links);
                }
                break;

            case QXmlStreamReader::TokenType::NoToken:
            case QXmlStreamReader::TokenType::Invalid:
            case QXmlStreamReader::TokenType::EndDocument:
                return;

            default:
                break;
        }
    }
}

/**
 * @brief		Parse text.
 *
 * @return		Parsed text.
 */
QVector<GameText::TextLink> GameText::parseText(QString s)
{
    QVector<GameText::TextLink> ret;
    TextLink                    link;
    QRegExp                     ignoreExp("\\(.*\\)");
    QRegExp                     whiteSpacewExp("\\s");
    QRegExp referenceExp("\\{\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\}");
    QRegExp numExp("\\d+");

    s = s.replace(ignoreExp, "");
    while (s != "") {
        int index = referenceExp.indexIn(s);
        if (index == -1) {
            /// No reference exists.
            link.isRef          = false;
            link.text           = s;
            link.refInfo.pageID = 0;
            link.refInfo.textId = 0;
            ret.append(link);
            break;
        } else {
            /// Reference found.
            if (index > 0) {
                /// Before
                link.isRef          = false;
                link.text           = s.left(index);
                link.refInfo.pageID = 0;
                link.refInfo.textId = 0;
                ret.append(link);
            }

            /// Reference
            link.isRef          = true;
            link.text           = "";
            link.refInfo.pageID = referenceExp.cap(1).toInt();
            link.refInfo.textId = referenceExp.cap(2).toInt();
            ret.append(link);

            /// After
            s = s.mid(index + referenceExp.cap().size());
        }
    }

    return ret;
}
