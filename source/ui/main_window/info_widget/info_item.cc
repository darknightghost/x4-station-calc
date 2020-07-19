#include <ui/controls/transparent_label.h>
#include <ui/main_window/info_widget/info_item.h>

/**
 * @brief		Constructor.
 */
InfoItem::InfoItem(::std::unique_ptr<GenericString> first) :
    m_first(::std::move(first))
{
    this->setFlags(Qt::ItemFlag::ItemIsEnabled);
    this->setTextAlignment(0, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
    this->setTextAlignment(1, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
}

/**
 * @brief		Constructor.
 */
InfoItem::InfoItem(::std::unique_ptr<GenericString> first,
                   ::std::unique_ptr<GenericString> second) :
    m_first(::std::move(first)),
    m_second(::std::move(second))
{
    this->setFlags(Qt::ItemFlag::ItemIsEnabled);
    this->setTextAlignment(0, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
    this->setTextAlignment(1, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
}

/**
 * @brief		Constructor.
 */
InfoItem::InfoItem(::std::unique_ptr<GenericString> first,
                   ::std::function<void(int)>       onDblCkicked) :
    m_first(::std::move(first)),
    m_onDblClick(onDblCkicked)
{
    this->setFlags(Qt::ItemFlag::ItemIsSelectable
                   | Qt::ItemFlag::ItemIsEnabled);
    this->setTextAlignment(0, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
    this->setTextAlignment(1, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
}

/**
 * @brief		Constructor.
 */
InfoItem::InfoItem(::std::unique_ptr<GenericString> first,
                   ::std::unique_ptr<GenericString> second,
                   ::std::function<void(int)>       onDblCkicked) :
    m_first(::std::move(first)),
    m_second(::std::move(second)), m_onDblClick(onDblCkicked)
{
    this->setFlags(Qt::ItemFlag::ItemIsSelectable
                   | Qt::ItemFlag::ItemIsEnabled);
    this->setTextAlignment(0, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
    this->setTextAlignment(1, Qt::AlignmentFlag::AlignLeft
                                  | Qt::AlignmentFlag::AlignTop);
}

/**
 * @brief		Destructor.
 */
InfoItem::~InfoItem() {}

/**
 * @brief		Called when language changed.
 */
void InfoItem::onLanguageChanged()
{
    this->setText(0, m_first->toString());
    if (m_second != nullptr) {
        this->treeWidget()->setItemWidget(
            this, 1,
            new TransparentLabel(m_second->toString(), this->treeWidget()));
        // this->setText(1, m_second->toString());
    }
}

/**
 * @brief		Called when double clicked.
 */
void InfoItem::onDoubleClicked(int column)
{
    if (m_onDblClick) {
        m_onDblClick(column);
    }
}
