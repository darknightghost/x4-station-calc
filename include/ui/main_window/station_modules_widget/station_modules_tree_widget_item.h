#pragma once

#include <functional>

#include <QtWidgets/QTreeWidgetItem>

#include <game_data/game_data.h>
#include <locale/string_table.h>

/**
 * @brief	Tree widget item of station modules.
 */
class StationModulesTreeWidgetItem : public QTreeWidgetItem {
  protected:
    ::std::shared_ptr<GameStationModules::StationModule>
        m_module; ///< Station module.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent.
     * @param[in]	module		Station module.
     */
    StationModulesTreeWidgetItem(
        QTreeWidgetItem *                                    parent,
        ::std::shared_ptr<GameStationModules::StationModule> module);

    /**
     * @brief		Get item module.
     *
     * @return		Module.
     */
    ::std::shared_ptr<GameStationModules::StationModule> module();

    /**
     * @brief	Destructor.
     */
    virtual ~StationModulesTreeWidgetItem();

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief		Operator <.
     *
     * @param[in]	other		Other item to compare.
     *
     * @return		Compare result.
     */
    virtual bool operator<(const QTreeWidgetItem &other) const override;
};
