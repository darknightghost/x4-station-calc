#pragma once

#include <QtCore/QMimeData>
#include <QtCore/QVector>

#include <save/save.h>

class X4SCClipboardMimeDataBuilder {
  private:
  public:
    /**
     * @brief       Constructor.
     */
    X4SCClipboardMimeDataBuilder();

    /**
     * @brief       Constructor.
     *
     * @param[in]   groups      Module groups.
     * @param[in]   modules     Modules.
     */
    X4SCClipboardMimeDataBuilder(
        const QVector<::std::shared_ptr<SaveGroup>> & groups,
        const QVector<::std::shared_ptr<SaveModule>> &modules);

    /**
     * @brief       Destructor.
     */
    virtual ~X4SCClipboardMimeDataBuilder();
};
