#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTreeWidgetItem>

#include <save/save_module.h>
#include <ui/controls/square_button.h>
#include <ui/main_window/editor_widget/amount_spin_box.h>

/**
 * @brief	Item of modules module.
 */
class ModuleItem : public QTreeWidgetItem {
  private:
    ::std::shared_ptr<SaveModule> m_module; ///<  Module.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	module		Module.
     */
    ModuleItem(::std::shared_ptr<SaveModule> module);

    /**
     * @brief		Get module.
     *
     * @return		Module.
     */
    ::std::shared_ptr<SaveModule> module();

    /**
     * @brief		Get module amount.
     */
    quint64 moduleAmount();

    /**
     * @brief		Set module amount.
     */
    void setModuleAmount(quint64 amount);

    /**
     * @brief		Update module name.
     */
    void updateName();

    /**
     * @brief		Destructor.
     */
    virtual ~ModuleItem();
};

/**
 * @brief	Item of modules module.
 */
class ModuleItemWidget : public QWidget {
    Q_OBJECT;

  private:
    ModuleItem *   m_item;                  ///< Item.
    QHBoxLayout *  m_layout;                ///< Layout.
    AmountSpinBox *m_spinAmount;            ///< Spinbox amount.
    SquareButton * m_btnUp;                 ///< Button up.
    SquareButton * m_btnDown;               ///< Button down.
    SquareButton * m_btnRemove;             ///< Button remove.
    QPushButton *m_btnSetToSuggestedAmount; ///< Button set to suggested amount.

    int m_suggestedAmountToChange; ///< Suggested amount to change.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	item	Item.
     */
    ModuleItemWidget(ModuleItem *item);

    /**
     * @brief		Destructor.
     */
    virtual ~ModuleItemWidget();

  public slots:
    /**
     * @brief		Set enable status of "up" button.
     *
     * @param[in]	enabled		Enable status.
     */
    void setUpBtnEnabled(bool enabled);

    /**
     * @brief		Set enable status of "down" button.
     *
     * @param[in]	enabled		Enable status.
     */
    void setDownBtnEnabled(bool enabled);

    /**
     * @brief       Update amount.
     */
    void updateAmount();

    /**
     * @brief       Set suggest amount enable status.
     *
     * @param[in]   enabled     Enable status.
     */
    void setSuggestAmountEnabled(bool enabled);

    /**
     * @brief       Set suggested amount to change.
     *
     * @param[in]   productPerHourToChange      Suggested value to change of the
     *                                          amount of product each hour.
     */
    void setSuggestedAmountToChange(qint64 productPerHourToChange);

  private slots:
    /**
     * @brief	On "up" button clicked.
     */
    void onBtnUpClicked();

    /**
     * @brief	On "down" button clicked.
     */
    void onBtnDownClicked();

    /**
     * @brief	On "remove" button clicked.
     */
    void onBtnRemoveClicked();

    /**
     * @brief   On "Set to Suggested Amount" button clicked.
     */
    void onBtnSetToSuggestedAmountClicked();

    /**
     * @brief	    On value of spinbox changed.
     *
     * @param[in]   i   New value.
     */
    void onSpinboxValueChanged(int i);

    /**
     * @brief       On language changed.
     */
    void onLanguageChanged();

  signals:
    /**
     * @brief	    "Up" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnUpClicked(ModuleItem *item);

    /**
     * @brief	    "Down" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnDownClicked(ModuleItem *item);

    /**
     * @brief	    "Remove" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnRemoveClicked(ModuleItem *item);

    /**
     * @brief	    Amount changed.
     *
     * @param[in]   oldAmount       Old amount.
     * @param[in]   newAmount       New amount.
     * @param[in]   item            Item.
     */
    void changeAmount(quint64 oldAmount, quint64 newAmount, ModuleItem *item);

  private:
    /**
     * @brief       Compute suggested amount to change.
     *
     * @param[in]   productPerHourToChange      Suggested value to change of the
     *                                          amount of product each hour.
     *
     * @return      Suggested amount.
     */
    int suggestedAmountToChange(qint64 productPerHourToChange);
};
