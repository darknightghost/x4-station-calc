#pragma once

#include <QtWidgets/QTreeWidgetItem>

#include <save/save_group.h>

/**
 * @brief	Item of modules group.
 */
class GroupItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT;

  private:
    ::std::shared_ptr<SaveGroup> m_group; ///<  Group.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	group		Group.
     */
    GroupItem(::std::shared_ptr<SaveGroup>);

    /**
     * @brief		Get group.
     *
     * @return		Group.
     */
    ::std::shared_ptr<SaveGroup> group();

    /**
     * @brief		Destructor.
     */
    virtual ~GroupItem();

  signals:
    /**
     * @brief	Move up.
     */
    void moveUp(GroupItem *item);

    /**
     * @brief	Move down.
     */
    void moveDown(GroupItem *item);

  public slots:
    /**
     * @brief		Set enable status of up button.
     *
     * @param[in]	enabled		Enable status.
     */
    void setMoveUpEnabled(bool enabled);

    /**
     * @brief	Set enable status of down button.
     *
     * @param[in]	enabled		Enable status.
     */
    void setMoveDownEnabled(bool enabled);
};
