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
 *
 * @param[in]	name		Name of element.
 */
void XMLLoader::Context::pushElement(const QString &name)
{
    m_elementStack.push_back(name);
}

/**
 * @brief		Pop element.
 *
 * @param[in]	name		Name of element.
 * @param[out]	remove		Remove flag, \c true if the context should be
 *							removed.
 *
 * @return		If the name of element found, \c true is returned,
 *				otherwise returns \c false.
 */
bool XMLLoader::Context::pop(const QString &name, bool &remove)
{
    while (! m_elementStack.empty()) {
        if (m_elementStack.back() == name) {
            m_elementStack.pop_back();
            remove = m_elementStack.empty();
            return true;

        } else {
            m_elementStack.pop_back();
        }
    }

    remove = true;
    return false;
}

// Document
/**
 * @brief		Set on start document callback.
 *
 * @param[in]	onStartDocument		Callback.
 */
void XMLLoader::Context::setOnStartDocument(
    ::std::function<void(XMLLoader &, Context &)> onStartDocument)
{
    m_onStartDocument = onStartDocument;
}

/**
 * @brief		On start document callback.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 */
void XMLLoader::Context::onStartDocument(XMLLoader &loader, Context &context)
{
    if (m_onStartDocument) {
        m_onStartDocument(loader, context);
    }
}

/**
 * @brief		Set on stop document callback.
 *
 * @param[in]	onStopDocument		Callback.
 */
void XMLLoader::Context::onSetStopDocument(
    ::std::function<void(XMLLoader &, Context &)> onStopDocument)
{
    m_onStopDocument = onStopDocument;
}

/**
 * @brief		On stop document callback.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 */
void XMLLoader::Context::onStopDocument(XMLLoader &loader, Context &context)
{
    if (m_onStopDocument) {
        m_onStopDocument(loader, context);
    }
}

// Elements
/**
 * @brief		Set on start element callback.
 *
 * @param[in]	onStartElement		Callback.
 */
void XMLLoader::Context::setOnStartElement(
    ::std::function<void(XMLLoader &,
                         Context &,
                         const QString &,
                         const QMap<QString, QString> &)> onStartElement)
{
    m_onStartElement = onStartElement;
}

/**
 * @brief		On start element callback.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 * @param[in]	attr		Attributes.
 */
void XMLLoader::Context::onStartElement(XMLLoader &                   loader,
                                        Context &                     context,
                                        const QString &               name,
                                        const QMap<QString, QString> &attr)
{
    if (m_onStartElement) {
        m_onStartElement(loader, context, name, attr);
    }
}

/**
 * @brief		Set on stop element callback.
 *
 * @param[in]	onStopElement		Callback.
 */
void XMLLoader::Context::setOnStopElement(
    ::std::function<void(XMLLoader &, Context &, const QString &)>
        onStopElement)
{
    m_onStopElement = onStopElement;
}

/**
 * @brief		On stop element callback.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	name		Name of the element.
 */
void XMLLoader::Context::onStopElement(XMLLoader &    loader,
                                       Context &      context,
                                       const QString &name)
{
    if (m_onStopElement) {
        m_onStopElement(loader, context, name);
    }
}

// Characters
/**
 * @brief		Set on characters callback.
 *
 * @param[in]	onCharacters	Callback.
 */
void XMLLoader::Context::setOnCharacters(
    ::std::function<void(XMLLoader &, Context &, const QString &)> onCharacters)
{
    m_onCharacters = onCharacters;
}

/**
 * @brief		On characters callback.
 *
 * @param[in]	loader		XML loader.
 * @param[in]	context		Context.
 * @param[in]	text		Text.
 */
void XMLLoader::Context::onCharacters(XMLLoader &    loader,
                                      Context &      context,
                                      const QString &text)
{
    if (m_onCharacters) {
        m_onCharacters(loader, context, text);
    }
}

/**
 * @brief		Destructor..
 */
XMLLoader::Context::~Context() {}
