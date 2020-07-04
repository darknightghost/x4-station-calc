#pragma once

#include <QtWidgets/QTreeWidgetItem>

#include <save/save_group.h>

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
};
