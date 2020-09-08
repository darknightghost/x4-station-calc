#include <ui/main_window/editor_widget/wares_item.h>

/**
 * @brief		Constructor.
 */
WaresItem::WaresItem() : QTreeWidgetItem()
{
    this->setExpanded(true);
}

/**
 * @brief       Check if the item has the ware.
 */
bool WaresItem::hasWare(const QString &macro)
{
    return m_macroMap.find(macro) != m_macroMap.end();
}

/**
 * @brief       Set ware range, if hte ware does not exists, the ware will
 *              be added to the item.
 */
void WaresItem::setWareAmountRange(const QString &macro, qint64 min, qint64 max)
{
    auto      iter = m_macroMap.find(macro);
    WareItem *item = nullptr;
    if (iter == m_macroMap.end()) {
        // Insert new ware.
        item = new WareItem(macro, min, max);
        this->QTreeWidgetItem::addChild(item);
        m_macroMap[macro] = item;

    } else {
        // Set range.
        item = *iter;
        item->setRange(min, max);
    }
}

/**
 * @brief       Remove the ware.
 */
void WaresItem::removeWare(const QString &macro)
{
    auto iter = m_macroMap.find(macro);
    if (iter != m_macroMap.end()) {
        WareItem *item = *iter;
        m_macroMap.erase(iter);
        this->QTreeWidgetItem::removeChild(item);
    }
}

/**
 * @brief		Change language.
 */
void WaresItem::onLanguageChanged()
{
    for (auto item : m_macroMap) {
        item->onLanguageChanged();
    }
}

/**
 * @brief		Destructor.
 */
WaresItem::~WaresItem() {}
