#pragma once

#include <any>
#include <map>
#include <memory>
#include <vector>

#include <QtCore/QString>
#include <QtXml/QDomDocument>

/**
 * @brief		XML parser.
 */
class XMLLoader {
  public:
    class XMLElementLoader;

    /**
     * @brief       Error infomation.
     */
    struct ErrorInfo {
        QString errorMsg;    ///< Error message.
        int     errorLine;   ///< Error line.
        int     errorColumn; ///< Error column.
    };

  protected:
    /**
     * @brief       Sel infomation.
     */
    struct SelInfo {
        ::std::vector<QString>       path;       ///< Path of target element.
        ::std::map<QString, QString> attributes; ///< Attributes.
    };

  protected:
    QDomDocument m_doc; ///< Document.
    ::std::unique_ptr<XMLElementLoader>
        m_rootElementLoader;                  ///< Root element loader.
    ::std::map<QString, ::std::any> m_values; ///< Values.

  public:
    /**
     * @brief	    Constructor.
     */
    XMLLoader();

    /**
     * @brief       Get values saved in the loader.
     *
     * @return      Reference to values.
     */
    ::std::map<QString, ::std::any> &values();

    /**
     * @brief       Get element loader, create if not exists.
     *
     * @param[in]   key         Key of the element.
     *
     * @return      Pointer to the element loader, the ownership of the element
     *              loader belongs to XML loader.
     */
    XMLElementLoader *elementLoader(const QString &key);

    /**
     * @brief	    Load XML data.
     *
     * @param[in]	xmlText         XML text.
     * @param[out]	err             Error infomation.
     *
     * @return		Returns \c true if the whole file has been parsed,
     *				otherwise returns \c false.
     */
    bool loadData(const QString &xmlText, ErrorInfo &err);

    /**
     * @brief	    Load patch.
     *
     * @param[in]	xmlText         XML text.
     * @param[out]	err             Error infomation.
     *
     * @return		Returns \c true if the whole file has been parsed,
     *				otherwise returns \c false.
     */
    bool loadPatch(const QString &xmlText, ErrorInfo &err);

    /**
     * @brief       Clear all data.
     */
    void clearData();

    /**
     * @brief	    Parse XML.
     *
     * @return		Returns \c true if the whole file has been parsed,
     *				otherwise returns \c false.
     */
    bool parse();

    /**
     * @brief	Destructor.
     */
    virtual ~XMLLoader();

  private:
    /**
     * @brief	    Load diff.
     *
     * @param[in]	source      Source root element.
     * @param[in]	dest`       Destination root element.
     *
     * @return		On success, the method returns \c true, otherwise returns
     *              \c false.
     */
    bool loadDiff(QDomElement &source, QDomElement &dest);

    /**
     * @brief	    Merge patch.
     *
     * @param[in]	source      Source root element.
     * @param[in]	dest`       Destination root element.
     *
     * @return		On success, the method returns \c true, otherwise returns
     *              \c false.
     */
    bool mergePatch(QDomElement &source, QDomElement &dest);

    /**
     * @brief       Parse "sel" atttribute.
     *
     * @return      Parse result.
     */
    SelInfo parseSel(const QString &sel);
};

#include <game_data/xml_loader/xml_element_loader.h>
