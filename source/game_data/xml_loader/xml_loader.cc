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
            qWarning() << "Missing attrbute \"sel\".";
            continue;
        }
        SelInfo sel = this->parseSel(diffElement.attribute("sel"));
        if (sel.path.size() == 0) {
            qWarning() << "Illegal value of attrbute \"sel\".";
            continue;
        }

        // Parse operation.
        if (diffElement.tagName() == "add") {
        } else if (diffElement.tagName() == "replace") {
        } else if (diffElement.tagName() == "remove") {
        } else {
            qWarning() << QString("Unknow element \"%1\" in \"diff\" element.")
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

    ::std::vector<QString> path;
    while (pathSplitted.size() > 1) {
        path.push_back(::std::move(pathSplitted.front()));
        pathSplitted.pop_front();
    }

    // Parse attribute.
    QRegExp expr("([a-zA-Z]\\w*)\\[@([a-zA-Z]\\w*)=['\"](.*)['\"]\\]");
    int     pos = expr.indexIn(pathSplitted.front());
    if (pos == 0 && expr.matchedLength() == pathSplitted.front().size()) {
        ret.attributes[expr.cap(2)] = expr.cap(3);
        path.push_back(expr.cap(1));

    } else {
        path.push_back(::std::move(pathSplitted.front()));
        pathSplitted.pop_front();
    }
    ret.path = ::std::move(path);

    return ret;
}
