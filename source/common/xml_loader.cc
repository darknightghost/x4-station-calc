#include <common/xml_loader.h>

/**
 * @brief	Constructor.
 */
XMLLoader::XMLLoader() {}

/**
 * @brief		Push context.
 *
 * @param[in]	context			Context to push.
 */
void XMLLoader::pushContext(::std::unique_ptr<Context> context)
{
    m_contextStack.push_back(::std::move(context));
}

/**
 * @brief	Parse XML.
 *
 * @param[in]	read			XML SAX reader..
 * @param[in]	context			First context.
 */
void XMLLoader::parse(QXmlStreamReader &         reader,
                      ::std::unique_ptr<Context> context)
{
    // Push first context
    m_contextStack.clear();
    m_contextStack.push_back(::std::move(context));

    // Parse file
    while (! (m_contextStack.empty() || reader.atEnd())) {
        QXmlStreamReader::TokenType type = reader.readNext();

        switch (type) {
            case QXmlStreamReader::TokenType::StartDocument:
                m_contextStack.back()->onStartDocument(*this,
                                                       *m_contextStack.back());
                break;

            case QXmlStreamReader::TokenType::EndDocument:
                m_contextStack.back()->onStopDocument(*this,
                                                      *m_contextStack.back());
                break;

            case QXmlStreamReader::TokenType::StartElement: {
                // Name.
                QString name = reader.name().toString();

                // Attributes.
                QMap<QString, QString> attributes;
                for (auto attr : reader.attributes()) {
                    attributes[attr.qualifiedName().toString()]
                        = attr.value().toString();
                }

                // Call callback.
                m_contextStack.back()->onStartElement(
                    *this, *m_contextStack.back(), name, attributes);

            } break;

            case QXmlStreamReader::TokenType::EndElement: {
                // Name.
                QString name = reader.name().toString();

                // Call callback.
                m_contextStack.back()->onStopElement(
                    *this, *m_contextStack.back(), name);
            } break;

            case QXmlStreamReader::TokenType::Characters:
                m_contextStack.back()->onCharacters(
                    *this, *m_contextStack.back(), reader.text().toString());
                break;

            default:
                // Ignore other types.
                break;
        }
    }
}

/**
 * @brief	Destructor.
 */
XMLLoader::~XMLLoader() {}
