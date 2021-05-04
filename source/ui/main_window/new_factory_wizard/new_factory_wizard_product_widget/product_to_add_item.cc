#include <game_data/game_data.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/product_to_add_item.h>

/**
 * @brief       Constructor.
 */
ProductToAddItem::ProductToAddItem(const QString &ware) : m_ware(ware)
{
    auto gameData = GameData::instance();
    auto texts    = gameData->texts();
    auto wares    = gameData->wares();

    this->setText(0, texts->text(wares->ware(m_ware)->name));
}

/**
 * @brief   Get ware ID.
 */
const QString &ProductToAddItem::ware() const
{
    return m_ware;
}

/**
 * @brief       Destructor.
 */
ProductToAddItem::~ProductToAddItem() {}
