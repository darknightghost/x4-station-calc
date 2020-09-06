#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QVector>

#include <save/save.h>

/**
 * @brief       Clipboard data builder.
 */
class X4SCModuleClipboardMimeDataBuilder {
  public:
    static const QString _mimeTypeStr; ///< Mime type.

  private:
    QJsonObject m_jsonRoot; ///< Root json object.

  public:
    /**
     * @brief       Constructor.
     */
    X4SCModuleClipboardMimeDataBuilder();

    /**
     * @brief       Load data from mime data.
     *
     * @param[in]   mimeData        Mime data to load.
     */
    void loadMimeData(const QMimeData *mimeData);

    /**
     * @brief       Load data from mime data.
     *
     * @param[in]   mimeData        Mime data to load.
     */
    void saveMimeData(QMimeData *mimeData) const;

    /**
     * @brief       Set data.
     *
     * @param[in]   modules     Modules.
     */
    void setData(const QVector<::std::shared_ptr<const SaveModule>> &modules);

    /**
     * @brief       Get modules.
     *
     * @return      Modules.
     */
    QVector<::std::shared_ptr<SaveModule>> modules() const;

    /**
     * @brief       Destructor.
     */
    virtual ~X4SCModuleClipboardMimeDataBuilder();
};
