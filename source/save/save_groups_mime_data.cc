#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <save/save.h>
#include <save/save_groups_mime_data.h>

/// Mime type.
const QString SaveGroupsMimeData::_type = "text/stationgroupslistv1";

/**
 * @brief		Constructor.
 */
SaveGroupsMimeData::SaveGroupsMimeData(
    const QVector<::std::shared_ptr<SaveGroup>> &groups)
{
    QJsonArray array;
    for (auto &group : groups) {
        array.append(group->toJson());
    }

    QJsonDocument doc;
    doc.setArray(array);
    this->setData(_type, doc.toJson());
}

/**
 * @brief		Get groups.
 */
QVector<::std::shared_ptr<SaveGroup>> SaveGroupsMimeData::groups()
{
    // Parse data.
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(this->data(_type), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << jsonError.errorString();
        return {};
    }

    if (! doc.isArray()) {
        return {};
    }

    QJsonArray root = doc.array();

    // Get groups.
    QVector<::std::shared_ptr<SaveGroup>> ret;
    for (auto value : root) {
        if (value.isObject()) {
            QJsonObject                  obj = value.toObject();
            ::std::shared_ptr<SaveGroup> group
                = SaveGroup::load(obj, Save::_currentVersion);
            if (group != nullptr) {
                ret.append(group);
            }
        }
    }

    return ret;
}

/**
 * @brief		Destructor.
 */
SaveGroupsMimeData::~SaveGroupsMimeData() {}
