#include <QtCore/QCollator>
#include <QtCore/QDebug>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>

#include <common/compare.h>
#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/customized_widgets/customized_message_box.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief		Constructor.
 */
StationModulesWidget::StationModulesWidget(QAction *       statusAction,
                                           QWidget *       parent,
                                           Qt::WindowFlags flags) :
    ActionControlDockWidget(statusAction, parent, flags)
{
    // Style
    // this->setWindowFlags(Qt::WindowType::Tool);
    this->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable
                      | QDockWidget::DockWidgetFeature::DockWidgetMovable);
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);

    // Load widgets
    m_widget = new QWidget(this);

    m_layout = new QVBoxLayout(m_widget);
    m_widget->setLayout(m_layout);

    // Button bar.
    m_widgetButtonBar = new QWidget(m_widget);
    m_layout->addWidget(m_widgetButtonBar);

    m_layoutButtonBar = new QHBoxLayout(m_widgetButtonBar);
    m_widgetButtonBar->setLayout(m_layoutButtonBar);

    m_btnShowHideFilter = new QPushButton(m_widgetButtonBar);
    m_layoutButtonBar->addWidget(m_btnShowHideFilter);
    this->connect(m_btnShowHideFilter, &QPushButton::clicked, this,
                  &StationModulesWidget::onBtnShowHideFilterWidgetClicked);

    m_layoutButtonBar->addStretch();

    m_btnAddToStation = new QPushButton(m_widgetButtonBar);
    m_layoutButtonBar->addWidget(m_btnAddToStation);
    m_btnAddToStation->setEnabled(false);

    // Filters.
    m_widgetFilters = new QWidget(m_widget);
    m_layout->addWidget(m_widgetFilters);
    m_widgetFilters->setVisible(false);

    m_layoutFilters = new QGridLayout(m_widgetFilters);

    m_chkByRace = new QCheckBox(m_widgetFilters);
    m_chkByRace->setChecked(false);
    m_chkByRace->setTristate(false);
    m_layoutFilters->addWidget(m_chkByRace, 0, 0);
    this->connect(m_chkByRace, &QCheckBox::stateChanged, this,
                  &StationModulesWidget::onByRaceChkChanged);
    m_comboByRaces = new QComboBox(m_widgetFilters);
    m_comboByRaces->setEnabled(false);
    m_layoutFilters->addWidget(m_comboByRaces, 0, 1);

    m_chkByProduction = new QCheckBox(m_widgetFilters);
    m_chkByProduction->setChecked(false);
    m_chkByProduction->setTristate(false);
    m_layoutFilters->addWidget(m_chkByProduction, 1, 0);
    this->connect(m_chkByProduction, &QCheckBox::stateChanged, this,
                  &StationModulesWidget::onByProductionChkChanged);
    m_comboByProduction = new QComboBox(m_widgetFilters);
    m_comboByProduction->setEnabled(false);
    m_layoutFilters->addWidget(m_comboByProduction, 1, 1);

    m_chkByResource = new QCheckBox(m_widgetFilters);
    m_chkByResource->setChecked(false);
    m_chkByResource->setTristate(false);
    m_layoutFilters->addWidget(m_chkByResource, 2, 0);
    this->connect(m_chkByResource, &QCheckBox::stateChanged, this,
                  &StationModulesWidget::onByResourceChkChanged);
    m_comboByResource = new QComboBox(m_widgetFilters);
    m_comboByResource->setEnabled(false);
    m_layoutFilters->addWidget(m_comboByResource, 2, 1);

    m_chkByKeyword = new QCheckBox(m_widgetFilters);
    m_chkByKeyword->setChecked(false);
    m_chkByKeyword->setTristate(false);
    m_layoutFilters->addWidget(m_chkByKeyword, 3, 0);
    this->connect(m_chkByKeyword, &QCheckBox::stateChanged, this,
                  &StationModulesWidget::onByKeywordChkChanged);
    m_txtKeyword = new QLineEdit(m_widgetFilters);
    m_txtKeyword->setEnabled(false);
    m_layoutFilters->addWidget(m_txtKeyword, 3, 1);

    // Station modules.
    m_treeStationModules = new QTreeWidget(m_widget);
    m_layout->addWidget(m_treeStationModules);
    m_treeStationModules->header()->setVisible(false);
    m_treeStationModules->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);

    m_itemBuild = new QTreeWidgetItem(m_treeStationModules);
    m_itemBuild->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemBuild->setExpanded(false);

    m_itemDock = new QTreeWidgetItem(m_treeStationModules);
    m_itemDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemDock->setExpanded(false);

    m_itemProduction = new QTreeWidgetItem(m_treeStationModules);
    m_itemProduction->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemProduction->setExpanded(false);

    m_itemStorage = new QTreeWidgetItem(m_treeStationModules);
    m_itemStorage->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemStorage->setExpanded(false);

    m_itemHabitation = new QTreeWidgetItem(m_treeStationModules);
    m_itemHabitation->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemHabitation->setExpanded(false);

    m_itemDefence = new QTreeWidgetItem(m_treeStationModules);
    m_itemDefence->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemDefence->setExpanded(false);

    m_itemConnect = new QTreeWidgetItem(m_treeStationModules);
    m_itemConnect->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemConnect->setExpanded(false);

    m_treeStationModules->insertTopLevelItems(
        0, {m_itemBuild, m_itemDock, m_itemProduction, m_itemStorage,
            m_itemHabitation, m_itemDefence, m_itemConnect});

    this->setWidget(m_widget);

    // Load modules
    this->loadStationModules();
    for (auto &resource : m_resources) {
        m_comboByResource->insertItem(-1, "", resource);
    }

    for (auto &product : m_products) {
        m_comboByProduction->insertItem(-1, "", product);
    }
    for (auto &race : m_races) {
        m_comboByRaces->insertItem(-1, "", race);
    }

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &StationModulesWidget::onLanguageChanged);

    this->onLanguageChanged();
    m_comboByResource->setCurrentIndex(0);
    m_comboByProduction->setCurrentIndex(0);
    m_comboByRaces->setCurrentIndex(0);

    this->connect(m_comboByRaces,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [=](int) {
                      this->filterModules();
                  });

    this->connect(m_comboByResource,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [=](int) {
                      this->filterModules();
                  });

    this->connect(m_comboByProduction,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [=](int) {
                      this->filterModules();
                  });

    this->connect(m_txtKeyword, &QLineEdit::textChanged, this,
                  &StationModulesWidget::filterModules);

    this->connect(m_btnAddToStation, &QPushButton::clicked, this,
                  &StationModulesWidget::onAddToStationClicked);

    this->connect(m_treeStationModules, &QTreeWidget::itemDoubleClicked, this,
                  &StationModulesWidget::onItemClicked);
}

/**
 * @brief		Set enable status of add to station button.
 */
void StationModulesWidget::setAddToStationStatus(bool enabled)
{
    m_btnAddToStation->setEnabled(enabled);
}

/**
 * @brief		Filter modules by product.
 */
void StationModulesWidget::onFilterByProduct(QString ware)
{
    for (auto i = 0; i < m_comboByProduction->count(); ++i) {
        QString macro = m_comboByProduction->itemData(i).toString();
        if (macro == ware) {
            m_comboByProduction->setCurrentIndex(i);
            if (! m_widgetFilters->isVisible()) {
                this->onBtnShowHideFilterWidgetClicked();
            }

            m_chkByProduction->setChecked(true);

            return;
        }
    }

    CustomizedMessageBox::warning(this, STR("STR_WARNING"),
                                  STR("STR_PRODUCT_NOT_FOUND"));
}

/**
 * @brief		Filter modules by resource.
 */
void StationModulesWidget::onFilterByResource(QString ware)
{
    for (auto i = 0; i < m_comboByResource->count(); ++i) {
        QString macro = m_comboByResource->itemData(i).toString();
        if (macro == ware) {
            m_comboByResource->setCurrentIndex(i);

            if (! m_widgetFilters->isVisible()) {
                this->onBtnShowHideFilterWidgetClicked();
            }

            m_chkByResource->setChecked(true);
            return;
        }
    }

    CustomizedMessageBox::warning(this, STR("STR_WARNING"),
                                  STR("STR_RESOURCE_NOT_FOUND"));
}

/**
 * @brief		Destructor.
 */
StationModulesWidget::~StationModulesWidget() {}

/**
 * @brief	Load all station modules.
 */
void StationModulesWidget::loadStationModules()
{
    for (::std::shared_ptr<GameStationModules::StationModule> module :
         GameData::instance()->stationModules()->modules()) {
        for (auto &race : module->races) {
            m_races.insert(race);
        }
        switch (module->moduleClass) {
            case GameStationModules::StationModule::StationModuleClass::
                BuildModule: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemBuild, module);
                m_itemBuild->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                ConnectionModule: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemConnect, module);
                m_itemConnect->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                DefenceModule: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemDefence, module);
                m_itemDefence->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                Dockarea: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemDock, module);
                m_itemDock->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                Habitation: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemHabitation,
                                                       module);
                m_itemHabitation->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                Production: {
                auto iter = module->properties.find(
                    GameStationModules::Property::Type::SupplyProduct);
                if (iter == module->properties.end()) {
                    break;
                }

                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemProduction,
                                                       module);
                m_itemProduction->insertChild(-1, item);
                m_moduleItems.push_back(item);

                ::std::shared_ptr<GameStationModules::SupplyProduct> property
                    = ::std::static_pointer_cast<
                        GameStationModules::SupplyProduct>(*iter);
                m_products.insert(property->product);
                for (auto resource : property->productionInfo->resources) {
                    m_resources.insert(resource->id);
                }

            } break;

            case GameStationModules::StationModule::StationModuleClass::
                Storage: {
                StationModulesTreeWidgetItem *item
                    = new StationModulesTreeWidgetItem(m_itemStorage, module);
                m_itemStorage->insertChild(-1, item);
                m_moduleItems.push_back(item);

            } break;

            default:
                break;
        }
    }
}

/**
 * @brief		Change language.
 */
void StationModulesWidget::onLanguageChanged()
{
    auto gameTexts = GameData::instance()->texts();

    // Title
    this->setWindowTitle(STR("STR_STATION_MODULE_WIDGET_TITLE"));

    // Button bar.
    if (m_widgetFilters->isVisible()) {
        m_btnShowHideFilter->setText(STR("STR_HIDE_FILTER"));

    } else {
        m_btnShowHideFilter->setText(STR("STR_SHOW_FILTER"));
    }
    m_btnAddToStation->setText(STR("STR_ADD_TO_STATION"));

    // Filters.
    m_chkByRace->setText(STR("STR_BY_RACE"));
    for (int i = 0; i < m_comboByRaces->count(); ++i) {
        QString id = m_comboByRaces->itemData(i).toString();
        m_comboByRaces->setItemText(
            i, gameTexts->text(GameData::instance()->races()->race(id).name));
    }
    this->sortComboBox(m_comboByRaces);
    m_chkByProduction->setText(STR("STR_BY_PRODUCT"));
    for (int i = 0; i < m_comboByProduction->count(); ++i) {
        QString id = m_comboByProduction->itemData(i).toString();
        m_comboByProduction->setItemText(
            i, gameTexts->text(GameData::instance()->wares()->ware(id)->name));
    }
    this->sortComboBox(m_comboByProduction);

    m_chkByResource->setText(STR("STR_BY_RESOURCE"));
    for (int i = 0; i < m_comboByResource->count(); ++i) {
        QString id = m_comboByResource->itemData(i).toString();
        m_comboByResource->setItemText(
            i, gameTexts->text(GameData::instance()->wares()->ware(id)->name));
    }
    this->sortComboBox(m_comboByResource);

    m_chkByKeyword->setText(STR("STR_BY_KEYWORD"));

    // Station modules.
    m_itemBuild->setText(0, STR("STATION_TYPE_BUILD"));
    m_itemDock->setText(0, STR("STATION_TYPE_DOCK"));
    m_itemProduction->setText(0, STR("STATION_TYPE_PRODUCTION"));
    m_itemStorage->setText(0, STR("STATION_TYPE_STORAGE"));
    m_itemHabitation->setText(0, STR("STATION_TYPE_HABITATION"));
    m_itemDefence->setText(0, STR("STATION_TYPE_DEFENCE"));
    m_itemConnect->setText(0, STR("STATION_TYPE_CONNECT"));

    // Items
    for (auto module : m_moduleItems) {
        module->onLanguageChanged();
    }

    m_itemBuild->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemDock->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemProduction->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemStorage->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemHabitation->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemDefence->sortChildren(0, Qt::SortOrder::AscendingOrder);
    m_itemConnect->sortChildren(0, Qt::SortOrder::AscendingOrder);
}

/**
 * @brief	Sort combobox.
 */
void StationModulesWidget::sortComboBox(QComboBox *combo)
{
    QCollator collator;
    int       index = combo->currentIndex();
    for (int i = 0; i < combo->count(); ++i) {
        for (int j = i + 1; j < combo->count(); ++j) {
            if (collator.compare(combo->itemText(i), combo->itemText(j)) > 0) {
                // Swap
                QVariant v = combo->itemData(i);
                QString  s = combo->itemText(i);
                combo->setItemData(i, combo->itemData(j));
                combo->setItemText(i, combo->itemText(j));
                combo->setItemData(j, v);
                combo->setItemText(j, s);
                if (index == i) {
                    index = j;

                } else if (index == j) {
                    index = i;
                }
            }
        }
    }

    combo->setCurrentIndex(index);
}

/**
 * @brief	Filter station modules.
 */
void StationModulesWidget::filterModules()
{
    for (auto &moduleItem : m_moduleItems) {
        if (m_chkByRace->isChecked()) {
            if ((! moduleItem->module()->races.empty())
                && moduleItem->module()->races.find(
                       m_comboByRaces->currentData().toString())
                       == moduleItem->module()->races.end()) {
                moduleItem->setHidden(true);
                continue;
            }
        }

        if (m_chkByResource->isChecked()) {
            if (moduleItem->module()->moduleClass
                == GameStationModules::StationModule::StationModuleClass::
                    Production) {
                ::std::shared_ptr<GameStationModules::StationModule> module
                    = moduleItem->module();
                ::std::shared_ptr<GameStationModules::SupplyProduct> property
                    = ::std::static_pointer_cast<
                        GameStationModules::SupplyProduct>(
                        module->properties[GameStationModules::Property::Type::
                                               SupplyProduct]);

                if (property->productionInfo->resources.find(
                        m_comboByResource->currentData().toString())
                    == property->productionInfo->resources.end()) {
                    moduleItem->setHidden(true);
                    continue;
                }
            }
        }

        if (m_chkByProduction->isChecked()) {
            if (moduleItem->module()->moduleClass
                == GameStationModules::StationModule::StationModuleClass::
                    Production) {
                ::std::shared_ptr<GameStationModules::StationModule> module
                    = moduleItem->module();
                ::std::shared_ptr<GameStationModules::SupplyProduct> property
                    = ::std::static_pointer_cast<
                        GameStationModules::SupplyProduct>(
                        module->properties[GameStationModules::Property::Type::
                                               SupplyProduct]);
                if (property->product
                    != m_comboByProduction->currentData().toString()) {
                    moduleItem->setHidden(true);
                    continue;
                }
            }
        }

        if (m_chkByKeyword->isChecked()) {
            if (! moduleItem->text(0).contains(m_txtKeyword->text())) {
                moduleItem->setHidden(true);
                continue;
            }
        }

        moduleItem->setHidden(false);
    }
}

/**
 * @brief		\c m_btnShowHideFilter is clicked.
 */
void StationModulesWidget::onBtnShowHideFilterWidgetClicked()
{
    // Button bar.
    if (m_widgetFilters->isVisible()) {
        m_widgetFilters->setVisible(false);
        m_btnShowHideFilter->setText(STR("STR_SHOW_FILTER"));

    } else {
        m_widgetFilters->setVisible(true);
        m_btnShowHideFilter->setText(STR("STR_HIDE_FILTER"));
    }
}

/**
 * @brief		\c m_chkByRace status changed.
 */
void StationModulesWidget::onByRaceChkChanged(int status)
{
    if (status == Qt::CheckState::Unchecked) {
        m_comboByRaces->setEnabled(false);

    } else {
        m_comboByRaces->setEnabled(true);
    }
    this->filterModules();
}

/**
 * @brief		\c m_chkByProduction status changed.
 */
void StationModulesWidget::onByProductionChkChanged(int status)
{
    if (status == Qt::CheckState::Unchecked) {
        m_comboByProduction->setEnabled(false);

    } else {
        m_comboByProduction->setEnabled(true);
    }
    this->filterModules();
}

/**
 * @brief		\c m_chkByResource status changed.
 */
void StationModulesWidget::onByResourceChkChanged(int status)
{
    if (status == Qt::CheckState::Unchecked) {
        m_comboByResource->setEnabled(false);

    } else {
        m_comboByResource->setEnabled(true);
    }
    this->filterModules();
}

/**
 * @brief		\c m_chkByKeyword status changed.
 */
void StationModulesWidget::onByKeywordChkChanged(int status)
{
    if (status == Qt::CheckState::Unchecked) {
        m_txtKeyword->setEnabled(false);

    } else {
        m_txtKeyword->setEnabled(true);
    }
    this->filterModules();
}

/**
 * @brief		On button add to station clicked.
 */
void StationModulesWidget::onAddToStationClicked()
{
    QStringList macros;
    for (auto item : m_treeStationModules->selectedItems()) {
        if (notIn(item, m_itemBuild, m_itemDock, m_itemProduction,
                  m_itemStorage, m_itemHabitation, m_itemDefence,
                  m_itemConnect)) {
            StationModulesTreeWidgetItem *moduleItem
                = (StationModulesTreeWidgetItem *)item;
            macros.push_back((moduleItem->module()->macro));
        }
    }
    if (! macros.empty()) {
        emit this->addToStation(::std::move(macros));
    }
}

/**
 * @brief		On item clicked.
 */
void StationModulesWidget::onItemClicked(QTreeWidgetItem *item)
{
    if (in(item, m_itemBuild, m_itemDock, m_itemProduction, m_itemStorage,
           m_itemHabitation, m_itemDefence, m_itemConnect)) {
        return;
    } else {
        StationModulesTreeWidgetItem *moduleItem
            = (StationModulesTreeWidgetItem *)item;
        qDebug() << moduleItem->module()->macro << "clicked.";
        emit this->stationModuleClicked(moduleItem->module()->macro, false);
    }
}
