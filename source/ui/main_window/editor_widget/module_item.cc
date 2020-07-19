#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/editor_widget/module_item.h>

/**
 * @brief		Constructor.
 */
ModuleItem::ModuleItem(::std::shared_ptr<SaveModule> module) : m_module(module)
{
    // Style.
    this->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable);

    // Name.
    this->updateName();
}

/**
 * @brief		Get module.
 */
::std::shared_ptr<SaveModule> ModuleItem::module()
{
    return m_module;
}

/**
 * @brief		Get module amount.
 */
quint64 ModuleItem::moduleAmount()
{
    return m_module->amount();
}

/**
 * @brief		Set module amount.
 */
void ModuleItem::setModuleAmount(quint64 amount)
{
    m_module->setAmount(amount);
}

/**
 * @brief		Update module name.
 */
void ModuleItem::updateName()
{
    ::std::shared_ptr<GameData> gameData = GameData::instance();
    this->setText(
        0, gameData->texts()->text(
               gameData->stationModules()->module(m_module->module())->name));
}

/**
 * @brief		Destructor.
 */
ModuleItem::~ModuleItem() {}

/**
 * @brief		Constructor.
 */
ModuleItemWidget::ModuleItemWidget(ModuleItem *item) : m_item(item)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    // Layout.
    m_layout = new QHBoxLayout(this);
    this->setLayout(m_layout);

    // Spinbox.
    m_spinAmount = new AmountSpinBox(this);
    m_layout->QLayout::addWidget(m_spinAmount);
    m_spinAmount->setValue((int)m_item->moduleAmount());
    this->connect(m_spinAmount, &AmountSpinBox::amountEdited, this,
                  &ModuleItemWidget::onSpinboxValueChanged);

    // Button Up.
    m_btnUp = new SquareButton(QIcon(":/Icons/Up.png"));
    m_layout->addWidget(m_btnUp, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnUp, &QPushButton::clicked, this,
                  &ModuleItemWidget::onUpBtnClicked);

    // Button Down.
    m_btnDown = new SquareButton(QIcon(":/Icons/Down.png"));
    m_layout->addWidget(m_btnDown, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnDown, &QPushButton::clicked, this,
                  &ModuleItemWidget::onDownBtnClicked);

    // Button Remove.
    m_btnRemove = new SquareButton(QIcon(":/Icons/EditRemove.png"));
    m_layout->addWidget(m_btnRemove, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &ModuleItemWidget::onRemoveBtnClicked);

    m_layout->addStretch();
    this->setMaximumHeight(this->fontMetrics().height()
                           + m_layout->margin() * 2);

    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &ModuleItemWidget::onLanguageChanged);
}

/**
 * @brief		Destructor.
 */
ModuleItemWidget::~ModuleItemWidget() {}

/**
 * @brief		Set enable status of "up" button.
 */
void ModuleItemWidget::setUpBtnEnabled(bool enabled)
{
    m_btnUp->setEnabled(enabled);
}

/**
 * @brief		Set enable status of "down" button.
 */
void ModuleItemWidget::setDownBtnEnabled(bool enabled)
{
    m_btnDown->setEnabled(enabled);
}

/**
 * @brief       Update amount.
 */
void ModuleItemWidget::updateAmount()
{
    m_spinAmount->setValue((int)m_item->moduleAmount());
}

/**
 * @brief       On language changed.
 */
void ModuleItemWidget::onLanguageChanged()
{
    m_item->updateName();
}

/**
 * @brief	On "up" button clicked.
 */
void ModuleItemWidget::onUpBtnClicked()
{
    emit this->upBtnClicked();
}

/**
 * @brief	On "down" button clicked.
 */
void ModuleItemWidget::onDownBtnClicked()
{
    emit this->downBtnClicked();
}

/**
 * @brief	On "remove" button clicked.
 */
void ModuleItemWidget::onRemoveBtnClicked()
{
    emit this->removeBtnClicked();
}

/**
 * @brief	On value of spinbox changed.
 */
void ModuleItemWidget::onSpinboxValueChanged(int i)
{
    int oldCount = m_item->moduleAmount();

    if (oldCount != i) {
        emit this->changeAmount(oldCount, i);
    }
}
