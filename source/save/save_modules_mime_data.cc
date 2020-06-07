#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <save/save.h>
#include <save/save_modules_mime_data.h>

/// Mime type.
const QString SaveModulesMimeData::_type = "text/stationmoduleslistv1";

/**
 * @brief		Constructor.
 */
SaveModulesMimeData::SaveModulesMimeData(
    const QVector<::std::shared_ptr<SaveModule>> &modules)
{
    QJsonArray array;
    for (auto &module : modules) {
        array.append(module->toJson());
    }

    QJsonDocument doc;
    doc.setArray(array);
    this->setData(_type, doc.toJson());
}

/**
 * @brief		Get modules.
 */
QVector<::std::shared_ptr<SaveModule>> SaveModulesMimeData::modules()
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

    // Get modules.
    QVector<::std::shared_ptr<SaveModule>> ret;
    for (auto value : root) {
        if (value.isObject()) {
            QJsonObject                   obj = value.toObject();
            ::std::shared_ptr<SaveModule> module
                = SaveModule::load(obj, Save::_currentVersion);
            if (module != nullptr) {
                ret.append(module);
            }
        }
    }

    return ret;
}

/**
 * @brief		Destructor.
 */
SaveModulesMimeData::~SaveModulesMimeData() {}
