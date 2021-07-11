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
    } else {
        if (m_rootElementLoader->name() != path.front()) {
            return nullptr;
        }
    }
    path.pop_front();

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
        return this->loadDiff(patchRoot);

    } else if (patchRoot.tagName() == dataRoot.tagName()) {
        // Merge.
        return this->mergePatch(patchRoot);

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
    XMLElementLoader *loader  = m_rootElementLoader.get();
    QDomElement       element = m_doc.documentElement();
    if (loader == nullptr || loader->name() != element.tagName()) {
        return true;
    }

    return this->parseElement(loader, element);
}

/**
 * @brief	Destructor.
 */
XMLLoader::~XMLLoader() {}

/**
 * @brief	    Load diff.
 */
bool XMLLoader::loadDiff(QDomElement &source)
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
                    qWarning()
                        << "Line: " << diffElement.lineNumber()
                        << ", column: " << diffElement.columnNumber() << " : "
                        << "Illegal value of attribute \"pos\".";
                }
            }

        } else if (diffElement.tagName() == "replace") {
            switch (sel.pathType) {
                case SelInfo::PathType::Element: {
                    for (QDomElement element : destElements) {
                        QDomNode parent = element.parentNode();
                        this->copyChildNodes(parent, diffElement,
                                             element.nextSiblingElement());
                        parent.removeChild(element);
                    }
                } break;

                case SelInfo::PathType::Attribute: {
                    for (QDomElement element : destElements) {
                        element.setAttribute(sel.path.back().name,
                                             diffElement.text());
                    }
                } break;
            }

        } else if (diffElement.tagName() == "remove") {
            switch (sel.pathType) {
                case SelInfo::PathType::Element: {
                    if (diffElement.hasAttribute("ws")) {
                        QString attrWs = diffElement.attribute("ws");
                        if (attrWs == "before") {
                            for (QDomElement element : destElements) {
                                // Scan witespace to remove.
                                ::std::vector<QDomText> nodeToRemove;
                                for (QDomNode node = element.previousSibling();
                                     ! node.isNull();
                                     node = node.previousSibling()) {
                                    QDomText textNode = node.toText();
                                    if (textNode.isNull()
                                        || textNode.data()
                                                   .replace(" ", "")
                                                   .replace("\t", "")
                                                   .replace("\r", "")
                                                   .replace("\n", "")
                                               != "") {
                                        break;
                                    }
                                    nodeToRemove.push_back(textNode);
                                }

                                // Remove.
                                for (auto &node : nodeToRemove) {
                                    element.parentNode().removeChild(node);
                                }
                            }
                        } else if (attrWs == "after") {
                            for (QDomElement element : destElements) {
                                // Scan witespace to remove.
                                ::std::vector<QDomText> nodeToRemove;
                                for (QDomNode node = element.nextSibling();
                                     ! node.isNull();
                                     node = node.nextSibling()) {
                                    QDomText textNode = node.toText();
                                    if (textNode.isNull()
                                        || textNode.data()
                                                   .replace(" ", "")
                                                   .replace("\t", "")
                                                   .replace("\r", "")
                                                   .replace("\n", "")
                                               != "") {
                                        break;
                                    }
                                    nodeToRemove.push_back(textNode);
                                }

                                // Remove.
                                for (auto &node : nodeToRemove) {
                                    element.parentNode().removeChild(node);
                                }
                            }
                        } else if (attrWs == "both") {
                            for (QDomElement element : destElements) {
                                // Scan witespace to remove.
                                ::std::vector<QDomText> nodeToRemove;
                                for (QDomNode node = element.previousSibling();
                                     ! node.isNull();
                                     node = node.previousSibling()) {
                                    QDomText textNode = node.toText();
                                    if (textNode.isNull()
                                        || textNode.data()
                                                   .replace(" ", "")
                                                   .replace("\t", "")
                                                   .replace("\r", "")
                                                   .replace("\n", "")
                                               != "") {
                                        break;
                                    }
                                    nodeToRemove.push_back(textNode);
                                }
                                for (QDomNode node = element.nextSibling();
                                     ! node.isNull();
                                     node = node.nextSibling()) {
                                    QDomText textNode = node.toText();
                                    if (textNode.isNull()
                                        || textNode.data()
                                                   .replace(" ", "")
                                                   .replace("\t", "")
                                                   .replace("\r", "")
                                                   .replace("\n", "")
                                               != "") {
                                        break;
                                    }
                                    nodeToRemove.push_back(textNode);
                                }

                                // Remove.
                                for (auto &node : nodeToRemove) {
                                    element.parentNode().removeChild(node);
                                }
                            }
                        } else {
                            qWarning()
                                << "Line: " << diffElement.lineNumber()
                                << ", column: " << diffElement.columnNumber()
                                << " : "
                                << "Illegal value of attribute \"ws\".";
                        }
                    }
                    for (QDomElement element : destElements) {
                        element.parentNode().removeChild(element);
                    }
                } break;

                case SelInfo::PathType::Attribute: {
                    for (QDomElement element : destElements) {
                        element.removeAttribute(sel.path.back().name);
                    }

                } break;
            }
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
bool XMLLoader::mergePatch(QDomElement &source)
{
    QDomElement dest = m_doc.documentElement();
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

    // Search.
    auto end = sel.path.end();
    if (sel.pathType == SelInfo::PathType::Attribute) {
        --end;
    }

    return this->getElementBySel(sel, element, sel.path.begin(), end);
}

/**
 * @brief       Get elements by sel.
 */
::std::list<QDomElement> XMLLoader::getElementBySel(
    SelInfo &                                       sel,
    const QDomElement &                             element,
    const ::std::vector<SelPathNodeInfo>::iterator &iter,
    const ::std::vector<SelPathNodeInfo>::iterator &end)
{
    ::std::list<QDomElement> ret;
    auto                     nextIter = iter;
    ++nextIter;

    do {
        if (iter->name == element.tagName() || iter->name == "*") {
            // Name matched.
            // Check attributes.
            if (iter->attribute.has_value()) {
                auto &attribute = iter->attribute.value();
                if (! element.hasAttribute(attribute.first)) {
                    break;
                }

                if (element.attribute(attribute.first) != attribute.second) {
                    break;
                }
            }

            // Skip ".".
            bool skip = false;
            while (nextIter != end && nextIter->name == ".") {
                if (nextIter->attribute.has_value()) {
                    auto &attribute = nextIter->attribute.value();
                    if (! element.hasAttribute(attribute.first)) {
                        skip = true;
                        break;
                    }

                    if (element.attribute(attribute.first)
                        != attribute.second) {
                        skip = true;
                        break;
                    }
                }
                ++nextIter;
            }
            if (skip) {
                break;
            }

            if (nextIter == end) {
                // Append current element to the list.
                ret.push_back(element);
            } else {
                // Search child.
                bool searchCurrentIter = iter->name == "*" ? true : false;
                for (auto childElement = element.firstChildElement();
                     ! childElement.isNull();
                     childElement = childElement.nextSiblingElement()) {
                    ret.splice(ret.end(),
                               this->getElementBySel(sel, childElement,
                                                     nextIter, end));
                    if (searchCurrentIter) {
                        ret.splice(ret.end(),
                                   this->getElementBySel(sel, childElement,
                                                         iter, end));
                    }
                }
            }
        }
    } while (0);

    return ret;
}

/**
 * @brief       Copy child nodes.
 */
void XMLLoader::copyChildNodes(QDomNode        dest,
                               const QDomNode &source,
                               const QDomNode &before)
{
    for (QDomNode child = source.firstChild(); ! child.isNull();
         child          = child.nextSiblingElement()) {
        QDomNode newNode = child.cloneNode();
        if (before.isNull()) {
            dest.insertAfter(newNode, before);

        } else {
            dest.insertBefore(newNode, before);
        }
    }
}

/**
 * @brief	    Parse XML element.
 */
bool XMLLoader::parseElement(XMLElementLoader * elementLoader,
                             const QDomElement &element)
{
    // Get attributes.
    ::std::map<QString, QString> attributes;
    QDomNamedNodeMap             elementAttributes = element.attributes();
    for (int i = 0; i < elementAttributes.size(); ++i) {
        QDomNode item               = elementAttributes.item(i);
        attributes[item.nodeName()] = item.nodeValue();
    }

    // Start element.
    if (! elementLoader->onStartElement(attributes)) {
        return false;
    }

    // Attributes.
    if (! elementLoader->parseAttributes(attributes)) {
        return false;
    }

    // Text.
    QString text = element.text();
    if (text != "") {
        if (! elementLoader->onElementText(text)) {
            return false;
        }
    }

    // Children.
    for (QDomElement child = element.firstChildElement(); ! child.isNull();
         child             = child.nextSiblingElement()) {
        auto iter = elementLoader->children().find(child.tagName());
        if (iter != elementLoader->children().end()) {
            if (! this->parseElement(iter->second.get(), child)) {
                return false;
            }
        }
    }

    // Stop element.
    if (! elementLoader->onStopElement()) {
        return false;
    }

    return true;
}
