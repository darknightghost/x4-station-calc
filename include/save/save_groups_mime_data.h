#pragma once

#include <QtCore/QMimeData>
#include <QtCore/QVector>

#include <save/save_group.h>

/**
 * @brief		Mime data for groups.
 */
class SaveGroupsMimeData : public QMimeData {
    Q_OBJECT;

  public:
    static const QString _type; ///< Mime type.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groups		Groups.
     */
    SaveGroupsMimeData(const QVector<::std::shared_ptr<SaveGroup>> &groups);

    /**
     * @brief		Get groups.
     *
     * @return		Groups.
     */
    QVector<::std::shared_ptr<SaveGroup>> groups();

    /**
     * @brief		Destructor.
     */
    virtual ~SaveGroupsMimeData();
};
