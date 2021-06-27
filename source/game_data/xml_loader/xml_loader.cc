#include <QtCore/QDebug>
#include <QtCore/QRegExp>

#include <game_data/xml_loader/xml_loader.h>

/**
 * @brief	    Constructor.
 */
XMLLoader::XMLLoader() {}

/**
 * @brief       Get values saved in the loader.
 */
::std::map<QString, ::std::any> &XMLLoader::values()
{
    return m_values;
}

/**
 * @brief       Get element loader, create if not exists.
 */
XMLLoader::XMLElementLoader *XMLLoader::elementLoader(const QString &key)
{
    QStringList path = key.split("/", Qt::SkipEmptyParts);

    // Check root node.
    if (m_rootElementLoader == nullptr) {
        m_rootElementLoader = XMLElementLoader::create(
            path.front(), this, nullptr, nullptr, nullptr, nullptr, {});
    }

    Q_ASSERT(m_rootElementLoader != nullptr);

    // Other nodes.
    XMLElementLoader *elementLoader = m_rootElementLoader.get();
    while (! path.empty()) {
        auto iter = elementLoader->children().find(path.front());
        if (iter == elementLoader->children().end()) {
            // Create new element loader.
            auto newElementLoader
                = XMLElementLoader::create(path.front(), this, elementLoader,
                                           nullptr, nullptr, nullptr, {});
            Q_ASSERT(newElementLoader != nullptr);
            auto nextElementLoader = newElementLoader.get();
            elementLoader->children()[path.front()]
                = ::std::move(newElementLoader);
            elementLoader = nextElementLoader;
        } else {
            elementLoader = iter->second.get();
        }
        path.pop_front();
    }

    return elementLoader;
}

/**
 * @brief	    Load XML data.
 */
bool XMLLoader::loadData(const QString &xmlText, ErrorInfo &err)
{
    bool ret = m_doc.setContent(xmlText, &err.errorMsg, &err.errorLine,
                                &err.errorColumn);
    return ret;
}

/**
 * @brief	    Load patch.
 */
bool XMLLoader::loadPatch(const QString &xmlText, ErrorInfo &err)
{
    // Parse document.
    QDomDocument doc;
    if (! doc.setContent(xmlText, &err.errorMsg, &err.errorLine,
                         &err.errorColumn)) {
        return false;
    }

    QDomElement patchRoot = doc.documentElement();
    QDomElement dataRoot  = m_doc.documentElement();
    if (patchRoot.tagName() == "diff") {
        // Diff mode.
        return this->loadDiff(patchRoot, dataRoot);

    } else if (patchRoot.tagName() == dataRoot.tagName()) {
        // Merge.
        return this->mergePatch(patchRoot, dataRoot);

    } else {
        // Ignore
        err.errorMsg    = "Root node does not match!";
        err.errorLine   = 0;
        err.errorColumn = 0;

        return false;
    }
}

/**
 * @brief       Clear all data.
 */
void XMLLoader::clearData()
{
    m_doc = QDomDocument();
}

/**
 * @brief	    Parse XML.
 */
bool XMLLoader::parse()
{
    qDebug() << "================================";
    qDebug() << m_doc.toString().toStdString().c_str();
    return true;
}

/**
 * @brief	Destructor.
 */
XMLLoader::~XMLLoader() {}

/**
 * @brief	    Load diff.
 */
bool XMLLoader::loadDiff(QDomElement &source, QDomElement &dest)
{
    for (QDomElement diffElement = source.firstChildElement();
         ! diffElement.isNull();
         diffElement = diffElement.nextSiblingElement()) {
        // Parse "sel" attribute.
        if (! diffElement.hasAttribute("sel")) {
            qWarning() << "Line: " << diffElement.lineNumber()
                       << ", column: " << diffElement.columnNumber() << " : "
                       << "Missing attrbute \"sel\".";
            continue;
        }
        SelInfo sel = this->parseSel(diffElement.attribute("sel"));
        if (sel.path.size() == 0) {
            qWarning() << "Line: " << diffElement.lineNumber()
                       << ", column: " << diffElement.columnNumber() << " : "
                       << "Illegal value of attrbute \"sel\".";
            continue;
        }

        // Get element by sel.
        ::std::list<QDomElement> destElements = this->getElementBySel(sel);

        // Parse operation.
        if (diffElement.tagName() == "add") {
            if (sel.pathType == SelInfo::PathType::Attribute) {
                qWarning() << "Line: " << diffElement.lineNumber()
                           << ", column: " << diffElement.columnNumber()
                           << " : "
                           << "Attribute does not support \"add\" operation.";
                continue;
            }

            if (diffElement.hasAttribute("type")) {
                // Add an attribute.
                QString attrType = diffElement.attribute("type");
                if (attrType[0] != '@') {
                    qWarning()
                        << "Line: " << diffElement.lineNumber()
                        << ", column: " << diffElement.columnNumber() << " : "
                        << "Illegal value of attribute \"type\".";
                    continue;
                }
                attrType = attrType.replace("@", "");

                for (auto &element : destElements) {
                    element.setAttribute(attrType, diffElement.text());
                }
            } else {
                /*
                 *     <xsd:enumeration value="before"/>
      <xsd:enumeration value="after"/>
      <xsd:enumeration value="prepend"/>
      */
                QString attrPos = "end";
                if (diffElement.hasAttribute("pos")) {
                    // Add an attribute.
                    attrPos = diffElement.attribute("pos");
                }

                if (attrPos == "prepend") {
                    for (auto &element : destElements) {
                        this->copyChildNodes(element, diffElement,
                                             element.firstChild());
                    }

                } else if (attrPos == "end") {
                    for (auto &element : destElements) {
                        this->copyChildNodes(element, diffElement);
                    }

                } else if (attrPos == "before") {
                    for (auto &element : destElements) {
                        QDomNode parent = element.parentNode();
                        this->copyChildNodes(parent, diffElement, element);
                    }

                } else if (attrPos == "after") {
                    for (auto &element : destElements) {
                        QDomNode parent = element.parentNode();
                        this->copyChildNodes(parent, diffElement,
                                             element.nextSiblingElement());
                    }

                } else {
                }
                qWarning() << "Line: " << diffElement.lineNumber()
                           << ", column: " << diffElement.columnNumber()
                           << " : "
                           << "Illegal value of attribute \"pos\".";
                continue;
            }

        } else if (diffElement.tagName() == "replace") {
            // TODO:
        } else if (diffElement.tagName() == "remove") {
            // TODO:
        } else {
            qWarning() << "Line: " << diffElement.lineNumber()
                       << ", column: " << diffElement.columnNumber() << " : "
                       << QString("Unknow element \"%1\" in \"diff\" element.")
                              .arg(diffElement.tagName())
                              .toStdString()
                              .c_str();
        }
    }

    return true;
}

/**
 * @brief	    Merge patch.
 */
bool XMLLoader::mergePatch(QDomElement &source, QDomElement &dest)
{
    for (QDomElement child = source.firstChildElement(); ! child.isNull();
         child             = child.nextSiblingElement()) {
        dest.appendChild(child);
    }
    return true;
}

/**
 * @brief       Parse "sel" atttribute.
 */
XMLLoader::SelInfo XMLLoader::parseSel(const QString &sel)
{
    SelInfo ret;

    // Split path.
    QStringList pathSplitted = sel.split("/", Qt::SkipEmptyParts);
    if (pathSplitted.size() == 0) {
        return ret;
    }

    ::std::vector<SelPathNodeInfo> path;
    ret.pathType = SelInfo::PathType::Element;
    while (! pathSplitted.empty()) {
        // Parse attribute.
        do {
            // Element wit attribute.
            QRegExp elementWithAttrExpr(
                "([a-zA-Z]\\w*)\\[@([a-zA-Z]\\w*)=['\"](.*)['\"]\\]");
            int pos = elementWithAttrExpr.indexIn(pathSplitted.front());
            if (pos == 0
                && elementWithAttrExpr.matchedLength()
                       == pathSplitted.front().size()) {
                path.push_back({elementWithAttrExpr.cap(1),
                                ::std::pair<QString, QString>(
                                    elementWithAttrExpr.cap(2),
                                    elementWithAttrExpr.cap(3))});
                break;
            }

            // Attribute.
            QRegExp attrExpr("@([a-zA-Z]\\w*)");
            pos = attrExpr.indexIn(pathSplitted.front());
            if (pos == 0
                && attrExpr.matchedLength() == pathSplitted.front().size()) {
                if (pathSplitted.size() != 1) {
                    return ret;
                }

                path.push_back({attrExpr.cap(1), ::std::nullopt});
                ret.pathType = SelInfo::PathType::Attribute;
                break;
            }

            // Element.
            path.push_back({pathSplitted.front(), ::std::nullopt});
            ret.pathType = SelInfo::PathType::Element;

        } while (false);
        pathSplitted.pop_front();
    }

    ret.path = ::std::move(path);

    return ret;
}

/**
 * @brief       Get element by sel.
 */
::std::list<QDomElement> XMLLoader::getElementBySel(XMLLoader::SelInfo &sel)
{
    QDomElement element = m_doc.documentElement();

    // Check root.
    auto iter = sel.path.begin();

    if (iter->name != element.tagName()) {
        return {};
    }

    // Search.
    auto end = sel.path.end();
    if (sel.pathType == SelInfo::PathType::Attribute) {
        --end;
    }

    if (iter == end) {
        return {element};
    }

    return this->getElementBySel(sel, element, ++iter, end);
}

/**
 * @brief       Get elements by sel.
 */
::std::list<QDomElement> XMLLoader::getElementBySel(
    SelInfo &                                       sel,
    QDomElement &                                   element,
    const ::std::vector<SelPathNodeInfo>::iterator &iter,
    const ::std::vector<SelPathNodeInfo>::iterator &end)
{
    ::std::list<QDomElement> ret;
    auto                     nextIter = iter;
    ++nextIter;
    QString name;

    if (iter->name != "*") {
        name = iter->name;
    }

    for (auto childElement = element.firstChildElement(name);
         ! childElement.isNull();
         childElement = childElement.nextSiblingElement(name)) {
        // Check attributes.
        if (iter->attribute.has_value()) {
            auto &attribute = iter->attribute.value();
            if (! childElement.hasAttribute(attribute.first)) {
                continue;
            }

            if (childElement.attribute(attribute.first) != attribute.second) {
                continue;
            }
        }

        // Skip ".".
        bool skip = false;
        while (nextIter != end && nextIter->name == ".") {
            if (nextIter->attribute.has_value()) {
                auto &attribute = nextIter->attribute.value();
                if (! childElement.hasAttribute(attribute.first)) {
                    skip = true;
                    break;
                }

                if (childElement.attribute(attribute.first)
                    != attribute.second) {
                    skip = true;
                    break;
                }
            }
            ++nextIter;
        }
        if (skip) {
            continue;
        }

        if (nextIter == end) {
            ret.push_back(childElement);
        } else {
            ret.splice(ret.end(),
                       this->getElementBySel(sel, childElement, nextIter, end));
        }
    }

    return ret;
}

/**
 * @brief       Copy child nodes.
 */
void XMLLoader::copyChildNodes(QDomNode        dest,
                               const QDomNode &src,
                               const QDomNode &before)
{
    for (QDomNode child = src.firstChild(); ! child.isNull();
         child          = child.nextSiblingElement()) {
        QDomNode newNode = child.cloneNode();
        if (before.isNull()) {
            dest.insertAfter(newNode, before);

        } else {
            dest.insertBefore(newNode, before);
        }
    }
}
