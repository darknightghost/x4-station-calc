#include <ui/main_window/editor_widget/group_item.h>

/**
 * @brief		Constructor.
 */
GroupItem::GroupItem(::std::shared_ptr<SaveGroup> group) : m_group(group)
{
    // Style.
    this->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable
                   | Qt::ItemIsEditable);
    this->updateGroupName();
}

/**
 * @brief		Get group.
 */
::std::shared_ptr<SaveGroup> GroupItem::group()
{
    return m_group;
}

/**
 * @brief		Update group name.
 */
void GroupItem::updateGroupName()
{
    this->setText(0, m_group->name());
}

/**
 * @brief		Destructor.
 */
GroupItem::~GroupItem() {}

/**
 * @brief		Constructor.
 */
GroupItemWidget::GroupItemWidget(GroupItem *item) : m_item(item)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_layout = new QHBoxLayout();
    this->setLayout(m_layout);

    m_btnUp = new SquareButton(QIcon(":/Icons/Up.png"));
    m_layout->addWidget(m_btnUp, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnUp, &QPushButton::clicked, this,
                  &GroupItemWidget::onUpBtnClicked);

    m_btnDown = new SquareButton(QIcon(":/Icons/Down.png"));
    m_layout->addWidget(m_btnDown, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnDown, &QPushButton::clicked, this,
                  &GroupItemWidget::onDownBtnClicked);

    m_btnRemove = new SquareButton(QIcon(":/Icons/EditRemove.png"));
    m_layout->addWidget(m_btnRemove, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &GroupItemWidget::onRemoveBtnClicked);

    m_layout->addStretch();
    this->setMaximumHeight(this->fontMetrics().height()
                           + m_layout->margin() * 2);
}

/**
 * @brief		Destructor.
 */
GroupItemWidget::~GroupItemWidget() {}

/**
 * @brief		Set enable status of "up" button.
 */
void GroupItemWidget::setUpBtnEnabled(bool enabled)
{
    m_btnUp->setEnabled(enabled);
}

/**
 * @brief		Set enable status of "down" button.
 */
void GroupItemWidget::setDownBtnEnabled(bool enabled)
{
    m_btnDown->setEnabled(enabled);
}

/**
 * @brief	On "up" button clicked.
 */
void GroupItemWidget::onUpBtnClicked()
{
    emit this->upBtnClicked(m_item);
}

/**
 * @brief	On "down" button clicked.
 */
void GroupItemWidget::onDownBtnClicked()
{
    emit this->downBtnClicked(m_item);
}

/**
 * @brief	On "remove" button clicked.
 */
void GroupItemWidget::onRemoveBtnClicked()
{
    emit this->removeBtnClicked(m_item);
}
