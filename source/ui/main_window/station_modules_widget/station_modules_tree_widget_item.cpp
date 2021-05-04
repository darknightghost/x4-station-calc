#include <locale/string_table.h>
#include <ui/main_window/station_modules_widget/station_modules_tree_widget_item.h>

/**
 * @brief		Constructor.
 */
StationModulesTreeWidgetItem::StationModulesTreeWidgetItem(
    QTreeWidgetItem *                                    parent,
    ::std::shared_ptr<GameStationModules::StationModule> module) :
    QTreeWidgetItemLocale(parent),
    m_module(module)
{}

/**
 * @brief		Get item module.
 */
::std::shared_ptr<GameStationModules::StationModule>
    StationModulesTreeWidgetItem::module()
{
    return m_module;
}

/**
 * @brief	Destructor.
 */
StationModulesTreeWidgetItem::~StationModulesTreeWidgetItem() {}

/**
 * @brief		Change language.
 */
void StationModulesTreeWidgetItem::onLanguageChanged()
{
    this->setText(0, GameData::instance()->texts()->text(m_module->name));
}
