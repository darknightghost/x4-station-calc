#include <QtCore/QSet>

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
 * @brief       Get count of wares.
 */
quint64 WaresItem::wareCount() const
{
    return m_macroMap.size();
}

/**
 * @brief       Set ware range, if hte ware does not exists, the ware will
 *              be added to the item.
 */
void WaresItem::setWareAmountRange(const QString &      macro,
                                   const Range<qint64> &range)
{
    auto      iter = m_macroMap.find(macro);
    WareItem *item = nullptr;
    if (iter == m_macroMap.end()) {
        // Insert new ware.
        item = new WareItem(macro, range);
        this->QTreeWidgetItem::addChild(item);
        m_macroMap[macro] = item;
    } else {
        // Set range.
        item = *iter;
        item->setRange(range);
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
 * @brief       Update wares.
 */
void WaresItem::update(const QMap<QString, Range<long double>> &wares)
{
    // Remove old wares.
    for (auto &key : m_macroMap.keys()) {
        if (wares.find(key) == wares.end()) {
            this->QTreeWidgetItem::removeChild(m_macroMap[key]);
            m_macroMap.remove(key);
        }
    }

    // Set wares range.
    for (auto iter = wares.begin(); iter != wares.end(); ++iter) {
        this->setWareAmountRange(iter.key(),
                                 Range<qint64>(qRound((double)(iter->min())),
                                               qRound((double)(iter->max()))));
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
