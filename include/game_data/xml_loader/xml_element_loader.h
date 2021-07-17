#pragma once

#include <functional>
#include <map>
#include <memory>

#include <QtCore/QString>

#include <game_data/xml_loader/xml_loader.h>
#include <interfaces/i_create_factory_func_unique.h>

/**
 * @brief	XML element loader.
 */
class XMLLoader::XMLElementLoader :
    public ICreateFactoryFuncUnique<
        XMLLoader::XMLElementLoader,
        QString,
        XMLLoader *,
        XMLElementLoader *,
        ::std::function<bool(XMLLoader &,
                             XMLElementLoader &,
                             const ::std::map<QString, QString> &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &, const QString &)>,
        ::std::map<QString,
                   ::std::function<bool(XMLLoader &,
                                        XMLElementLoader &,
                                        const QString &,
                                        const QString &)>>> {
    CREATE_UNIQUE_FUNC(
        XMLLoader::XMLElementLoader,
        QString,
        XMLLoader *,
        XMLElementLoader *,
        ::std::function<bool(XMLLoader &,
                             XMLElementLoader &,
                             const ::std::map<QString, QString> &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &, const QString &)>,
        ::std::map<QString,
                   ::std::function<bool(XMLLoader &,
                                        XMLElementLoader &,
                                        const QString &,
                                        const QString &)>>);

  public:
    /**
     * @brief       Start element callback.
     *
     * @param[in]   xmlLoader       XML loader.
     * @param[in]   elementLoader   Element loader.
     * @param[in]   attributes      Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    using StartElementCallback = ::std::function<bool(
        XMLLoader &, XMLElementLoader &, const ::std::map<QString, QString> &)>;

    /**
     * @brief       Stop element callback.
     *
     * @param[in]   xmlLoader       XML loader.
     * @param[in]   elementLoader   Element loader.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    using StopElementCallback
        = ::std::function<bool(XMLLoader &, XMLElementLoader &)>;

    /**
     * @brief       Stop element callback.
     *
     * @param[in]   xmlLoader       XML loader.
     * @param[in]   elementLoader   Element loader.
     * @param[in]   text            Text.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    using ElementTextCallback = ::std::function<bool(
        XMLLoader &, XMLElementLoader &, const QString &)>;

    /**
     * @brief       Attribute callback.
     *
     * @param[in]   xmlLoader       XML loader.
     * @param[in]   elementLoader   Element loader.
     * @param[in]   name            Name of the attribute.
     * @param[in]   value           Value of the attribute.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    using AttrributeCallback = ::std::function<bool(
        XMLLoader &, XMLElementLoader &, const QString &, const QString &)>;

  protected:
    QString m_name; ///< Element name.

    XMLLoader *       m_loader; ///< XML loader.
    XMLElementLoader *m_parent; ///< Parent node.
    ::std::map<QString, ::std::unique_ptr<XMLElementLoader>>
        m_children; ///< Children.

    // Event handlers.
    StartElementCallback m_onStartElement;                  ///< Start element.
    StopElementCallback  m_onStopElement;                   ///< Stop element.
    ElementTextCallback  m_onElementText;                   ///< Element text.
    ::std::map<QString, AttrributeCallback> m_onAttributes; ///< Attributes.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]   name            Element name.
     * @param[in]   xmlLoader       XML loader.
     * @param[in]   parent          Parent loader.
     * @param[in]   onStartElement  Start element callback.
     * @param[in]   onStopElement   Stop element callback.
     * @param[in]   onElementText   Element text callback.
     * @param[in]   onAttributes    Attribute callbacks.
     */
    XMLElementLoader(QString              name,
                     XMLLoader *          xmlLoader,
                     XMLElementLoader *   parent,
                     StartElementCallback onStartElement = nullptr,
                     StopElementCallback  onStopElement  = nullptr,
                     ElementTextCallback  onElementText  = nullptr,
                     ::std::map<QString, AttrributeCallback> onAttributes = {});

    // Delete constructors
    XMLElementLoader(const XMLElementLoader &) = delete;
    XMLElementLoader(XMLElementLoader &&)      = delete;

  public:
    using ICreateFactoryFuncUnique<
        XMLLoader::XMLElementLoader,
        QString,
        XMLLoader *,
        XMLElementLoader *,
        ::std::function<bool(XMLLoader &,
                             XMLElementLoader &,
                             const ::std::map<QString, QString> &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &)>,
        ::std::function<bool(XMLLoader &, XMLElementLoader &, const QString &)>,
        ::std::map<QString,
                   ::std::function<bool(XMLLoader &,
                                        XMLElementLoader &,
                                        const QString &,
                                        const QString &)>>>::create;

    /**
     * @brief   Get element name.
     *
     * @return  Element name.
     */
    const QString &name() const;

    /**
     * @brief   Get xML loader.
     *
     * @return  XML loader.
     */
    XMLLoader *loader();

    /**
     * @brief   Get parent node.
     *
     * @return  Parent node.
     */
    XMLElementLoader *parent();

    /**
     * @brief   Get children.
     *
     * @return  Children.
     */
    ::std::map<QString, ::std::unique_ptr<XMLElementLoader>> &children();

    // Elements
    /**
     * @brief		Set on start element callback.
     *
     * @param[in]	onStartElement		Callback.
     */
    void setOnStartElement(StartElementCallback onStartElement);

    /**
     * @brief		On start element callback.
     *
     * @param[in]	attributes      Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElement(const ::std::map<QString, QString> &attributes);

    /**
     * @brief		Set on stop element callback.
     *
     * @param[in]	onStopElement		Callback.
     */
    void setOnStopElement(StopElementCallback onStopElement);

    /**
     * @brief		On stop element callback.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStopElement();

    // Characters
    /**
     * @brief		Set on element text callback.
     *
     * @param[in]	onElementText	    Callback.
     */
    void setOnElementText(ElementTextCallback onElementText);

    /**
     * @brief		On element text callback.
     *
     * @param[in]	text		    Text.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onElementText(const QString &text);

    /**
     * @brief		Set on attribute callback.
     *
     * @param[in]	attrName	    Name of attribute.
     * @param[in]	onAttribute	    Callback.
     */
    void setOnAttribute(const QString &    attrName,
                        AttrributeCallback onAttribute);

    /**
     * @brief       Get reference to attribute callbacks.
     *
     * @return      Reference.
     */
    ::std::map<QString, AttrributeCallback> &attributeCallbacks();

    /**
     * @brief		Parse attribute.
     *
     * @param[in]	name        Name of the attribute.
     * @param[in]	value       Value of the attribute.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool parseAttribute(const QString &name, const QString &value);

    /**
     * @brief		Parse attribute.
     *
     * @param[in]	attributes          Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool parseAttributes(const ::std::map<QString, QString> &attributes);

    /**
     * @brief		Destructor.
     */
    virtual ~XMLElementLoader();
};
