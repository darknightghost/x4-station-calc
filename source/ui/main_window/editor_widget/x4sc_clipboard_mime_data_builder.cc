#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <save/save.h>
#include <ui/main_window/editor_widget/x4sc_clipboard_mime_data_builder.h>

/// Mime type.
const QString X4SCClipboardMimeDataBuilder::_mimeTypeStr
    = "text/x4-station-clipboard";

/**
 * @brief       Constructor.
 */
X4SCClipboardMimeDataBuilder::X4SCClipboardMimeDataBuilder()
{
    m_jsonRoot
        = QJsonDocument::fromJson("{\"groups\":[],\"modules\":[]}").object();
    m_jsonRoot.insert("version", (QString)Save::_currentVersion);
}

/**
 * @brief       Load data from mime data.
 */
void X4SCClipboardMimeDataBuilder::loadMimeData(const QMimeData *mimeData)
{
    QString       jsonStr = mimeData->data(_mimeTypeStr);
    QJsonDocument doc     = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull()) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[],\"modules\":[]}")
                         .object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    QJsonValue versionValue = doc.object().value("version");
    if (! versionValue.isString()) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[],\"modules\":[]}")
                         .object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    SaveVersion version(versionValue.toString());

    if (version > Save::_currentVersion) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[],\"modules\":[]}")
                         .object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    // Get groups.
    QJsonValue groupValue = doc.object().value("groups");
    if (groupValue.isArray()) {
        m_jsonRoot.insert("groups", groupValue);
    }

    // Get modules.
    QJsonValue moduleValue = doc.object().value("modules");
    if (moduleValue.isArray()) {
        m_jsonRoot.insert("modules", moduleValue);
    }
}

/**
 * @brief       Load data from mime data.
 */
void X4SCClipboardMimeDataBuilder::saveMimeData(QMimeData *mimeData) const
{
    QJsonDocument doc(m_jsonRoot);
    mimeData->setData(_mimeTypeStr, doc.toJson());
}

/**
 * @brief       Set data.
 */
void X4SCClipboardMimeDataBuilder::setData(
    const QVector<::std::shared_ptr<const SaveGroup>> & groups,
    const QVector<::std::shared_ptr<const SaveModule>> &modules)
{
    // Groups.
    QJsonArray groupsArray;
    for (auto &group : groups) {
        groupsArray.append(group->toJson());
    }
    m_jsonRoot.insert("groups", groupsArray);

    // Modules
    QJsonArray modulesArray;
    for (auto &module : modules) {
        modulesArray.append(module->toJson());
    }
    m_jsonRoot.insert("modules", modulesArray);

    m_jsonRoot.insert("version", (QString)Save::_currentVersion);
}

/**
 * @brief       Get groups.
 */
QVector<::std::shared_ptr<SaveGroup>>
    X4SCClipboardMimeDataBuilder::groups() const
{
    SaveVersion version(m_jsonRoot.value("version").toString());
    QVector<::std::shared_ptr<SaveGroup>> groups;

    QJsonValue value = m_jsonRoot.value("groups");
    if (value.isArray()) {
        for (auto groupValue : value.toArray()) {
            if (groupValue.isObject()) {
                QJsonObject groupObject = groupValue.toObject();
                ::std::shared_ptr<SaveGroup> group
                    = SaveGroup::load(groupObject, version);
                if (group != nullptr) {
                    groups.append(group);
                }
            }
        }
    }

    return groups;
}

/**
 * @brief       Get modules.
 */
QVector<::std::shared_ptr<SaveModule>>
    X4SCClipboardMimeDataBuilder::modules() const
{
    SaveVersion version(m_jsonRoot.value("version").toString());
    QVector<::std::shared_ptr<SaveModule>> modules;

    QJsonValue value = m_jsonRoot.value("modules");
    if (value.isArray()) {
        for (auto moduleValue : value.toArray()) {
            if (moduleValue.isObject()) {
                QJsonObject moduleObject = moduleValue.toObject();
                ::std::shared_ptr<SaveModule> module
                    = SaveModule::load(moduleObject, version);
                if (module != nullptr) {
                    modules.append(module);
                }
            }
        }
    }

    return modules;
}

/**
 * @brief       Destructor.
 */
X4SCClipboardMimeDataBuilder::~X4SCClipboardMimeDataBuilder() {}
