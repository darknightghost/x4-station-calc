#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <interface/i_load_factory_func.h>
#include <locale/string_table.h>

class GameVFS;

/**
 * @brief	Text in game.
 */
class GameText :
    public ILoadFactoryFunc<GameText(::std::shared_ptr<GameVFS>,
                                     ::std::function<void(const QString &)>)> {
    LOAD_FUNC(GameText,
              ::std::shared_ptr<GameVFS>,
              ::std::function<void(const QString &)>)
  private:
    /**
     * @brief	Text link.
     */
    struct TextLink {
        bool    isRef; //< True if the link is a reference.
        QString text;  //< String of the link if it is not a reference link.
        struct {
            qint32 pageID; //< ID of referenced page.
            qint32 textId; //< ID of referenced text.
        } refInfo;         /*< Information of referenced string if the link is
                            *  a reference.
                            */
    };

    /**
     * @brief	Text link.
     */
    struct Text {
        qint32                           pageID; //< ID of page.
        qint32                           textId; //< ID of text.
        QMutex                           lock;   //< Lock.
        QMap<quint32, QVector<TextLink>> links;  //< Links.
    };

    /**
     * @brief	Text page.
     */
    struct TextPage {
        qint32                                pageID; //< ID of page.
        QMap<qint32, ::std::shared_ptr<Text>> texts;  //< Texts.
        QMutex                                lock;   //< Lock.
    };

  private:
    QMap<qint32, ::std::shared_ptr<TextPage>> m_textPages; //< Text pages.
    QMutex                                    m_pageLock;  //< Test page lock.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameText(::std::shared_ptr<GameVFS>             vfs,
             ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief		Get text.
     *
     * @param[in]	pageID		Page ID of the text.
     * @param[in]	textID		ID of the text.
     *
     * @return		Text.
     */
    QString text(qint32 pageID, qint32 textID);

    /**
     * @brief		Destructor.
     */
    virtual ~GameText();

  private:
    /**
     * @brief		Read text pages.
     *
     * @param[in]	reader		XML reader.
     * @param[in]	languageID	Language ID of the text.
     */
    void readPage(QXmlStreamReader &reader, quint32 languageID);

    /**
     * @brief		Read texts in the page.
     *
     * @param[in]	reader		XML reader.
     * @param[in]	languageID	Language ID of the text.
     * @param[in]	page		Page of the text.
     */
    void readText(QXmlStreamReader &          reader,
                  quint32                     languageID,
                  ::std::shared_ptr<TextPage> page);

    /**
     * @brief		Parse text.
     *
     * @return		Parsed text.
     */
    QVector<TextLink> parseText(QString s);
};

#include <game_data/game_vfs.h>
