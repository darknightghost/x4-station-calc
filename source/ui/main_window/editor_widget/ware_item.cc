#include <game_data/game_data.h>
#include <ui/main_window/editor_widget/ware_item.h>

/**
 * @brief       Constructor.
 */
WareItem::WareItem(const QString &ware, const Range<qint64> &range) :
    QTreeWidgetItem(), m_ware(ware), m_range(range)
{
    this->setFlags(Qt::ItemFlag::ItemIsEnabled);
    this->setText(1,
                  QString("%1/h - %2/h").arg(m_range.min()).arg(m_range.max()));
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
void WareItem::setRange(const Range<qint64> &range)
{
    m_range = range;
    this->setText(1,
                  QString("%1/h - %2/h").arg(m_range.min()).arg(m_range.max()));
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
