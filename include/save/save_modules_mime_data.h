#pragma once

#include <QtCore/QMimeData>
#include <QtCore/QVector>

#include <save/save_module.h>

/**
 * @brief		Mime data for modules.
 */
class SaveModulesMimeData : public QMimeData {
    Q_OBJECT;

  public:
    static const QString _type; ///< Mime type.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	modules		Modules.
     */
    SaveModulesMimeData(const QVector<::std::shared_ptr<SaveModule>> &modules);

    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    QVector<::std::shared_ptr<SaveModule>> modules();

    /**
     * @brief		Destructor.
     */
    virtual ~SaveModulesMimeData();
};
