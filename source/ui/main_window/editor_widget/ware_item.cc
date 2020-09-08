#include <game_data/game_data.h>
#include <ui/main_window/editor_widget/ware_item.h>

/**
 * @brief       Constructor.
 */
WareItem::WareItem(const QString &ware, qint64 min, qint64 max) :
    QTreeWidgetItem(), m_ware(ware), m_min(min), m_max(max)
{
    this->setText(1, QString("%1/h - %2/h").arg(m_min).arg(m_max));
    this->onLanguageChanged();
}

/**
 * @brief       Get the macro of the ware.
 */
const QString &WareItem::ware() const
{
    return m_ware;
}

/**
 * @brief       Set range.
 */
void WareItem::setRange(qint64 min, qint64 max)
{
    m_min = min;
    m_max = max;
    this->setText(1, QString("%1/h - %2/h").arg(m_min).arg(m_max));
}

/**
 * @brief       Called when languaged changed.
 */
void WareItem::onLanguageChanged()
{
    ::std::shared_ptr<GameData>  gameData  = GameData::instance();
    ::std::shared_ptr<GameWares> gameWares = gameData->wares();
    ::std::shared_ptr<GameTexts> gameTexts = gameData->texts();
    this->setText(0, gameTexts->text(gameWares->ware(m_ware)->name));
}

/**
 * @brief       Destructor.
 */
WareItem::~WareItem() {}
