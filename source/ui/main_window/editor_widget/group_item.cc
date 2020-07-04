#include <ui/main_window/editor_widget/group_item.h>

/**
 * @brief		Constructor.
 */
GroupItem::GroupItem(::std::shared_ptr<SaveGroup> group) : m_group(group)
{
    this->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable);
    this->setText(0, group->name());
}

/**
 * @brief		Get group.
 */
::std::shared_ptr<SaveGroup> GroupItem::group()
{
    return m_group;
}

/**
 * @brief		Destructor.
 */
GroupItem::~GroupItem() {}
