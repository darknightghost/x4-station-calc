#pragma once

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <ui/main_window/action_control_dock_widget.h>

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
     * @brief		Destructor.
     */
    virtual ~StationModulesWidget();

  private slots:
    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief		\c m_btnShowHideFilter is clicked.
     */
    void onBtnShowHideFilterWidgetClicked();
};
