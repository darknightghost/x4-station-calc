#include <cmath>

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
ModuleItemWidget::ModuleItemWidget(ModuleItem *item) :
    m_item(item), m_suggestedAmountToChange(0)
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
                  &ModuleItemWidget::onBtnUpClicked);

    // Button Down.
    m_btnDown = new SquareButton(QIcon(":/Icons/Down.png"));
    m_layout->addWidget(m_btnDown, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnDown, &QPushButton::clicked, this,
                  &ModuleItemWidget::onBtnDownClicked);

    // Button Remove.
    m_btnRemove = new SquareButton(QIcon(":/Icons/EditRemove.png"));
    m_layout->addWidget(m_btnRemove, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &ModuleItemWidget::onBtnRemoveClicked);

    // Button Set to Suggested Amount.
    m_btnSetToSuggestedAmount = new QPushButton();
    m_layout->addWidget(m_btnSetToSuggestedAmount,
                        Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnSetToSuggestedAmount, &QPushButton::clicked, this,
                  &ModuleItemWidget::onBtnSetToSuggestedAmountClicked);
    m_btnSetToSuggestedAmount->setEnabled(false);

    // Get module.
    auto module = GameData::instance()->stationModules()->module(
        m_item->module()->module());
    if (module == nullptr) {
        m_btnSetToSuggestedAmount->setVisible(false);
    }

    // Update button status.
    if (module->moduleClass
            != GameStationModules::StationModule::StationModuleClass::Production
        && module->moduleClass
               != GameStationModules::StationModule::StationModuleClass::
                   Habitation) {
        m_btnSetToSuggestedAmount->setVisible(false);
    } else {
        m_btnSetToSuggestedAmount->setVisible(true);
    }

    m_layout->addStretch();
    this->setMaximumHeight(this->fontMetrics().height()
                           + m_layout->margin() * 2);

    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &ModuleItemWidget::onLanguageChanged);
    this->onLanguageChanged();
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
 * @brief       Set suggested amount enable status.
 */
void ModuleItemWidget::setSuggestAmountEnabled(bool enabled)
{
    if (m_btnSetToSuggestedAmount->isVisible()) {
        if (enabled && m_suggestedAmountToChange != 0) {
            m_btnSetToSuggestedAmount->setEnabled(true);

        } else {
            m_btnSetToSuggestedAmount->setEnabled(false);
        }
    }
}

/**
 * @brief       Set suggested amount to change.
 */
void ModuleItemWidget::setSuggestedAmountToChange(qint64 productPerHourToChange)
{
    m_suggestedAmountToChange
        = this->suggestedAmountToChange(productPerHourToChange);
}

/**
 * @brief       On language changed.
 */
void ModuleItemWidget::onLanguageChanged()
{
    m_item->updateName();
    m_btnSetToSuggestedAmount->setText(STR("STR_SET_TO_SUGGESTED_AMOUNT"));
}

/**
 * @brief	On "up" button clicked.
 */
void ModuleItemWidget::onBtnUpClicked()
{
    emit this->btnUpClicked(m_item);
}

/**
 * @brief	On "down" button clicked.
 */
void ModuleItemWidget::onBtnDownClicked()
{
    emit this->btnDownClicked(m_item);
}

/**
 * @brief	On "remove" button clicked.
 */
void ModuleItemWidget::onBtnRemoveClicked()
{
    emit this->btnRemoveClicked(m_item);
}

/**
 * @brief   On "Set to Suggested Amount" button clicked.
 */
void ModuleItemWidget::onBtnSetToSuggestedAmountClicked()
{
    if (m_btnSetToSuggestedAmount->isEnabled()
        && m_btnSetToSuggestedAmount->isVisible()) {
        int amount = static_cast<int>(m_item->moduleAmount())
                     + m_suggestedAmountToChange;

        if (amount > 0) {
            m_spinAmount->setValue(amount);
            this->onSpinboxValueChanged(amount);

        } else {
            this->onBtnRemoveClicked();
        }
    }
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
 * @brief       Compute suggested amount.
 */
int ModuleItemWidget::suggestedAmountToChange(qint64 productPerHourToChange)
{
    if (productPerHourToChange == 0) {
        return 0;
    }

    // Get module.
    auto module = GameData::instance()->stationModules()->module(
        m_item->module()->module());
    if (module == nullptr) {
        return 0;
    }

    switch (module->moduleClass) {
        case GameStationModules::StationModule::StationModuleClass::
            Habitation: {
            // Find property.
            auto iter = module->properties.find(
                GameStationModules::Property::Type::SupplyWorkforce);
            if (iter == module->properties.end()) {
                return 0;
            }

            ::std::shared_ptr<GameStationModules::SupplyWorkforce> property
                = ::std::static_pointer_cast<
                    GameStationModules::SupplyWorkforce>(*iter);

            // Compute result.
            return static_cast<int>(
                ::ceil(static_cast<double>(productPerHourToChange)
                       / property->workforce));

        } break;

        case GameStationModules::StationModule::StationModuleClass::
            Production: {
            // Find property.
            auto iter = module->properties.find(
                GameStationModules::Property::Type::SupplyProduct);
            if (iter == module->properties.end()) {
                return 0;
            }
            ::std::shared_ptr<GameStationModules::SupplyProduct> property
                = ::std::static_pointer_cast<GameStationModules::SupplyProduct>(
                    *iter);

            // Work effect.
            long double workEffect = 0.0;
            auto propertyIter      = property->productionInfo->properties.find(
                GameWares::ProductionInfoProperty::PropertyType::Effect);
            if (propertyIter != property->productionInfo->properties.end()) {
                ::std::shared_ptr<GameWares::Effect> effect
                    = ::std::static_pointer_cast<GameWares::Effect>(
                        propertyIter.value());
                workEffect = effect->product;
            }

            // Compute result.
            double amountPerHour
                = static_cast<double>(property->productionInfo->amount) * 3600
                  * (1 + workEffect) / property->productionInfo->time;

            return static_cast<int>(
                ::ceil(productPerHourToChange / amountPerHour));
        } break;

        default:
            return 0;
    }

    return 0;
}
