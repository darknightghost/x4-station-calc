#pragma once

#include <ui/locale/q_tree_widget_item_locale.h>

/**
 * @brief   Product to add.
 */
class ProductToAddItem : public QTreeWidgetItemLocale {
  private:
    QString m_ware; ///< Ware ID.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ware    Ware.
     */
    ProductToAddItem(const QString &ware);

    /**
     * @brief   Get ware ID.
     *
     * @return  Ware ID.
     */
    const QString &ware() const;

    /**
     * @brief       Destructor.
     */
    virtual ~ProductToAddItem();
};
