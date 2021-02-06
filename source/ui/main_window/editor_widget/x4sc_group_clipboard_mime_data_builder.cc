#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <save/save.h>
#include <ui/main_window/editor_widget/x4sc_group_clipboard_mime_data_builder.h>

/// Mime type.
const QString X4SCGroupClipboardMimeDataBuilder::_mimeTypeStr
    = "text/x4-station-group-clipboard";

/**
 * @brief       Constructor.
 */
X4SCGroupClipboardMimeDataBuilder::X4SCGroupClipboardMimeDataBuilder()
{
    m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[]}").object();
    m_jsonRoot.insert("version", (QString)Save::_currentVersion);
}

/**
 * @brief       Load data from mime data.
 */
void X4SCGroupClipboardMimeDataBuilder::loadMimeData(const QMimeData *mimeData)
{
    QString       jsonStr = mimeData->data(_mimeTypeStr);
    QJsonDocument doc     = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isNull()) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[]}").object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    QJsonValue versionValue = doc.object().value("version");
    if (! versionValue.isString()) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[]}").object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    SaveVersion version(versionValue.toString());

    if (version > Save::_currentVersion) {
        m_jsonRoot = QJsonDocument::fromJson("{\"groups\":[]}").object();
        m_jsonRoot.insert("version", (QString)Save::_currentVersion);
        return;
    }

    // Get groups.
    QJsonValue groupValue = doc.object().value("groups");
    if (groupValue.isArray()) {
        m_jsonRoot.insert("groups", groupValue);
    }
}

/**
 * @brief       Load data from mime data.
 */
void X4SCGroupClipboardMimeDataBuilder::saveMimeData(QMimeData *mimeData) const
{
    QJsonDocument doc(m_jsonRoot);
    mimeData->setData(_mimeTypeStr, doc.toJson());
}

/**
 * @brief       Set data.
 */
void X4SCGroupClipboardMimeDataBuilder::setData(
    const QVector<::std::shared_ptr<const SaveGroup>> &groups)
{
    // Groups.
    QJsonArray groupsArray;
    for (auto &group : groups) {
        groupsArray.append(group->toJson());
    }
    m_jsonRoot.insert("groups", groupsArray);
    m_jsonRoot.insert("version", (QString)Save::_currentVersion);
}

/**
 * @brief       Get groups.
 */
QVector<::std::shared_ptr<SaveGroup>>
    X4SCGroupClipboardMimeDataBuilder::groups() const
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
 * @brief       Destructor.
 */
X4SCGroupClipboardMimeDataBuilder::~X4SCGroupClipboardMimeDataBuilder() {}
