#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QString>

#include <common/xml_loader.h>

/**
 * @brief	Loader context.
 */
class XMLLoader::Context {
  protected:
    // Stack
    QVector<QString> m_elementStack; //< Element stack.

    // Document
    ::std::function<void(XMLLoader &, Context &)>
        m_onStartDocument; //< Start document.
    ::std::function<void(XMLLoader &, Context &)>
        m_onStopDocument; //< Stop document.

    // Elements
    ::std::function<void(XMLLoader &,
                         Context &,
                         const QString &,
                         const QMap<QString, QString> &)>
        m_onStartElement; //< Start element.
    ::std::function<void(XMLLoader &, Context &, const QString &)>
        m_onStopElement; //< Stop element.

    // Characters
    ::std::function<void(XMLLoader &, Context &, const QString &)>
        m_onCharacters; //< Character.

  protected:
    /**
     * @brief		Constructor.
     */
    Context();

    // Delete constructors
    Context(const Context &) = delete;
    Context(Context &&)      = delete;

  public:
    /**
     * @brief		Create constructor.
     */
    static ::std::unique_ptr<Context> create();

    /**
     * @brief		Push element.
     *
     * @param[in]	name		Name of element.
     */
    void pushElement(const QString &name);

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
    bool pop(const QString &name, bool &remove);

    // Document
    /**
     * @brief		Set on start document callback.
     *
     * @param[in]	onStartDocument		Callback.
     */
    void setOnStartDocument(
        ::std::function<void(XMLLoader &, Context &)> onStartDocument);

    /**
     * @brief		On start document callback.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     */
    void onStartDocument(XMLLoader &loader, Context &context);

    /**
     * @brief		Set on stop document callback.
     *
     * @param[in]	onStopDocument		Callback.
     */
    void onSetStopDocument(
        ::std::function<void(XMLLoader &, Context &)> onStopDocument);

    /**
     * @brief		On stop document callback.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     */
    void onStopDocument(XMLLoader &loader, Context &context);

    // Elements
    /**
     * @brief		Set on start element callback.
     *
     * @param[in]	onStartElement		Callback.
     */
    void setOnStartElement(
        ::std::function<void(XMLLoader &,
                             Context &,
                             const QString &,
                             const QMap<QString, QString> &)> onStartElement);

    /**
     * @brief		On start element callback.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     */
    void onStartElement(XMLLoader &                   loader,
                        Context &                     context,
                        const QString &               name,
                        const QMap<QString, QString> &attr);

    /**
     * @brief		Set on stop element callback.
     *
     * @param[in]	onStopElement		Callback.
     */
    void setOnStopElement(
        ::std::function<void(XMLLoader &, Context &, const QString &)>
            onStopElement);

    /**
     * @brief		On stop element callback.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     */
    void
        onStopElement(XMLLoader &loader, Context &context, const QString &name);

    // Characters
    /**
     * @brief		Set on characters callback.
     *
     * @param[in]	onCharacters	Callback.
     */
    void setOnCharacters(
        ::std::function<void(XMLLoader &, Context &, const QString &)>
            onCharacters);

    /**
     * @brief		On characters callback.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	text		Text.
     */
    void onCharacters(XMLLoader &loader, Context &context, const QString &text);

    /**
     * @brief		Destructor..
     */
    virtual ~Context();
};
