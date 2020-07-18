#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTreeWidgetItem>

#include <save/save_module.h>
#include <ui/controls/square_button.h>

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
    ModuleItem *  m_item;       ///< Item.
    QHBoxLayout * m_layout;     ///< Layout.
    QSpinBox *    m_spinAmount; ///< Spinbox amount.
    SquareButton *m_btnUp;      ///< Button up.
    SquareButton *m_btnDown;    ///< Button down.
    SquareButton *m_btnRemove;  ///< Button remove.

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

  private slots:
    /**
     * @brief	On "up" button clicked.
     */
    void onUpBtnClicked();

    /**
     * @brief	On "down" button clicked.
     */
    void onDownBtnClicked();

    /**
     * @brief	On "remove" button clicked.
     */
    void onRemoveBtnClicked();

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
     * @brief	"Up" button clicked.
     */
    void upBtnClicked();

    /**
     * @brief	"Down" button clicked.
     */
    void downBtnClicked();

    /**
     * @brief	"Remove" button clicked.
     */
    void removeBtnClicked();

    /**
     * @brief	    Amount changed.
     *
     * @param[in]   oldAmount       Old amount.
     * @param[in]   newAmount       New amount.
     */
    void changeAmount(quint64 oldAmount, quint64 newAmount);
};
