#pragma once

#include <QtWidgets/QTreeWidgetItem>

/**
 * @brief       QTreeWidgetItem with other locales support.
 */
class QTreeWidgetItemLocale : public QTreeWidgetItem {
  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   other       Other item to copy.
     */
    QTreeWidgetItemLocale(const QTreeWidgetItem &other);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent item.
     * @param[in]   preceding   Previous item.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidgetItem *parent,
                          QTreeWidgetItem *preceding,
                          int              type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent item.
     * @param[in]   strings     Strings in each column.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidgetItem *  parent,
                          const QStringList &strings,
                          int                type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent item.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidgetItem *parent, int type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Tree widget.
     * @param[in]   preceding   Previous item.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidget *    parent,
                          QTreeWidgetItem *preceding,
                          int              type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Tree widget.
     * @param[in]   strings     Strings in each column.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidget *      parent,
                          const QStringList &strings,
                          int                type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Tree widget.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(QTreeWidget *parent, int type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   strings     Strings in each column.
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(const QStringList &strings, int type = Type);

    /**
     * @brief       Constructor.
     *
     * @param[in]   type        Item type
     */
    QTreeWidgetItemLocale(int type = Type);

    /**
     * @brief       Destructor.
     */
    virtual ~QTreeWidgetItemLocale();

  public:
    /**
     * @brief       Compare.
     *
     * @param[in]   other       Other item to compare.
     */
    virtual bool operator<(const QTreeWidgetItem &other) const override;
};
