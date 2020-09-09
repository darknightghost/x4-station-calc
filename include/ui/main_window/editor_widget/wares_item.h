#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QTreeWidgetItem>

#include <common.h>
#include <ui/main_window/editor_widget/ware_item.h>

/**
 * @brief	Item of game wares.
 */
class WaresItem : public QTreeWidgetItem {
  private:
    QMap<QString, WareItem *> m_macroMap; ///< Macro/Ware item map.

  public:
    /**
     * @brief		Constructor.
     */
    WaresItem();

    /**
     * @brief       Check if the item has the ware.
     *
     * @param[in]   macro       Macro of the ware.
     */
    bool hasWare(const QString &macro);

    /**
     * @brief       Set ware range, if hte ware does not exists, the ware will
     *              be added to the item.
     *
     * @param[in]   macro       Macro of the ware.
     * @param[in]   range       Range of the amount each hour.
     */
    void setWareAmountRange(const QString &macro, const Range<qint64> &range);

    /**
     * @brief       Remove the ware.
     *
     * @param[in]   macro       Macro of the ware.
     */
    void removeWare(const QString &macro);

    /**
     * @brief       Update wares.
     *
     * @param[in]   wares       Macros and ranges of the wares.
     */
    void update(const QMap<QString, Range<long double>> &wares);

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief		Destructor.
     */
    virtual ~WaresItem();

  public:
    void             addChild(QTreeWidgetItem *child)               = delete;
    QTreeWidgetItem *child(int index) const                         = delete;
    QTreeWidgetItem *child(const QString &macro) const              = delete;
    int              indexOfChild(QTreeWidgetItem *child) const     = delete;
    void             insertChild(int index, QTreeWidgetItem *child) = delete;
    void             removeChild(QTreeWidgetItem *child)            = delete;
    QTreeWidgetItem *takeChild(int index)                           = delete;
    void addChildren(const QList<QTreeWidgetItem *> &children)      = delete;
    void insertChildren(int index, const QList<QTreeWidgetItem *> &children)
        = delete;
    QList<QTreeWidgetItem *> takeChildren() = delete;
};
