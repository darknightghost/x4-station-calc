#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTreeWidgetItem>

#include <save/save_group.h>
#include <ui/controls/square_button.h>
#include <ui/main_window/editor_widget/module_item.h>

/**
 * @brief	Item of modules group.
 */
class GroupItem : public QTreeWidgetItem {
  private:
    ::std::shared_ptr<SaveGroup> m_group;    ///<  Group.
    QMap<QString, ModuleItem *>  m_macroMap; ///< Macro/ModuleItemMap.

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

  public:
    /**
     * @brief       Appends the child item to the list of children.
     *
     * @param[in]   child       Child to append.
     *
     * @return      On success, the method returns \c true, otherwise returns
     *              \c false.
     */
    bool addChild(ModuleItem *child);

    /**
     * @brief       Returns the item at the given index in the list of the
     *              item's children.
     *
     * @param[in]   index       Index of the child.
     */
    ModuleItem *child(int index) const;

    /**
     * @brief       Returns the item which macro of the module is \c macro in
     *              the list of the item's children.
     *
     * @param[in]   macro       Macro of the module.
     */
    ModuleItem *child(const QString &macro) const;

    /**
     * @brief       Get the index of the child.
     *
     * @param[in]   child       Child.
     */
    int indexOfChild(ModuleItem *child) const;

    /**
     * @brief       Insert child.
     *
     * @param[in]   index       Index of the new child.
     * @param[in]   child       Child.
     *
     * @return      On success, the method returns \c true, otherwise returns
     *              \c false.
     */
    bool insertChild(int index, ModuleItem *child);

    /**
     * @brief       Remove child.
     *
     * @param[in]   child       Child to remove.
     */
    void removeChild(ModuleItem *child);

    /**
     * @brief       Take child.
     *
     * @param[in]   index       Index of child to take.
     */
    ModuleItem *takeChild(int index);

    void addChild(QTreeWidgetItem *child)                      = delete;
    void addChildren(const QList<ModuleItem *> &children)      = delete;
    void addChildren(const QList<QTreeWidgetItem *> &children) = delete;
    int  indexOfChild(QTreeWidgetItem *child) const            = delete;
    void insertChildren(int index, const QList<ModuleItem *> &children)
        = delete;
    void insertChildren(int index, const QList<QTreeWidgetItem *> &children)
        = delete;
    void                insertChild(int index, QTreeWidgetItem *child) = delete;
    void                removeChild(QTreeWidgetItem *child)            = delete;
    QList<ModuleItem *> takeChildren()                                 = delete;
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
    void onBtnUpClicked();

    /**
     * @brief	On "down" button clicked.
     */
    void onBtnDownClicked();

    /**
     * @brief	On "remove" button clicked.
     */
    void onBtnRemoveClicked();

  signals:
    /**
     * @brief	    "Up" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnUpClicked(GroupItem *item);

    /**
     * @brief	    "Down" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnDownClicked(GroupItem *item);

    /**
     * @brief	    "Remove" button clicked.
     *
     * @param[in]   item    Item.
     */
    void btnRemoveClicked(GroupItem *item);
};
