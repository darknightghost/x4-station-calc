#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <skin_manager.h>
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
    m_btnUp = new SquareButton();
    m_layout->addWidget(m_btnUp, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnUp, &QPushButton::clicked, this,
                  &ModuleItemWidget::onUpBtnClicked);

    // Button Down.
    m_btnDown = new SquareButton();
    m_layout->addWidget(m_btnDown, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnDown, &QPushButton::clicked, this,
                  &ModuleItemWidget::onDownBtnClicked);

    // Button Remove.
    m_btnRemove = new SquareButton();
    m_layout->addWidget(m_btnRemove, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &ModuleItemWidget::onRemoveBtnClicked);

    m_layout->addStretch();
    this->setMaximumHeight(this->fontMetrics().height()
                           + m_layout->margin() * 2);

    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &ModuleItemWidget::onLanguageChanged);
    this->connect(SkinManager::instance().get(), &SkinManager::skinChanged,
                  this, &ModuleItemWidget::onSkinChanged);

    this->onSkinChanged();
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
    emit this->upBtnClicked(m_item);
}

/**
 * @brief	On "down" button clicked.
 */
void ModuleItemWidget::onDownBtnClicked()
{
    emit this->downBtnClicked(m_item);
}

/**
 * @brief	On "remove" button clicked.
 */
void ModuleItemWidget::onRemoveBtnClicked()
{
    emit this->removeBtnClicked(m_item);
}

/**
 * @brief	On value of spinbox changed.
 */
void ModuleItemWidget::onSpinboxValueChanged(int i)
{
    int oldCount = m_item->moduleAmount();

    if (oldCount != i) {
        emit this->changeAmount(oldCount, i, m_item);
    }
}

/**
 * @brief		Change skin.
 */
void ModuleItemWidget::onSkinChanged()
{
    // Button Up.
    m_btnUp->setIcon(QIcon(QString(":/Skins/%1/Icons/Up.png")
                               .arg(SkinManager::instance()->currentSkin())));

    // Button Down.
    m_btnDown->setIcon(QIcon(QString(":/Skins/%1/Icons/Down.png")
                                 .arg(SkinManager::instance()->currentSkin())));

    // Button Remove.
    m_btnRemove->setIcon(
        QIcon(QString("();:/Skins/%1/Icons/EditRemove.png")
                  .arg(SkinManager::instance()->currentSkin())));
}
