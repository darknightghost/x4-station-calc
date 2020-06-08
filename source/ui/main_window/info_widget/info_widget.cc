#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidgetItemIterator>

#include <common/generic_string.h>
#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/info_widget/info_item.h>
#include <ui/main_window/info_widget/info_widget.h>

/**
 * @brief		Constructor.
 */
InfoWidget::InfoWidget(QAction *       statusAction,
                       QWidget *       parent,
                       Qt::WindowFlags flags) :
    ActionControlDockWidget(statusAction, parent, flags),
    m_historyIndex(0)
{
    m_widget = new QWidget(this);
    this->setWidget(m_widget);

    m_layout = new QGridLayout(m_widget);
    m_widget->setLayout(m_layout);

    m_btnBack = new SquareButton(QIcon(":/Icons/Back.png"), m_widget);
    m_layout->addWidget(m_btnBack, 0, 0);
    m_btnBack->setEnabled(false);
    this->connect(m_btnBack, &QPushButton::clicked, this,
                  &InfoWidget::onBtnBackClicked);

    m_btnForward = new SquareButton(QIcon(":/Icons/Forward.png"), m_widget);
    m_layout->addWidget(m_btnForward, 0, 1);
    m_btnForward->setEnabled(false);
    this->connect(m_btnForward, &QPushButton::clicked, this,
                  &InfoWidget::onBtnForwardClicked);

    m_treeInfo = new QTreeWidget(m_widget);
    m_layout->addWidget(m_treeInfo, 1, 0, 1, 3);
    m_treeInfo->header()->setVisible(false);
    m_treeInfo->header()->setSectionResizeMode(
        QHeaderView::ResizeMode::ResizeToContents);
    m_treeInfo->header()->setStretchLastSection(true);
    m_treeInfo->setColumnCount(2);
    m_treeInfo->setSelectionMode(
        QAbstractItemView::SelectionMode::SingleSelection);
    this->connect(m_treeInfo, &QTreeWidget::itemDoubleClicked, this,
                  &InfoWidget::onItemDoubleClicked);

    // Style
    this->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable
                      | QDockWidget::DockWidgetFeature::DockWidgetMovable);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &InfoWidget::onLanguageChanged);

    this->onLanguageChanged();
}

/**
 * @brief		Destructor.
 */
InfoWidget::~InfoWidget() {}

/**
 * @brief		Change language.
 */
void InfoWidget::onLanguageChanged()
{
    // Title
    this->setWindowTitle(STR("STR_INFO_WIDGET_TITLE"));

    // Items
    for (QTreeWidgetItemIterator iter(m_treeInfo); *iter; ++iter) {
        InfoItem *item = static_cast<InfoItem *>(*iter);
        item->onLanguageChanged();
    }
}

/**
 * @brief		Show information of station module.
 */
void InfoWidget::showStationModuleInfo(QString macro, bool keepPrevious)
{
    qDebug() << "Show module" << macro;
    if (! keepPrevious) {
        m_history.clear();
    } else {
        m_history.erase(m_history.begin() + m_historyIndex + 1,
                        m_history.end());
    }

    History history = {HistoryType::StationModule, macro};
    m_history.push_back(history);
    m_historyIndex = m_history.size() - 1;

    this->update();
}

/**
 * @brief		Show information of ware.
 */
void InfoWidget::showWareInfo(QString macro, bool keepPrevious)
{
    if (! keepPrevious) {
        m_history.clear();
    } else {
        m_history.erase(m_history.begin() + m_historyIndex + 1,
                        m_history.end());
    }

    History history = {HistoryType::Ware, macro};
    m_history.push_back(history);
    m_historyIndex = m_history.size() - 1;

    this->update();
}

/**
 * @brief		Show information of race.
 */
void InfoWidget::showRaceInfo(QString macro, bool keepPrevious)
{
    if (! keepPrevious) {
        m_history.clear();
    } else {
        m_history.erase(m_history.begin() + m_historyIndex + 1,
                        m_history.end());
    }

    History history = {HistoryType::Race, macro};
    m_history.push_back(history);
    m_historyIndex = m_history.size() - 1;

    this->update();
}

/**
 * @brief		Update.
 */
void InfoWidget::update()
{
    m_treeInfo->clear();

    // Set button status.
    if (m_history.empty()) {
        m_btnBack->setEnabled(false);
        m_btnForward->setEnabled(false);

        return;
    }
    m_btnBack->setEnabled(m_historyIndex > 0);
    m_btnForward->setEnabled(m_historyIndex < m_history.size() - 1);

    // Show info
    switch (m_history[m_historyIndex].type) {
        case HistoryType::Race:
            this->updateRace(m_history[m_historyIndex].macro);
            break;

        case HistoryType::Ware:
            this->updateWare(m_history[m_historyIndex].macro);
            break;

        case HistoryType::StationModule:
            this->updateModule(m_history[m_historyIndex].macro);
            break;
    }

    this->onLanguageChanged();
    m_treeInfo->expandAll();

    return;
}

/**
 * @brief		Update information of station module.
 */
void InfoWidget::updateModule(const QString &macro)
{
    auto module = GameData::instance()->stationModules()->module(macro);
    auto texts  = GameData::instance()->texts();

    // Basical information
    // Name
    QTreeWidgetItem *nameItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_NAME")),
        ::std::unique_ptr<GenericString>(new GameString(module->name)));
    m_treeInfo->addTopLevelItem(nameItem);

    // Description
    QTreeWidgetItem *desctiptionItem = new InfoItem(
        ::std::unique_ptr<GenericString>(
            new LocaleString("STR_INFO_DESCRIPTION")),
        ::std::unique_ptr<GenericString>(new GameString(module->description)));
    m_treeInfo->addTopLevelItem(desctiptionItem);

    // Type
    QTreeWidgetItem *typeItem = nullptr;
    switch (module->moduleClass) {
        case GameStationModules::StationModule::StationModuleClass::BuildModule:
            typeItem
                = new InfoItem(::std::unique_ptr<GenericString>(
                                   new LocaleString("STR_INFO_STATION_TYPE")),
                               ::std::unique_ptr<GenericString>(
                                   new LocaleString("STATION_TYPE_BUILD")));
            break;

        case GameStationModules::StationModule::StationModuleClass::
            ConnectionModule:
            typeItem
                = new InfoItem(::std::unique_ptr<GenericString>(
                                   new LocaleString("STR_INFO_STATION_TYPE")),
                               ::std::unique_ptr<GenericString>(
                                   new LocaleString("STATION_TYPE_CONNECT")));
            break;

        case GameStationModules::StationModule::StationModuleClass::
            DefenceModule:
            typeItem
                = new InfoItem(::std::unique_ptr<GenericString>(
                                   new LocaleString("STR_INFO_STATION_TYPE")),
                               ::std::unique_ptr<GenericString>(
                                   new LocaleString("STATION_TYPE_DEFENCE")));
            break;

        case GameStationModules::StationModule::StationModuleClass::Dockarea:
            typeItem
                = new InfoItem(::std::unique_ptr<GenericString>(
                                   new LocaleString("STR_INFO_STATION_TYPE")),
                               ::std::unique_ptr<GenericString>(
                                   new LocaleString("STATION_TYPE_DOCK")));
            break;

        case GameStationModules::StationModule::StationModuleClass::Habitation:
            typeItem = new InfoItem(
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_STATION_TYPE")),
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STATION_TYPE_HABITATION")));
            break;

        case GameStationModules::StationModule::StationModuleClass::Production:
            typeItem = new InfoItem(
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_STATION_TYPE")),
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STATION_TYPE_PRODUCTION")));
            break;

        case GameStationModules::StationModule::StationModuleClass::Storage:
            typeItem
                = new InfoItem(::std::unique_ptr<GenericString>(
                                   new LocaleString("STR_INFO_STATION_TYPE")),
                               ::std::unique_ptr<GenericString>(
                                   new LocaleString("STATION_TYPE_STORAGE")));
            break;

        default:
            break;
    }
    if (typeItem != nullptr) {
        m_treeInfo->addTopLevelItem(typeItem);
    }

    // Race
    QTreeWidgetItem *racesItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_RACE")));
    m_treeInfo->addTopLevelItem(racesItem);
    for (auto &race : module->races) {
        QTreeWidgetItem *raceItem
            = new InfoItem(::std::unique_ptr<GenericString>(new GameString(
                               GameData::instance()->races()->race(race).name)),
                           [race, this](int) -> void {
                               this->showRaceInfo(race, true);
                           });
        racesItem->addChild(raceItem);
    }

    // Hull
    QTreeWidgetItem *hullItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_HULL")),
        ::std::unique_ptr<GenericString>(
            new QtString(QString("%1").arg(module->hull))));
    m_treeInfo->addTopLevelItem(hullItem);

    // Explosion Damage
    QTreeWidgetItem *explosionDamageItem
        = new InfoItem(::std::unique_ptr<GenericString>(
                           new LocaleString("STR_INFO_EXPLOSION_DAMAGE")),
                       ::std::unique_ptr<GenericString>(new QtString(
                           QString("%1").arg(module->explosiondamage))));
    m_treeInfo->addTopLevelItem(explosionDamageItem);

    // Weapon/Sield
    // M turret
    auto iter
        = module->properties.find(GameStationModules::Property::Type::MTurret);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasMTurret> property
            = ::std::static_pointer_cast<GameStationModules::HasMTurret>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_M_TURRET")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // M shield
    iter = module->properties.find(GameStationModules::Property::Type::MShield);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasMShield> property
            = ::std::static_pointer_cast<GameStationModules::HasMShield>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_M_SHIELD")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // L turret
    iter = module->properties.find(GameStationModules::Property::Type::LTurret);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasLTurret> property
            = ::std::static_pointer_cast<GameStationModules::HasLTurret>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_L_TURRET")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // L shield
    iter = module->properties.find(GameStationModules::Property::Type::LShield);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasLShield> property
            = ::std::static_pointer_cast<GameStationModules::HasLShield>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_L_SHIELD")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // S launch tube
    iter = module->properties.find(
        GameStationModules::Property::Type::SLaunchTube);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasSLaunchTube> property
            = ::std::static_pointer_cast<GameStationModules::HasSLaunchTube>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_S_LAUNCH_TUBE")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // M launch tube
    iter = module->properties.find(
        GameStationModules::Property::Type::MLaunchTube);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasMLaunchTube> property
            = ::std::static_pointer_cast<GameStationModules::HasMLaunchTube>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_M_LAUNCH_TUBE")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // Harbor
    // S docking bay
    iter = module->properties.find(GameStationModules::Property::Type::SDock);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasSDock> property
            = ::std::static_pointer_cast<GameStationModules::HasSDock>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_S_DOCK")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // S ship cargo
    iter = module->properties.find(
        GameStationModules::Property::Type::SShipCargo);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasSShipCargo> property
            = ::std::static_pointer_cast<GameStationModules::HasSShipCargo>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_S_SHIP_CAPACITY")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->capacity))));
        m_treeInfo->addTopLevelItem(item);
    }

    // M docking bay
    iter = module->properties.find(GameStationModules::Property::Type::MDock);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasMDock> property
            = ::std::static_pointer_cast<GameStationModules::HasMDock>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_M_DOCK")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // M ship cargo
    iter = module->properties.find(
        GameStationModules::Property::Type::MShipCargo);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasMShipCargo> property
            = ::std::static_pointer_cast<GameStationModules::HasMShipCargo>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_M_SHIP_CAPACITY")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->capacity))));
        m_treeInfo->addTopLevelItem(item);
    }

    // L docking bay
    iter = module->properties.find(GameStationModules::Property::Type::LDock);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasLDock> property
            = ::std::static_pointer_cast<GameStationModules::HasLDock>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_L_DOCK")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // XL docking bay
    iter = module->properties.find(GameStationModules::Property::Type::XLDock);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasXLDock> property
            = ::std::static_pointer_cast<GameStationModules::HasXLDock>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_XL_DOCK")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // L/XL docking bay
    iter = module->properties.find(GameStationModules::Property::Type::LXLDock);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasLXLDock> property
            = ::std::static_pointer_cast<GameStationModules::HasLXLDock>(*iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_L_XL_DOCK")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->count))));
        m_treeInfo->addTopLevelItem(item);
    }

    // Habitat
    QTreeWidgetItem *resourcesItem = nullptr;

    // Workforce supply
    iter = module->properties.find(
        GameStationModules::Property::Type::SupplyWorkforce);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::SupplyWorkforce> property
            = ::std::static_pointer_cast<GameStationModules::SupplyWorkforce>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_SUPPLY_WORKFORCE")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->workforce))));

        if (resourcesItem == nullptr) {
            resourcesItem = new InfoItem(::std::unique_ptr<GenericString>(
                new LocaleString("STR_INFO_RESOURCE")));
        }

        for (auto &resource : property->supplyInfo->resources) {
            QString   id           = resource->id;
            InfoItem *resourceItem = new InfoItem(
                ::std::unique_ptr<GenericString>(new GameString(
                    GameData::instance()->wares()->ware(resource->id)->name)),
                ::std::unique_ptr<GenericString>(
                    new QtString(QString("%1/h").arg(
                        ::round((double)(((long double)resource->amount)
                                         * property->workforce * 3600
                                         / property->supplyInfo->amount
                                         / property->supplyInfo->time))))),
                [this, id](int) -> void {
                    this->showWareInfo(id, true);
                });
            resourcesItem->addChild(resourceItem);
        }
        m_treeInfo->addTopLevelItem(item);
    }

    // Product/Build
    // Workforce required
    iter = module->properties.find(
        GameStationModules::Property::Type::RequireWorkforce);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::RequireWorkforce> property
            = ::std::static_pointer_cast<GameStationModules::RequireWorkforce>(
                *iter);
        QTreeWidgetItem *item
            = new InfoItem(::std::unique_ptr<GenericString>(
                               new LocaleString("STR_INFO_REQUIRE_WORKFORCE")),
                           ::std::unique_ptr<GenericString>(new QtString(
                               QString("%1").arg(property->workforce))));
        m_treeInfo->addTopLevelItem(item);
    }

    // Product
    iter = module->properties.find(
        GameStationModules::Property::Type::SupplyProduct);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::SupplyProduct> property
            = ::std::static_pointer_cast<GameStationModules::SupplyProduct>(
                *iter);
        QTreeWidgetItem *productsItem
            = new InfoItem(::std::unique_ptr<GenericString>(
                new LocaleString("STR_INFO_PRODUCT")));
        QString          id          = property->productionInfo->id;
        QTreeWidgetItem *productItem = new InfoItem(
            ::std::unique_ptr<GenericString>(new GameString(
                GameData::instance()->wares()->ware(property->product)->name)),
            ::std::unique_ptr<GenericString>(new QtString(
                QString("%1/h - %2/h")
                    .arg(::round((double)(property->productionInfo->amount)
                                 * 3600 / property->productionInfo->time))
                    .arg(::round(
                        (double)(property->productionInfo->amount) * 3600
                        / property->productionInfo->time
                        * (1.0 + property->productionInfo->workEffect))))),
            [this, id](int) -> void {
                this->showWareInfo(id, true);
            });
        productsItem->addChild(productItem);
        m_treeInfo->addTopLevelItem(productsItem);

        if (resourcesItem == nullptr) {
            resourcesItem = new InfoItem(::std::unique_ptr<GenericString>(
                new LocaleString("STR_INFO_RESOURCE")));
        }

        for (auto &resource : property->productionInfo->resources) {
            QString   id           = resource->id;
            InfoItem *resourceItem = new InfoItem(
                ::std::unique_ptr<GenericString>(new GameString(
                    GameData::instance()->wares()->ware(resource->id)->name)),
                ::std::unique_ptr<GenericString>(new QtString(
                    QString("%1/h - %2/h")
                        .arg(::round((double)(resource->amount) * 3600
                                     / property->productionInfo->time))
                        .arg(::round(
                            (double)(resource->amount) * 3600
                            / property->productionInfo->time
                            * (1.0 + property->productionInfo->workEffect))))),

                [this, id](int) -> void {
                    this->showWareInfo(id, true);
                });
            resourcesItem->addChild(resourceItem);
        }
    }

    // Resource
    if (resourcesItem != nullptr) {
        m_treeInfo->addTopLevelItem(resourcesItem);
    }

    // Cargo
    // Cargo type
    // Cargo size
    iter = module->properties.find(GameStationModules::Property::Type::Cargo);
    if (iter != module->properties.end()) {
        ::std::shared_ptr<GameStationModules::HasCargo> property
            = ::std::static_pointer_cast<GameStationModules::HasCargo>(*iter);
        QTreeWidgetItem *cargoTypeItem = nullptr;

        switch (property->cargoType) {
            case GameWares::TransportType::Container:
                cargoTypeItem = new InfoItem(
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE")),
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE_CONTAINER")));
                break;

            case GameWares::TransportType::Solid:
                cargoTypeItem = new InfoItem(
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE")),
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE_SOLID")));
                break;

            case GameWares::TransportType::Liquid:
                cargoTypeItem = new InfoItem(
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE")),
                    ::std::unique_ptr<GenericString>(
                        new LocaleString("STR_INFO_CARGO_TYPE_LIQUID")));
                break;

            default:
                break;
        }

        if (cargoTypeItem != nullptr) {
            m_treeInfo->addTopLevelItem(cargoTypeItem);
        }

        QTreeWidgetItem *cargoSize = new InfoItem(
            ::std::unique_ptr<GenericString>(
                new LocaleString("STR_INFO_CARGO_SIZE")),
            ::std::unique_ptr<GenericString>(new QtString(
                QString("%1 m\xc2\xb3").arg(property->cargoSize))));
        m_treeInfo->addTopLevelItem(cargoSize);
    }
}

/**
 * @brief		Update information of ware.
 */
void InfoWidget::updateWare(const QString &macro)
{
    auto ware  = GameData::instance()->wares()->ware(macro);
    auto texts = GameData::instance()->texts();

    // Name
    QTreeWidgetItem *nameItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_NAME")),
        ::std::unique_ptr<GenericString>(new GameString(ware->name)));
    m_treeInfo->addTopLevelItem(nameItem);

    // Description
    QTreeWidgetItem *desctiptionItem = new InfoItem(
        ::std::unique_ptr<GenericString>(
            new LocaleString("STR_INFO_DESCRIPTION")),
        ::std::unique_ptr<GenericString>(new GameString(ware->description)));
    m_treeInfo->addTopLevelItem(desctiptionItem);

    // Transport type
    QTreeWidgetItem *transportTypeItem = nullptr;

    switch (ware->transportType) {
        case GameWares::TransportType::Container:
            transportTypeItem = new InfoItem(
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_TRANSPORT_TYPE")),
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_CARGO_TYPE_CONTAINER")));
            break;

        case GameWares::TransportType::Solid:
            transportTypeItem = new InfoItem(
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_TRANSPORT_TYPE")),
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_CARGO_TYPE_SOLID")));
            break;

        case GameWares::TransportType::Liquid:
            transportTypeItem = new InfoItem(
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_TRANSPORT_TYPE")),
                ::std::unique_ptr<GenericString>(
                    new LocaleString("STR_INFO_CARGO_TYPE_LIQUID")));
            break;

        default:
            break;
    }

    if (transportTypeItem != nullptr) {
        m_treeInfo->addTopLevelItem(transportTypeItem);
    }

    // Volume
    QTreeWidgetItem *volumeItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_VOLUME")),
        ::std::unique_ptr<GenericString>(
            new QtString(QString("%1 m\xc2\xb3").arg(ware->volume))));
    m_treeInfo->addTopLevelItem(volumeItem);

    // Minimum price
    QTreeWidgetItem *minPriceItem
        = new InfoItem(::std::unique_ptr<GenericString>(
                           new LocaleString("STR_INFO_MIN_PRICE")),
                       ::std::unique_ptr<GenericString>(
                           new QtString(QString("%1 CR").arg(ware->minPrice))));
    m_treeInfo->addTopLevelItem(minPriceItem);

    // Average price
    QTreeWidgetItem *averagePriceItem
        = new InfoItem(::std::unique_ptr<GenericString>(
                           new LocaleString("STR_INFO_AVERAGE_PRICE")),
                       ::std::unique_ptr<GenericString>(new QtString(
                           QString("%1 CR").arg(ware->averagePrice))));
    m_treeInfo->addTopLevelItem(averagePriceItem);

    // Maximum price
    QTreeWidgetItem *maxPriceItem
        = new InfoItem(::std::unique_ptr<GenericString>(
                           new LocaleString("STR_INFO_MAX_PRICE")),
                       ::std::unique_ptr<GenericString>(
                           new QtString(QString("%1 CR").arg(ware->maxPrice))));
    m_treeInfo->addTopLevelItem(maxPriceItem);
}

/**
 * @brief		Update information of race.
 */
void InfoWidget::updateRace(const QString &macro)
{
    auto &race  = GameData::instance()->races()->race(macro);
    auto  texts = GameData::instance()->texts();

    // Name
    QTreeWidgetItem *nameItem = new InfoItem(
        ::std::unique_ptr<GenericString>(new LocaleString("STR_INFO_NAME")),
        ::std::unique_ptr<GenericString>(new GameString(race.name)));
    m_treeInfo->addTopLevelItem(nameItem);

    // Description
    QTreeWidgetItem *desctiptionItem = new InfoItem(
        ::std::unique_ptr<GenericString>(
            new LocaleString("STR_INFO_DESCRIPTION")),
        ::std::unique_ptr<GenericString>(new GameString(race.description)));
    m_treeInfo->addTopLevelItem(desctiptionItem);

    return;
}

/**
 * @brief		On item double clicked.
 */
void InfoWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    ((InfoItem *)item)->onDoubleClicked(column);
}

/**
 * @brief	On button back clicked.
 */
void InfoWidget::onBtnBackClicked()
{
    qDebug() << "Back";
    if (m_historyIndex > 0) {
        --m_historyIndex;
        this->update();
    }
}

/**
 * @brief	On button forward clicked.
 */
void InfoWidget::onBtnForwardClicked()
{
    qDebug() << "Forward";
    if (m_historyIndex < m_history.size() - 1) {
        ++m_historyIndex;
        this->update();
    }
}
