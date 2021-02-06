#include <QtCore/QDebug>

#include <common/xml_loader_context.h>

/**
 * @brief		Constructor.
 */
XMLLoader::Context::Context() {}

/**
 * @brief		Create constructor.
 */
::std::unique_ptr<XMLLoader::Context> XMLLoader::Context::create()
{
    return ::std::unique_ptr<Context>(new Context());
}

/**
 * @brief		Push element.
 */
void XMLLoader::Context::pushElement(const QString &name)
{
    m_elementStack.push_back(name);
}

/**
 * @brief		Pop element.
 */
bool XMLLoader::Context::popElement(const QString &name)
{
    while (! m_elementStack.empty()) {
        if (m_elementStack.back() == name) {
            m_elementStack.pop_back();
            return true;
        } else {
            m_elementStack.pop_back();
        }
    }

    return false;
}

// Document
/**
 * @brief		Set on start document callback.
 */
void XMLLoader::Context::setOnStartDocument(
    ::std::function<bool(XMLLoader &, Context &)> onStartDocument)
{
    m_onStartDocument = onStartDocument;
}

/**
 * @brief		On start document callback.
 */
bool XMLLoader::Context::onStartDocument(XMLLoader &loader, Context &context)
{
    if (m_onStartDocument) {
        return m_onStartDocument(loader, context);
    }

    return true;
}

/**
 * @brief		Set on stop document callback.
 */
void XMLLoader::Context::onSetStopDocument(
    ::std::function<bool(XMLLoader &, Context &)> onStopDocument)
{
    m_onStopDocument = onStopDocument;
}

/**
 * @brief		On stop document callback.
 */
bool XMLLoader::Context::onStopDocument(XMLLoader &loader, Context &context)
{
    if (m_onStopDocument) {
        return m_onStopDocument(loader, context);
    }

    return true;
}

// Elements
/**
 * @brief		Set on start element callback.
 */
void XMLLoader::Context::setOnStartElement(
    ::std::function<bool(XMLLoader &,
                         Context &,
                         const QString &,
                         const QMap<QString, QString> &)> onStartElement)
{
    m_onStartElement = onStartElement;
}

/**
 * @brief		On start element callback.
 */
bool XMLLoader::Context::onStartElement(XMLLoader &                   loader,
                                        Context &                     context,
                                        const QString &               name,
                                        const QMap<QString, QString> &attr)
{
    if (m_onStartElement) {
        return m_onStartElement(loader, context, name, attr);
    }

    return true;
}

/**
 * @brief		Set on stop element callback.
 */
void XMLLoader::Context::setOnStopElement(
    ::std::function<bool(XMLLoader &, Context &, const QString &)>
        onStopElement)
{
    m_onStopElement = onStopElement;
}

/**
 * @brief		On stop element callback.
 */
bool XMLLoader::Context::onStopElement(XMLLoader &    loader,
                                       Context &      context,
                                       const QString &name)
{
    if (m_onStopElement) {
        return m_onStopElement(loader, context, name);
    }

    return true;
}

// Characters
/**
 * @brief		Set on characters callback.
 */
void XMLLoader::Context::setOnCharacters(
    ::std::function<bool(XMLLoader &, Context &, const QString &)> onCharacters)
{
    m_onCharacters = onCharacters;
}

/**
 * @brief		On characters callback.
 */
bool XMLLoader::Context::onCharacters(XMLLoader &    loader,
                                      Context &      context,
                                      const QString &text)
{
    if (m_onCharacters) {
        return m_onCharacters(loader, context, text);
    }

    return true;
}

/**
 * @brief		Destructor.
 */
XMLLoader::Context::~Context() {}
