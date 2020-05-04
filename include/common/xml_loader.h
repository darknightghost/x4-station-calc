#pragma once

#include <memory>
#include <vector>

#include <QtCore/QXmlStreamReader>

/**
 * @brief		XML parser.QXmlStreamReader
 */
class XMLLoader {
  public:
    /**
     * @brief		Reader context.
     */
    class Context;

  protected:
    ::std::vector<::std::unique_ptr<Context>> m_contextStack; ///< Contexts.

  public:
    /**
     * @brief	Constructor.
     */
    XMLLoader();

    /**
     * @brief		Push context.
     *
     * @param[in]	context			Context to push.
     */
    void pushContext(::std::unique_ptr<Context> context);

    /**
     * @brief	Parse XML.
     *
     * @param[in]	reader			XML SAX reader..
     * @param[in]	context			First context.
     *
     * @return		Returns \c true if the whole file has been parsed,
     *				otherwise returns false.
     */
    bool parse(QXmlStreamReader &reader, ::std::unique_ptr<Context> context);

    /**
     * @brief	Destructor.
     */
    virtual ~XMLLoader();
};

#include <common/xml_loader_context.h>
