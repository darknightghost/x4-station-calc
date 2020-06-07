#pragma once

#include <QtCore/QVector>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <ui/main_window/action_control_dock_widget.h>
#include <ui/main_window/station_modules_widget/station_modules_tree_widget_item.h>

/**
 * @brief	Station module.
 */
class StationModulesWidget : public ActionControlDockWidget {
    Q_OBJECT
  protected:
    QWidget *    m_widget; ///< Widget.
    QVBoxLayout *m_layout; ///< Layout.

    // Button bar.
    QWidget *    m_widgetButtonBar;   ///< Button bar.
    QHBoxLayout *m_layoutButtonBar;   ///< Button bar layout.
    QPushButton *m_btnShowHideFilter; ///< Show/hide filters button.
    QPushButton *m_btnAddToStation;   ///< Add to station button.

    // Filters.
    QWidget *    m_widgetFilters; ///< Filters widget.
    QGridLayout *m_layoutFilters; ///< Filters layout.

    QCheckBox *m_chkByRace;    ///< By race checkbox.
    QComboBox *m_comboByRaces; ///< Combobox to select race.

    QCheckBox *m_chkByProduction;   ///< By production checkbox.
    QComboBox *m_comboByProduction; ///< Combobox to selecrt production.

    QCheckBox *m_chkByKeyword; ///< By production checkbox.
    QLineEdit *m_txtKeyword;   ///< Text box to input keyword.

    // Station modules.
    QTreeWidget *m_treeStationModules; ///< Tree view to select station module.
    QTreeWidgetItem *m_itemBuild;      ///< Build modules root item.
    QTreeWidgetItem *m_itemDock;       ///< Dock modules root item.
    QTreeWidgetItem *m_itemProduction; ///< Production modules root item.
    QTreeWidgetItem *m_itemStorage;    ///< Storage modules root item.
    QTreeWidgetItem *m_itemHabitation; ///< Habitation modules root item.
    QTreeWidgetItem *m_itemDefence;    ///< Defence modules root item.
    QTreeWidgetItem *m_itemConnect;    ///< Connect modules root item.

    QVector<StationModulesTreeWidgetItem *> m_moduleItems; ///< Module items.
    QSet<QString>                           m_races;       ///< Races.
    QSet<QString>                           m_products;    ///< Products.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	statusAction	Action to control the visibility of the
     *								widget.
     * @param[in]	parent			Parent object.
     * @param[in]	flags			Window flags.
     */
    StationModulesWidget(QAction *       statusAction,
                         QWidget *       parent = nullptr,
                         Qt::WindowFlags flags  = Qt::WindowFlags());

    /**
     * @brief		Set enable status of add to station button.
     *
     * @param[in]	enabled		Enable status.
     */
    void setAddToStationStatus(bool enabled);

    /**
     * @brief		Destructor.
     */
    virtual ~StationModulesWidget();

  signals:
    /**
     * @brief		Emit when a station module has been clicked.
     *
     * @param[in]	macro		Macro of the module.
     * @param[in]	falseVal	Always \c false.
     */
    void stationModuleClicked(QString macro, bool falseVal);

    /**
     * @brief		Emit when add to station button clicked.
     *
     * @param[in]	macros	Macros of the selected modules.
     */
    void addToStation(QStringList macros);

  private:
    /**
     * @brief	Load all station modules.
     */
    void loadStationModules();

    /**
     * @brief	Sort combobox.
     */
    void sortComboBox(QComboBox *combo);

  private slots:

    /**
     * @brief	Filter station modules.
     */
    void filterModules();

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief		\c m_btnShowHideFilter is clicked.
     */
    void onBtnShowHideFilterWidgetClicked();

    /**
     * @brief		\c m_chkByRace status changed.
     *
     * @param[in]	status		New status.
     */
    void onByRaceChkChanged(int status);

    /**
     * @brief		\c m_chkByProduction status changed.
     *
     * @param[in]	status		New status.
     */
    void onByProductionChkChanged(int status);

    /**
     * @brief		\c m_chkByKeyword status changed.
     *
     * @param[in]	status		New status.
     */
    void onByKeywordChkChanged(int status);

    /**
     * @brief		On button add to station clicked.
     */
    void onAddToStationClicked();

    /**
     * @brief		On item clicked.
     *
     * @param[in]	item		Item clicked.
     */
    void onItemClicked(QTreeWidgetItem *item);
};
