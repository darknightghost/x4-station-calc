#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTreeWidgetItem>

#include <save/save_group.h>
#include <ui/controls/square_button.h>

/**
 * @brief	Item of modules group.
 */
class GroupItem : public QTreeWidgetItem {
  private:
    ::std::shared_ptr<SaveGroup> m_group; ///<  Group.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	group		Group.
     */
    GroupItem(::std::shared_ptr<SaveGroup> group);

    /**
     * @brief		Get group.
     *
     * @return		Group.
     */
    ::std::shared_ptr<SaveGroup> group();

    /**
     * @brief		Update group name to text.
     */
    void updateGroupName();

    /**
     * @brief		Destructor.
     */
    virtual ~GroupItem();
};

/**
 * @brief	Item of modules group.
 */
class GroupItemWidget : public QWidget {
    Q_OBJECT;

  private:
    GroupItem *   m_item;      ///< Item.
    QHBoxLayout * m_layout;    ///< Layout.
    SquareButton *m_btnUp;     ///< Button up.
    SquareButton *m_btnDown;   ///< Button down.
    SquareButton *m_btnRemove; ///< Button remove.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	item	Item.
     */
    GroupItemWidget(GroupItem *item);

    /**
     * @brief		Destructor.
     */
    virtual ~GroupItemWidget();

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

  signals:
    /**
     * @brief	    "Up" button clicked.
     *
     * @param[in]   item    Item.
     */
    void upBtnClicked(GroupItem *item);

    /**
     * @brief	    "Down" button clicked.
     *
     * @param[in]   item    Item.
     */
    void downBtnClicked(GroupItem *item);

    /**
     * @brief	    "Remove" button clicked.
     *
     * @param[in]   item    Item.
     */
    void removeBtnClicked(GroupItem *item);
};
