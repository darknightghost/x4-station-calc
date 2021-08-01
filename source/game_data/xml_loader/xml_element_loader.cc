#include <game_data/xml_loader/xml_element_loader.h>

/**
 * @brief		Constructor.
 */
XMLLoader::XMLElementLoader::XMLElementLoader(
    QString                                 name,
    XMLLoader *                             xmlLoader,
    XMLElementLoader *                      parent,
    StartElementCallback                    onStartElement,
    StopElementCallback                     onStopElement,
    ElementTextCallback                     onElementText,
    ::std::map<QString, AttrributeCallback> onAttributes) :
    m_name(name),
    m_loader(xmlLoader), m_parent(parent), m_onStartElement(onStartElement),
    m_onStopElement(onStopElement), m_onElementText(onElementText),
    m_onAttributes(onAttributes)
{
    this->setInitialized();
}

/**
 * @brief		Copy constructor.
 */
XMLLoader::XMLElementLoader::XMLElementLoader(const XMLElementLoader &src) :
    m_name(src.m_name), m_loader(nullptr), m_parent(nullptr),
    m_onStartElement(src.m_onStartElement),
    m_onStopElement(src.m_onStopElement), m_onElementText(src.m_onElementText),
    m_onAttributes(src.m_onAttributes)
{
    for (auto &pair : m_children) {
        auto clonedChild = pair.second->clone();
        if (clonedChild == nullptr) {
            return;
        }

        clonedChild->m_parent  = this;
        m_children[pair.first] = ::std::move(clonedChild);
    }

    this->setInitialized();
}

/**
 * @brief   Get element name.
 */
const QString &XMLLoader::XMLElementLoader::name() const
{
    return m_name;
}

/**
 * @brief   Get XML loader.
 */
XMLLoader *XMLLoader::XMLElementLoader::loader()
{
    return m_loader;
}

/**
 * @brief   Get parent node.
 */
XMLLoader::XMLElementLoader *XMLLoader::XMLElementLoader::parent()
{
    return m_parent;
}

/**
 * @brief   Get children.
 */
::std::map<QString, ::std::unique_ptr<XMLLoader::XMLElementLoader>> &
    XMLLoader::XMLElementLoader::children()
{
    return m_children;
}

/**
 * @brief		Set on start element callback.
 */
void XMLLoader::XMLElementLoader::setOnStartElement(
    StartElementCallback onStartElement)
{
    m_onStartElement = ::std::move(onStartElement);
}

/**
 * @brief		On start element callback.
 */
bool XMLLoader::XMLElementLoader::onStartElement(
    const ::std::map<QString, QString> &attributes)
{
    if (m_onStartElement) {
        return m_onStartElement(*m_loader, *this, attributes);
    } else {
        return true;
    }
}

/**
 * @brief		Set on stop element callback.
 *
 * @param[in]	onStopElement		Callback.
 */
void XMLLoader::XMLElementLoader::setOnStopElement(
    StopElementCallback onStopElement)
{
    m_onStopElement = ::std::move(onStopElement);
}

/**
 * @brief		On stop element callback.
 *
 */
bool XMLLoader::XMLElementLoader::onStopElement()
{
    if (m_onStopElement) {
        return m_onStopElement(*m_loader, *this);
    } else {
        return true;
    }
}

/**
 * @brief		Set on element text callback.
 */
void XMLLoader::XMLElementLoader::setOnElementText(
    ElementTextCallback onElementText)
{
    m_onElementText = ::std::move(onElementText);
}

/**
 * @brief		On element text callback.
 *
 */
bool XMLLoader::XMLElementLoader::onElementText(const QString &text)
{
    if (m_onElementText) {
        return m_onElementText(*m_loader, *this, text);
    } else {
        return true;
    }
}

/**
 * @brief		Set on attribute callback.
 */
void XMLLoader::XMLElementLoader::setOnAttribute(const QString &    attrName,
                                                 AttrributeCallback onAttribute)
{
    m_onAttributes[attrName] = onAttribute;
}

/**
 * @brief       Get reference to attribute callbacks.
 */
::std::map<QString, XMLLoader::XMLElementLoader::AttrributeCallback> &
    XMLLoader::XMLElementLoader::attributeCallbacks()
{
    return m_onAttributes;
}

/**
 * @brief		Parse attribute.
 */
bool XMLLoader::XMLElementLoader::parseAttribute(const QString &name,
                                                 const QString &value)
{
    auto iter = m_onAttributes.find(name);
    if (iter != m_onAttributes.end()) {
        if (iter->second) {
            return iter->second(*m_loader, *this, name, value);
        }
    }

    return true;
}

/**
 * @brief		Parse attribute.
 */
bool XMLLoader::XMLElementLoader::parseAttributes(
    const ::std::map<QString, QString> &attributes)
{
    for (auto attrIter = attributes.begin(); attrIter != attributes.end();
         ++attrIter) {
        auto iter = m_onAttributes.find(attrIter->first);
        if (iter != m_onAttributes.end()) {
            if (iter->second) {
                if (! iter->second(*m_loader, *this, attrIter->first,
                                   attrIter->second)) {
                    return false;
                }
            }
        }
    }

    return true;
}

/**
 * @brief		Destructor.
 */
XMLLoader::XMLElementLoader::~XMLElementLoader() {}

/**
 * @brief       Set element name.
 */
void XMLLoader::XMLElementLoader::setName(const QString &name)
{
    m_name = name;
}

/**
 * @brief   Set XML loader.
 *
 */
void XMLLoader::XMLElementLoader::setLoader(XMLLoader *loader)
{
    m_loader = loader;

    for (auto &pair : m_children) {
        pair.second->setLoader(loader);
    }
}
