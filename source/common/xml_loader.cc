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
 * @param[in]	reader   		XML SAX reader..
 * @param[in]	context			First context.
 *
 * @return		Returns \c true if the whole file has been parsed,
 *				otherwise returns false.
 */
bool XMLLoader::parse(QXmlStreamReader &         reader,
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
                if (! m_contextStack.back()->onStartDocument(
                        *this, *m_contextStack.back())) {
                    return false;
                }
                break;

            case QXmlStreamReader::TokenType::EndDocument:
                if (! m_contextStack.back()->onStopDocument(
                        *this, *m_contextStack.back())) {
                    return false;
                }
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

                m_contextStack.back()->pushElement(name);

                // Call callback.
                if (! m_contextStack.back()->onStartElement(
                        *this, *m_contextStack.back(), name, attributes)) {
                    return false;
                }

            } break;

            case QXmlStreamReader::TokenType::EndElement: {
                // Name.
                QString name = reader.name().toString();

                while (! m_contextStack.empty()) {
                    if (m_contextStack.back()->popElement(name)) {
                        // Call callback.
                        if (m_contextStack.back()->onStopElement(
                                *this, *m_contextStack.back(), name)) {
                            break;

                        } else {
                            return false;
                        }

                    } else {
                        m_contextStack.pop_back();
                    }
                }

            } break;

            case QXmlStreamReader::TokenType::Characters:
                if (! m_contextStack.back()->onCharacters(
                        *this, *m_contextStack.back(),
                        reader.text().toString())) {
                    return false;
                }
                break;

            default:
                // Ignore other types.
                break;
        }
    }

    return true;
}

/**
 * @brief	Destructor.
 */
XMLLoader::~XMLLoader() {}
