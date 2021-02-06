#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QVector>

#include <save/save.h>

/**
 * @brief       Clipboard data builder.
 */
class X4SCGroupClipboardMimeDataBuilder
{
  public:
    static const QString _mimeTypeStr; ///< Mime type.

  private:
    QJsonObject m_jsonRoot; ///< Root json object.

  public:
    /**
     * @brief       Constructor.
     */
    X4SCGroupClipboardMimeDataBuilder();

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
     * @param[in]   groups      Module groups.
     */
    void setData(const QVector<::std::shared_ptr<const SaveGroup>> &groups);

    /**
     * @brief       Get groups.
     *
     * @return      Groups.
     */
    QVector<::std::shared_ptr<SaveGroup>> groups() const;

    /**
     * @brief       Destructor.
     */
    virtual ~X4SCGroupClipboardMimeDataBuilder();
};
