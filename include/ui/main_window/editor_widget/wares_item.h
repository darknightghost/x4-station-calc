#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QTreeWidgetItem>

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
     * @param[in]   min         Minimum amount each hour.
     * @param[in]   max         Maxium amount each hour.
     */
    void setWareAmountRange(const QString &macro, qint64 min, qint64 max);

    /**
     * @brief       Remove the ware.
     *
     * @param[in]   macro       Macro of the ware.
     */
    void removeWare(const QString &macro);

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
