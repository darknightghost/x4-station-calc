#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtGui/QCloseEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <locale/string_table.h>
#include <ui/main_window/editor_widget/editor_widget.h>
#include <ui/main_window/editor_widget/x4sc_group_clipboard_mime_data_builder.h>
#include <ui/main_window/editor_widget/x4sc_module_clipboard_mime_data_builder.h>

QMap<QString, EditorWidget *> EditorWidget::_opendFiles; ///< Opened files.

/**
 * @brief       Get editor widget by path.
 */
EditorWidget *EditorWidget::getEditorWidgetByPath(const QString &path)
{
    auto iter = _opendFiles.find(QDir(".").absoluteFilePath(path));
    if (iter == _opendFiles.end()) {
        return nullptr;
    } else {
        return *iter;
    }
}

/**
 * @brief		Constructor.
 */
EditorWidget::EditorWidget(::std::shared_ptr<Save>  save,
                           MainWindow::FileActions *fileActions,
                           MainWindow::EditActions *editActions,
                           InfoWidget *             infoWidget,
                           StationModulesWidget *   stationModulesWidget,
                           QMdiSubWindow *          parent) :
    QWidget(parent),
    m_stationModulesWidget(stationModulesWidget), m_infoWidget(infoWidget),
    m_save(save), m_savedUndoCount(0), m_fileActions(fileActions),
    m_editActions(editActions), m_backgroundTasks(new BackgroundTask(
                                    BackgroundTask::RunType::Newest, this)),
    m_treeEditor(nullptr)
{
    this->connect(this, &EditorWidget::windowTitleChanged, parent,
                  &QMdiSubWindow::setWindowTitle);
    this->updateTitle();

    // UI
    // Layout
    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);

    // Editor.
    m_treeEditor = new QTreeWidget(this);
    m_treeEditor->header()->setVisible(false);
    m_treeEditor->header()->setSectionResizeMode(
        QHeaderView::ResizeMode::ResizeToContents);
    m_treeEditor->header()->setStretchLastSection(true);
    m_treeEditor->setColumnCount(3);
    m_treeEditor->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);
    m_layout->addWidget(m_treeEditor);
    m_treeEditor->setContextMenuPolicy(
        Qt::ContextMenuPolicy::CustomContextMenu);
    this->connect(m_treeEditor, &QTreeWidget::itemChanged, this,
                  &EditorWidget::onItemChanged);
    this->connect(m_treeEditor, &QTreeWidget::itemDoubleClicked, this,
                  &EditorWidget::onItemDoubleClicked);
    this->connect(m_treeEditor, &QTreeWidget::itemSelectionChanged, this,
                  &EditorWidget::updateCutCopyRemoveStatus);
    this->connect(m_treeEditor, &QTreeWidget::itemSelectionChanged, this,
                  &EditorWidget::updatePasteStatus);
    this->connect(m_treeEditor, &QTreeWidget::itemSelectionChanged, this,
                  &EditorWidget::updateAddToStationStatus);
    this->connect(m_treeEditor, &QWidget::customContextMenuRequested, this,
                  &EditorWidget::onCustomContextMenuRequested);

    // Items.
    // Groups.
    m_itemGroups = new QTreeWidgetItem();
    m_itemGroups->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemGroups);
    this->loadGroups();

    // Summary.
    m_itemSummary = new QTreeWidgetItem();
    m_itemSummary->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_treeEditor->addTopLevelItem(m_itemSummary);

    m_itemHull = new QTreeWidgetItem();
    m_itemHull->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemHull);

    m_itemExplosionDamage = new QTreeWidgetItem();
    m_itemExplosionDamage->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemExplosionDamage);

    m_itemWeapons = new QTreeWidgetItem();
    m_itemWeapons->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemWeapons);
    m_itemWeapons->setExpanded(true);
    {
        m_itemWeaponSLaunchTube = new QTreeWidgetItem();
        m_itemWeaponSLaunchTube->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemWeapons->addChild(m_itemWeaponSLaunchTube);

        m_itemWeaponMLaunchTube = new QTreeWidgetItem();
        m_itemWeaponMLaunchTube->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemWeapons->addChild(m_itemWeaponMLaunchTube);

        m_itemWeaponMTurret = new QTreeWidgetItem();
        m_itemWeaponMTurret->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemWeapons->addChild(m_itemWeaponMTurret);

        m_itemWeaponLTurret = new QTreeWidgetItem();
        m_itemWeaponLTurret->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemWeapons->addChild(m_itemWeaponLTurret);
    }

    m_itemShields = new QTreeWidgetItem();
    m_itemShields->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemShields);
    m_itemShields->setExpanded(true);
    {
        m_itemShieldMShield = new QTreeWidgetItem();
        m_itemShieldMShield->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemShields->addChild(m_itemShieldMShield);

        m_itemShieldLShield = new QTreeWidgetItem();
        m_itemShieldLShield->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemShields->addChild(m_itemShieldLShield);
    }

    m_itemStorage = new QTreeWidgetItem();
    m_itemStorage->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemStorage);
    m_itemStorage->setExpanded(true);
    {
        m_itemStorageContainer = new QTreeWidgetItem();
        m_itemStorageContainer->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemStorage->addChild(m_itemStorageContainer);

        m_itemStorageSolid = new QTreeWidgetItem();
        m_itemStorageSolid->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemStorage->addChild(m_itemStorageSolid);

        m_itemStorageLiquid = new QTreeWidgetItem();
        m_itemStorageLiquid->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemStorage->addChild(m_itemStorageLiquid);
    }

    m_itemDockingbay = new QTreeWidgetItem();
    m_itemDockingbay->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemDockingbay);
    m_itemDockingbay->setExpanded(true);
    {
        m_itemDockingbaySDock = new QTreeWidgetItem();
        m_itemDockingbaySDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemDockingbay->addChild(m_itemDockingbaySDock);

        m_itemDockingbayMDock = new QTreeWidgetItem();
        m_itemDockingbayMDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemDockingbay->addChild(m_itemDockingbayMDock);

        m_itemDockingbayLDock = new QTreeWidgetItem();
        m_itemDockingbayLDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemDockingbay->addChild(m_itemDockingbayLDock);

        m_itemDockingbayXLDock = new QTreeWidgetItem();
        m_itemDockingbayXLDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemDockingbay->addChild(m_itemDockingbayXLDock);

        m_itemDockingbayLXLDock = new QTreeWidgetItem();
        m_itemDockingbayLXLDock->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemDockingbay->addChild(m_itemDockingbayLXLDock);
    }

    m_itemShipStorage = new QTreeWidgetItem();
    m_itemShipStorage->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemShipStorage);
    m_itemShipStorage->setExpanded(true);
    {
        m_itemShipStorageSShipCargo = new QTreeWidgetItem();
        m_itemShipStorageSShipCargo->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemShipStorage->addChild(m_itemShipStorageSShipCargo);

        m_itemShipStorageMShipCargo = new QTreeWidgetItem();
        m_itemShipStorageMShipCargo->setFlags(Qt::ItemFlag::ItemIsEnabled);
        m_itemShipStorage->addChild(m_itemShipStorageMShipCargo);
    }

    m_itemWorkforce = new QTreeWidgetItem();
    m_itemWorkforce->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemWorkforce);

    m_itemSurplusWorkforce = new QTreeWidgetItem();
    m_itemSurplusWorkforce->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemSurplusWorkforce);

    m_itemResources = new WaresItem();
    m_itemResources->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemResources);

    m_itemIntermediates = new WaresItem();
    m_itemIntermediates->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemIntermediates);

    m_itemProducts = new WaresItem();
    m_itemProducts->setFlags(Qt::ItemFlag::ItemIsEnabled);
    m_itemSummary->addChild(m_itemProducts);

    // Change language.
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &EditorWidget::onLanguageChanged);

    m_treeEditor->expandAll();

    // Add to container.
    parent->setWidget(this);
    parent->show();
    this->show();

    this->setAttribute(Qt::WA_DeleteOnClose);
    parent->setAttribute(Qt::WA_DeleteOnClose);

    if (m_save->path() != "") {
        _opendFiles[m_save->path()] = this;
    }

    this->onLanguageChanged();
    this->updateSummary();
}

/**
 * @brief	Destructors.
 */
EditorWidget::~EditorWidget() {}

/**
 * @brief		Do operation.
 */
void EditorWidget::doOperation(::std::shared_ptr<Operation> operation)
{
    if (operation->doOperation()) {
        m_redoStack.clear();
        m_undoStack.push_back(operation);
        qDebug() << "Operation done.";
        this->updateSaveStatus();
        this->updateUndoRedoStatus();
        this->updateSummary();
    } else {
        qDebug() << "Operation failed.";
    }
}

/**
 * @brief       Update window title.
 */
void EditorWidget::updateTitle()
{
    if (m_save->path() == "") {
        this->setWindowTitle(STR("STR_NEW_STATION"));
    } else {
        this->setWindowTitle(
            m_save->path().split("/", Qt::SkipEmptyParts).back());
    }
}

/**
 * @brief       Update button "Save" status.
 */
void EditorWidget::updateSaveStatus()
{
    if (m_savedUndoCount == m_undoStack.size()) {
        m_fileActions->actionFileSave->setEnabled(false);
    } else {
        m_fileActions->actionFileSave->setEnabled(true);
    }
}

/**
 * @brief       Update button "Add to Station" status.
 */
void EditorWidget::updateAddToStationStatus()
{
    if (m_itemGroups->childCount() == 0) {
        emit this->addToStationStatusChaged(false);
    } else {
        emit this->addToStationStatusChaged(true);
    }
}

/**
 * @brief       Update undo/redo action statis.
 */
void EditorWidget::updateUndoRedoStatus()
{
    if (m_undoStack.empty()) {
        m_editActions->actionEditUndo->setEnabled(false);
    } else {
        m_editActions->actionEditUndo->setEnabled(true);
    }

    if (m_redoStack.empty()) {
        m_editActions->actionEditRedo->setEnabled(false);
    } else {
        m_editActions->actionEditRedo->setEnabled(true);
    }
}

/**
 * @brief       Update cut/copy/remove action statis.
 */
void EditorWidget::updateCutCopyRemoveStatus()
{
    bool found       = false;
    bool foundGroup  = false;
    bool foundModule = false;
    if (m_treeEditor != nullptr) {
        for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
            if (dynamic_cast<GroupItem *>(rawItem) != nullptr) {
                if (foundModule) {
                    found = false;
                    break;
                } else {
                    foundGroup = true;
                    found      = true;
                }
            } else if (dynamic_cast<ModuleItem *>(rawItem) != nullptr) {
                if (foundGroup) {
                    found = false;
                    break;
                } else {
                    foundModule = true;
                    found       = true;
                }
            }
        }
    }

    m_editActions->actionEditCut->setEnabled(found);
    m_editActions->actionEditCopy->setEnabled(found);
    m_editActions->actionEditRemove->setEnabled(foundModule || foundGroup);
}

/**
 * @brief       Update paste action statis.
 */
void EditorWidget::updatePasteStatus()
{
    // Check clipboard.
    QClipboard *clipboard = QApplication::clipboard();

    const QMimeData *data = clipboard->mimeData();
    if (data->hasFormat(X4SCGroupClipboardMimeDataBuilder::_mimeTypeStr)) {
        // Check selection.
        QTreeWidgetItem *item = m_treeEditor->currentItem();
        if (item == m_itemGroups
            || dynamic_cast<GroupItem *>(item) != nullptr) {
            m_editActions->actionEditPaste->setEnabled(true);
        } else {
            m_editActions->actionEditPaste->setEnabled(false);
        }
    } else if (data->hasFormat(
                   X4SCModuleClipboardMimeDataBuilder::_mimeTypeStr)) {
        // Check selection.
        QTreeWidgetItem *item = m_treeEditor->currentItem();
        if (dynamic_cast<GroupItem *>(item) != nullptr
            || dynamic_cast<ModuleItem *>(item) != nullptr) {
            m_editActions->actionEditPaste->setEnabled(true);
        } else {
            m_editActions->actionEditPaste->setEnabled(false);
        }
    } else {
        m_editActions->actionEditPaste->setEnabled(false);
    }
}

/**
 * @brief       Update move button status.
 */
void EditorWidget::updateGroupMoveButtonStatus(GroupItem *      item,
                                               GroupItemWidget *itemWidget)
{
    int index = m_itemGroups->indexOfChild(item);

    if (itemWidget == nullptr) {
        itemWidget = dynamic_cast<GroupItemWidget *>(
            m_treeEditor->itemWidget(item, 1));
        Q_ASSERT(itemWidget != nullptr);
    }

    if (index == 0) {
        if (index == m_itemGroups->childCount() - 1) {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(false);
        } else {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(true);
        }
    } else if (index == m_itemGroups->childCount() - 1) {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(false);
    } else {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(true);
    }
}

/**
 * @brief       Update move button status.
 */
void EditorWidget::updateModuleMoveButtonStatus(ModuleItem *      item,
                                                ModuleItemWidget *itemWidget)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);
    int index = groupItem->indexOfChild(item);

    if (itemWidget == nullptr) {
        itemWidget = dynamic_cast<ModuleItemWidget *>(
            m_treeEditor->itemWidget(item, 1));
        Q_ASSERT(itemWidget != nullptr);
    }

    if (index == 0) {
        if (index == groupItem->childCount() - 1) {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(false);
        } else {
            itemWidget->setUpBtnEnabled(false);
            itemWidget->setDownBtnEnabled(true);
        }
    } else if (index == groupItem->childCount() - 1) {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(false);
    } else {
        itemWidget->setUpBtnEnabled(true);
        itemWidget->setDownBtnEnabled(true);
    }
}

/**
 * @brief       Update summary.
 */
void EditorWidget::updateSummary()
{
    this->disableSuggestedAmounts();

    SummaryInfo summary;
    this->makeSummary(summary);
    this->showSummary(summary);
    this->checkSummary(summary);

    this->updateSuggestedAmounts(summary);
}

/**
 * @brief       Disable suggested amounts.
 */
void EditorWidget::disableSuggestedAmounts()
{
    // Group.
    for (int groupIndex = 0; groupIndex < m_itemGroups->childCount();
         ++groupIndex) {
        QTreeWidgetItem *groupItem = m_itemGroups->child(groupIndex);

        // Module.
        for (int moduleIndex = 0; moduleIndex < groupItem->childCount();
             ++moduleIndex) {
            QTreeWidgetItem * moduleItem = groupItem->child(moduleIndex);
            ModuleItemWidget *widget     = dynamic_cast<ModuleItemWidget *>(
                m_treeEditor->itemWidget(moduleItem, 1));
            if (widget != nullptr) {
                widget->setSuggestAmountEnabled(false);
            }
        }
    }
}

/**
 * @brief       Update suggested amounts.
 */
void EditorWidget::updateSuggestedAmounts(SummaryInfo &summary)
{
    // Group.
    for (int groupIndex = 0; groupIndex < m_itemGroups->childCount();
         ++groupIndex) {
        QTreeWidgetItem *groupItem = m_itemGroups->child(groupIndex);

        // Item.
        for (int moduleIndex = 0; moduleIndex < groupItem->childCount();
             ++moduleIndex) {
            // Module Item.
            ModuleItem *moduleItem
                = dynamic_cast<ModuleItem *>(groupItem->child(moduleIndex));
            if (moduleItem == nullptr) {
                continue;
            }

            // Module Widget.
            ModuleItemWidget *widget = dynamic_cast<ModuleItemWidget *>(
                m_treeEditor->itemWidget(moduleItem, 1));
            if (widget == nullptr) {
                continue;
            }

            // Station module.
            auto module = GameData::instance()->stationModules()->module(
                moduleItem->module()->module());
            if (module == nullptr) {
                continue;
            }

            switch (module->moduleClass) {
                case GameStationModules::StationModule::StationModuleClass::
                    Habitation: {
                    widget->setSuggestedAmountToChange(
                        static_cast<qint64>(-summary.surplusWorkforce));
                    widget->setSuggestAmountEnabled(true);

                } break;

                case GameStationModules::StationModule::StationModuleClass::
                    Production: {
                    // Search property.
                    auto iter = module->properties.find(
                        GameStationModules::Property::Type::SupplyProduct);
                    if (iter == module->properties.end()) {
                        break;
                    }
                    ::std::shared_ptr<GameStationModules::SupplyProduct>
                        property = ::std::static_pointer_cast<
                            GameStationModules::SupplyProduct>(*iter);

                    // Search intermediate.
                    auto intermediatesIter
                        = summary.intermediates.find(property->product);
                    if (intermediatesIter == summary.intermediates.end()) {
                        break;
                    }

                    widget->setSuggestedAmountToChange(
                        static_cast<qint64>(-intermediatesIter->max()));
                    widget->setSuggestAmountEnabled(true);
                } break;

                default:
                    break;
            }
        }
    }
}

/**
 * @brief       Make summary.
 */
void EditorWidget::makeSummary(SummaryInfo &summary)
{
    auto gameStationModules = GameData::instance()->stationModules();
    auto gameWares          = GameData::instance()->wares();

    // Count summary.
    for (auto saveGroup : m_save->groups()) {
        for (auto saveModule : saveGroup->modules()) {
            auto module = gameStationModules->module(saveModule->module());

            // Hull & explosion damage.
            summary.hull += module->hull * saveModule->amount();
            summary.explosionDamage
                += module->explosiondamage * saveModule->amount();

            // Properties.
            for (auto property : module->properties) {
                switch (property->type) {
                    case GameStationModules::Property::Type::MTurret:
                        // Has M turret.
                        {
                            ::std::shared_ptr<GameStationModules::HasMTurret>
                                hasMTurret = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasMTurret>(property);
                            summary.weapons.mTurret
                                += hasMTurret->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::MShield:
                        // Has M shield.
                        {
                            ::std::shared_ptr<GameStationModules::HasMShield>
                                hasMShield = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasMShield>(property);
                            summary.shields.mShield
                                += hasMShield->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::LTurret:
                        // Has L turret.
                        {
                            ::std::shared_ptr<GameStationModules::HasLTurret>
                                hasLTurret = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasLTurret>(property);
                            summary.weapons.lTurret
                                += hasLTurret->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::LShield:
                        // Has L shield.
                        {
                            ::std::shared_ptr<GameStationModules::HasLShield>
                                hasLShield = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasLShield>(property);
                            summary.shields.lShield
                                += hasLShield->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::SDock:
                        // Has S docking bay.
                        {
                            ::std::shared_ptr<GameStationModules::HasSDock>
                                hasSDock = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasSDock>(property);
                            summary.dockingBay.sDock
                                += hasSDock->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::SShipCargo:
                        // Has S ship cargo.
                        {
                            ::std::shared_ptr<GameStationModules::HasSShipCargo>
                                hasSShipCargo = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasSShipCargo>(
                                    property);
                            summary.shipStorage.sShipCargo
                                += hasSShipCargo->capacity
                                   * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::MDock:
                        // Has M docking bay.
                        {
                            ::std::shared_ptr<GameStationModules::HasMDock>
                                hasMDock = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasMDock>(property);
                            summary.dockingBay.mDock
                                += hasMDock->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::MShipCargo:
                        // Has M ship cargo.
                        {
                            ::std::shared_ptr<GameStationModules::HasMShipCargo>
                                hasMShipCargo = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasMShipCargo>(
                                    property);
                            summary.shipStorage.mShipCargo
                                += hasMShipCargo->capacity
                                   * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::LDock:
                        // Has L docking bay.
                        {
                            ::std::shared_ptr<GameStationModules::HasLDock>
                                hasLDock = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasLDock>(property);
                            summary.dockingBay.lDock
                                += hasLDock->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::XLDock:
                        // Has XL docking bay.
                        {
                            ::std::shared_ptr<GameStationModules::HasXLDock>
                                hasXLDock = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasXLDock>(property);
                            summary.dockingBay.xlDock
                                += hasXLDock->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::LXLDock:
                        // Has L/XL docking bay.
                        {
                            ::std::shared_ptr<GameStationModules::HasLXLDock>
                                hasLXLDock = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasLXLDock>(property);
                            summary.dockingBay.lXLDock
                                += hasLXLDock->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::SLaunchTube:
                        // Has S launch tube.
                        {
                            ::std::shared_ptr<
                                GameStationModules::HasSLaunchTube>
                                hasSLaunchTube = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasSLaunchTube>(
                                    property);
                            summary.weapons.sLaunchTube
                                += hasSLaunchTube->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::MLaunchTube:
                        // Has M launch tube.
                        {
                            ::std::shared_ptr<
                                GameStationModules::HasMLaunchTube>
                                hasMLaunchTube = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasMLaunchTube>(
                                    property);
                            summary.weapons.mLaunchTube
                                += hasMLaunchTube->count * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::SupplyWorkforce:
                        // Supply workforce.
                        {
                            ::std::shared_ptr<
                                GameStationModules::SupplyWorkforce>
                                supplyWorkforce = ::std::dynamic_pointer_cast<
                                    GameStationModules::SupplyWorkforce>(
                                    property);

                            // Workforce.
                            summary.workforce += supplyWorkforce->workforce
                                                 * saveModule->amount();
                            summary.surplusWorkforce
                                += (qint64)(supplyWorkforce->workforce)
                                   * saveModule->amount();

                            // Supply.
                            for (auto resource :
                                 supplyWorkforce->supplyInfo->resources) {
                                // Find/create ware.
                                const QString &macro = resource->id;
                                auto iter = summary.resources.find(macro);
                                if (iter == summary.resources.end()) {
                                    summary.resources[macro] = Range<
                                        long double>(
                                        0.0,
                                        (((long double)resource->amount)
                                         * supplyWorkforce->workforce * 3600
                                         * saveModule->amount()
                                         / supplyWorkforce->supplyInfo->amount
                                         / supplyWorkforce->supplyInfo->time));
                                } else {
                                    // Increase amount.
                                    iter->setMax(
                                        iter->max()
                                        + (((long double)resource->amount)
                                           * supplyWorkforce->workforce * 3600
                                           * saveModule->amount()
                                           / supplyWorkforce->supplyInfo->amount
                                           / supplyWorkforce->supplyInfo
                                                 ->time));
                                }

                                // Transport type.
                                switch (gameWares->ware(macro)->transportType) {
                                    case GameWares::TransportType::Container:
                                        // Container.
                                        summary.requirements
                                            .requireContainerStorage
                                            = true;
                                        break;

                                    case GameWares::TransportType::Solid:
                                        // Solid.
                                        summary.requirements.requireSolidStorage
                                            = true;
                                        break;

                                    case GameWares::TransportType::Liquid:
                                        // Liquid.
                                        summary.requirements
                                            .requireLiquidStorage
                                            = true;
                                        break;

                                    default:
                                        break;
                                }
                            }
                        }
                        break;

                    case GameStationModules::Property::Type::RequireWorkforce:
                        // Require workforce.
                        {
                            ::std::shared_ptr<
                                GameStationModules::RequireWorkforce>
                                requireWorkforce = ::std::dynamic_pointer_cast<
                                    GameStationModules::RequireWorkforce>(
                                    property);
                            summary.surplusWorkforce
                                -= (qint64)(requireWorkforce->workforce)
                                   * saveModule->amount();
                        }
                        break;

                    case GameStationModules::Property::Type::SupplyProduct:
                        // Supply product.
                        {
                            ::std::shared_ptr<GameStationModules::SupplyProduct>
                                supplyProfduct = ::std::dynamic_pointer_cast<
                                    GameStationModules::SupplyProduct>(
                                    property);
                            ::std::shared_ptr<GameWares::ProductionInfo>
                                productionInfo = supplyProfduct->productionInfo;
                            const QString &macro = productionInfo->id;

                            // Work effect.
                            long double workEffect = 0.0;
                            auto propertyIter = productionInfo->properties.find(
                                GameWares::ProductionInfoProperty::
                                    PropertyType::Effect);
                            if (propertyIter
                                != productionInfo->properties.end()) {
                                ::std::shared_ptr<GameWares::Effect> effect
                                    = ::std::static_pointer_cast<
                                        GameWares::Effect>(
                                        propertyIter.value());
                                workEffect = effect->product;
                            }

                            // Product.
                            // Find/create ware.
                            auto iter = summary.products.find(macro);
                            if (iter == summary.products.end()) {
                                summary.products[macro] = Range<long double>(
                                    (long double)(productionInfo->amount) * 3600
                                        * saveModule->amount()
                                        / productionInfo->time,
                                    (long double)(productionInfo->amount) * 3600
                                        * saveModule->amount()
                                        * (1.0 + workEffect)
                                        / productionInfo->time);
                            } else {
                                // Increase amount.
                                iter->setRange(
                                    (long double)(productionInfo->amount) * 3600
                                            * saveModule->amount()
                                            / productionInfo->time
                                        + iter->min(),
                                    (long double)(productionInfo->amount) * 3600
                                            * saveModule->amount()
                                            * (1.0 + workEffect)
                                            / productionInfo->time
                                        + iter->max());
                            }

                            // Transport type.
                            switch (gameWares->ware(macro)->transportType) {
                                case GameWares::TransportType::Container:
                                    // Container.
                                    summary.requirements.requireContainerStorage
                                        = true;
                                    break;

                                case GameWares::TransportType::Solid:
                                    // Solid.
                                    summary.requirements.requireSolidStorage
                                        = true;
                                    break;

                                case GameWares::TransportType::Liquid:
                                    // Liquid.
                                    summary.requirements.requireLiquidStorage
                                        = true;
                                    break;

                                default:
                                    break;
                            }

                            // Resources.
                            for (auto resouce : productionInfo->resources) {
                                const QString &macro = resouce->id;

                                // Work effect.
                                long double workEffect = 0.0;
                                auto        propertyIter
                                    = productionInfo->properties.find(
                                        GameWares::ProductionInfoProperty::
                                            PropertyType::Effect);
                                if (propertyIter
                                    != productionInfo->properties.end()) {
                                    ::std::shared_ptr<GameWares::Effect> effect
                                        = ::std::static_pointer_cast<
                                            GameWares::Effect>(
                                            propertyIter.value());
                                    workEffect = effect->product;
                                }

                                // Find/create ware.
                                auto iter = summary.resources.find(macro);
                                if (iter == summary.resources.end()) {
                                    summary.resources[macro]
                                        = Range<long double>(
                                            (long double)(resouce->amount)
                                                * 3600 * saveModule->amount()
                                                / productionInfo->time,
                                            (long double)(resouce->amount)
                                                * 3600 * saveModule->amount()
                                                * (1.0 + workEffect)
                                                / productionInfo->time);
                                } else {
                                    // Increase amount.
                                    iter->setRange(
                                        (long double)(resouce->amount) * 3600
                                                * saveModule->amount()
                                                / productionInfo->time
                                            + iter->min(),
                                        (long double)(resouce->amount) * 3600
                                                * saveModule->amount()
                                                * (1.0 + workEffect)
                                                / productionInfo->time
                                            + iter->max());
                                }

                                // Transport type.
                                switch (gameWares->ware(macro)->transportType) {
                                    case GameWares::TransportType::Container:
                                        // Container.
                                        summary.requirements
                                            .requireContainerStorage
                                            = true;
                                        break;

                                    case GameWares::TransportType::Solid:
                                        // Solid.
                                        summary.requirements.requireSolidStorage
                                            = true;
                                        break;

                                    case GameWares::TransportType::Liquid:
                                        // Liquid.
                                        summary.requirements
                                            .requireLiquidStorage
                                            = true;
                                        break;

                                    default:
                                        break;
                                }
                            }
                        }
                        break;

                    case GameStationModules::Property::Type::Cargo:
                        // Has cargo.
                        {
                            ::std::shared_ptr<GameStationModules::HasCargo>
                                hasCargo = ::std::dynamic_pointer_cast<
                                    GameStationModules::HasCargo>(property);

                            switch (hasCargo->cargoType) {
                                case GameWares::TransportType::Container:
                                    // Container.
                                    summary.storage.container
                                        += hasCargo->cargoSize
                                           * saveModule->amount();
                                    break;

                                case GameWares::TransportType::Solid:
                                    // Solid.
                                    summary.storage.solid
                                        += hasCargo->cargoSize
                                           * saveModule->amount();
                                    break;

                                case GameWares::TransportType::Liquid:
                                    // Liquid.
                                    summary.storage.liquid
                                        += hasCargo->cargoSize
                                           * saveModule->amount();
                                    break;

                                default:
                                    break;
                            }
                        }
                        break;
                }
            }
        }
    }

    // Intermediates
    for (auto macro : summary.products.keys()) {
        if (summary.resources.find(macro) != summary.resources.end()) {
            auto productRange  = summary.products[macro];
            auto resourceRange = summary.resources[macro];
            summary.intermediates[macro]
                = Range<long double>(productRange.min() - resourceRange.min(),
                                     productRange.max() - resourceRange.max());
            summary.products.remove(macro);
            summary.resources.remove(macro);
        }
    }
}

/**
 * @brief       Show summary.
 *
 * @param[in]   summary     Summary.
 */
void EditorWidget::showSummary(const SummaryInfo &summary)
{
#define SET_HIDE_ZERO(item, value)                      \
    {                                                   \
        (item)->setText(1, QString("%1").arg((value))); \
        (item)->setHidden((value) == 0);                \
    }
    m_itemHull->setText(1, QString("%1").arg(summary.hull));
    m_itemExplosionDamage->setText(1,
                                   QString("%1").arg(summary.explosionDamage));

    m_itemWeapons->setHidden(
        summary.weapons.sLaunchTube == 0 && summary.weapons.mLaunchTube == 0
        && summary.weapons.mTurret == 0 && summary.weapons.lTurret == 0);
    SET_HIDE_ZERO(m_itemWeaponSLaunchTube, summary.weapons.sLaunchTube);
    SET_HIDE_ZERO(m_itemWeaponMLaunchTube, summary.weapons.mLaunchTube);
    SET_HIDE_ZERO(m_itemWeaponMTurret, summary.weapons.mTurret);
    SET_HIDE_ZERO(m_itemWeaponLTurret, summary.weapons.lTurret);

    m_itemShields->setHidden(summary.shields.mShield == 0
                             && summary.shields.lShield == 0);
    SET_HIDE_ZERO(m_itemShieldMShield, summary.shields.mShield);
    SET_HIDE_ZERO(m_itemShieldLShield, summary.shields.lShield);

    m_itemStorage->setHidden(summary.storage.container == 0
                             && summary.storage.solid == 0
                             && summary.storage.liquid == 0);
    SET_HIDE_ZERO(m_itemStorageContainer, summary.storage.container);
    SET_HIDE_ZERO(m_itemStorageSolid, summary.storage.solid);
    SET_HIDE_ZERO(m_itemStorageLiquid, summary.storage.liquid);

    m_itemDockingbay->setHidden(
        summary.dockingBay.sDock == 0 && summary.dockingBay.mDock == 0
        && summary.dockingBay.lDock == 0 && summary.dockingBay.xlDock == 0
        && summary.dockingBay.lXLDock == 0);
    SET_HIDE_ZERO(m_itemDockingbaySDock, summary.dockingBay.sDock);
    SET_HIDE_ZERO(m_itemDockingbayMDock, summary.dockingBay.mDock);
    SET_HIDE_ZERO(m_itemDockingbayLDock, summary.dockingBay.lDock);
    SET_HIDE_ZERO(m_itemDockingbayXLDock, summary.dockingBay.xlDock);
    SET_HIDE_ZERO(m_itemDockingbayLXLDock, summary.dockingBay.lXLDock);

    m_itemShipStorage->setHidden(summary.shipStorage.sShipCargo == 0
                                 && summary.shipStorage.mShipCargo == 0);
    SET_HIDE_ZERO(m_itemShipStorageSShipCargo, summary.shipStorage.sShipCargo);
    SET_HIDE_ZERO(m_itemShipStorageMShipCargo, summary.shipStorage.sShipCargo);

    m_itemWorkforce->setText(1, QString("%1").arg(summary.workforce));
    m_itemSurplusWorkforce->setText(
        1, QString("%1").arg(summary.surplusWorkforce));

    m_itemResources->update(summary.resources);
    m_itemResources->setHidden(m_itemResources->wareCount() == 0);

    m_itemIntermediates->update(summary.intermediates);
    m_itemIntermediates->setHidden(m_itemIntermediates->wareCount() == 0);

    m_itemProducts->update(summary.products);
    m_itemProducts->setHidden(m_itemProducts->wareCount() == 0);

#undef SET_HIDE_ZERO
}

/**
 * @brief       Check summary.
 *
 * @param[in]   summary     Summary.
 */
void EditorWidget::checkSummary(const SummaryInfo &summary)
{
    this->clearWarnings();
    if (summary.surplusWorkforce < 0) {
        this->addWarning("STR_EDITOR_WARING_NOT_ENOUGH_WORKFORCE");
    }

    if (summary.requirements.requireContainerStorage
        && summary.storage.container == 0) {
        this->addWarning("STR_EDITOR_WARING_MISSING_CONTAINER_STORAGE");
    }

    if (summary.requirements.requireSolidStorage
        && summary.storage.solid == 0) {
        this->addWarning("STR_EDITOR_WARING_MISSING_SOLID_STORAGE");
    }

    if (summary.requirements.requireLiquidStorage
        && summary.storage.liquid == 0) {
        this->addWarning("STR_EDITOR_WARING_MISSING_LIQUID_STORAGE");
    }

    if (summary.dockingBay.sDock == 0 && summary.dockingBay.mDock == 0) {
        this->addWarning("STR_EDITOR_WARING_MISSING_DOCKINGBAY");
    }
}

/**
 * @brief	Close save file.
 */
bool EditorWidget::closeSave()
{
    if (m_save == nullptr) {
        return true;
    }

    if (m_savedUndoCount == m_undoStack.size()) {
        return true;
    } else {
        switch (QMessageBox::question(
            this, STR("STR_TITLE_SAVE_STATION"),
            STR("STR_SAVE_STATION").arg(this->windowTitle()),
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No
                | QMessageBox::StandardButton::Cancel,
            QMessageBox::StandardButton::Yes)) {
            case QMessageBox::StandardButton::Yes:
                this->save();
                return m_savedUndoCount == m_undoStack.size();
                break;

            case QMessageBox::StandardButton::No:
                m_savedUndoCount = m_undoStack.size();
                return true;
                break;

            case QMessageBox::StandardButton::Cancel:
            default:
                return false;
        }
    }
}

/**
 * @brief	Load groups.
 */
void EditorWidget::loadGroups()
{
    // Load groups.
    for (::std::shared_ptr<SaveGroup> group : m_save->groups()) {
        GroupItem *groupItem = new GroupItem(group);
        m_itemGroups->addChild(groupItem);
        GroupItemWidget *groupWidget = new GroupItemWidget(groupItem);
        groupItem->treeWidget()->setItemWidget(groupItem, 1, groupWidget);
        groupWidget->show();
        this->connect(groupWidget, &GroupItemWidget::btnUpClicked, this,
                      &EditorWidget::onGroupMoveUp);
        this->connect(groupWidget, &GroupItemWidget::btnDownClicked, this,
                      &EditorWidget::onGroupMoveDown);
        this->connect(groupWidget, &GroupItemWidget::btnRemoveClicked, this,
                      &EditorWidget::removeGroupItem);

        // Load modules.
        for (::std::shared_ptr<SaveModule> module : group->modules()) {
            ModuleItem *moduleItem = new ModuleItem(module);
            groupItem->addChild(moduleItem);
            ModuleItemWidget *moduleWidget = new ModuleItemWidget(moduleItem);
            moduleItem->treeWidget()->setItemWidget(moduleItem, 1,
                                                    moduleWidget);
            moduleWidget->show();
            ::std::shared_ptr<ModuleInfo> moduleInfo(
                new ModuleInfo({moduleItem, moduleWidget}));

            this->connect(moduleWidget, &ModuleItemWidget::changeAmount, this,
                          &EditorWidget::onChangeAmount);
            this->connect(moduleWidget, &ModuleItemWidget::btnUpClicked, this,
                          &EditorWidget::onModuleMoveUp);
            this->connect(moduleWidget, &ModuleItemWidget::btnDownClicked, this,
                          &EditorWidget::onModuleMoveDown);
            this->connect(moduleWidget, &ModuleItemWidget::btnRemoveClicked,
                          this, &EditorWidget::removeModuleItem);
        }

        ModuleItem *first = dynamic_cast<ModuleItem *>(groupItem->child(0));
        if (first != nullptr) {
            this->updateModuleMoveButtonStatus(first);
        }

        ModuleItem *last = dynamic_cast<ModuleItem *>(
            groupItem->child(groupItem->childCount() - 1));
        if (last != nullptr) {
            this->updateModuleMoveButtonStatus(last);
        }
    }

    GroupItem *first = dynamic_cast<GroupItem *>(m_itemGroups->child(0));
    if (first != nullptr) {
        this->updateGroupMoveButtonStatus(first);
    }

    GroupItem *last = dynamic_cast<GroupItem *>(
        m_itemGroups->child(m_itemGroups->childCount() - 1));
    if (last != nullptr) {
        this->updateGroupMoveButtonStatus(last);
    }
}

/**
 * @brief		Close event.
 */
void EditorWidget::closeEvent(QCloseEvent *event)
{
    if (this->closeSave()) {
        _opendFiles.remove(m_save->path());
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief	    Add module.
 */
void EditorWidget::addModules(const QStringList &macros)
{
    // Get item.
    GroupItem *groupItem = dynamic_cast<GroupItem *>(
        m_itemGroups->child(m_itemGroups->childCount() - 1));
    Q_ASSERT(groupItem != nullptr);
    ModuleItem *moduleItem = nullptr;

    QTreeWidgetItem *item = m_treeEditor->currentItem();
    if (item != nullptr) {
        if (dynamic_cast<GroupItem *>(item) != nullptr) {
            groupItem = static_cast<GroupItem *>(item);
        } else if (dynamic_cast<ModuleItem *>(item) != nullptr) {
            moduleItem = static_cast<ModuleItem *>(item);
            groupItem  = static_cast<GroupItem *>(item->parent());
        }
    }

    int groupIndex  = m_itemGroups->indexOfChild(groupItem);
    int moduleIndex = 0;
    if (moduleItem == nullptr) {
        moduleIndex = groupItem->childCount();
    } else {
        moduleIndex = groupItem->indexOfChild(moduleItem) + 1;
    }

    // Do operation.
    ::std::shared_ptr<Operation> operation
        = AddModuleOperation::create(groupIndex, moduleIndex, macros, this);
    this->doOperation(operation);
}

/**
 * @brief		Create new group.
 */
void EditorWidget::newGroup()
{
    // Get index.
    int index = m_itemGroups->childCount();

    QTreeWidgetItem *item = m_treeEditor->currentItem();
    if (item != nullptr) {
        if (dynamic_cast<GroupItem *>(item) != nullptr) {
            index = m_itemGroups->indexOfChild(item) + 1;
        } else if (dynamic_cast<ModuleItem *>(item) != nullptr) {
            index = m_itemGroups->indexOfChild(item->parent()) + 1;
        }
    }

    // Do operation.
    ::std::shared_ptr<Operation> operation
        = AddGroupOperation::create(index, this);
    this->doOperation(operation);

    this->updateAddToStationStatus();
}

/**
 * @brief		Undo.
 */
void EditorWidget::undo()
{
    if (m_undoStack.empty()) {
        return;
    }

    // Get opetarion.
    ::std::shared_ptr<Operation> op = m_undoStack.takeLast();

    // Undo.
    op->undoOperation();

    // Add to redo stack.
    m_redoStack.push_back(op);

    this->updateSaveStatus();
    this->updateUndoRedoStatus();
    this->updateSummary();
}

/**
 * @brief		Redo.
 */
void EditorWidget::redo()
{
    if (m_redoStack.empty()) {
        return;
    }

    // Get opetarion.
    ::std::shared_ptr<Operation> op = m_redoStack.takeLast();

    // Redo.
    op->doOperation();

    // Add to redo stack.
    m_undoStack.push_back(op);

    this->updateUndoRedoStatus();
}

/**
 * @brief		Cut.
 */
void EditorWidget::cut()
{
    QVector<GroupItem *>                         groupItems;
    QVector<::std::shared_ptr<const SaveGroup>>  groups;
    QVector<ModuleItem *>                        moduleItems;
    QVector<::std::shared_ptr<const SaveModule>> modules;

    // Scan items.
    for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
        {
            GroupItem *item = dynamic_cast<GroupItem *>(rawItem);

            if (item != nullptr) {
                groupItems.push_back(item);
                groups.append(item->group());
            }
        }

        {
            ModuleItem *item = dynamic_cast<ModuleItem *>(rawItem);

            if (item != nullptr) {
                moduleItems.push_back(item);
                modules.append(item->module());
            }
        }
    }

    if (moduleItems.empty()) {
        // Groups
        // Get mimedata.
        QMimeData *                       mimeData = new QMimeData();
        X4SCGroupClipboardMimeDataBuilder builder;
        builder.setData(groups);
        builder.saveMimeData(mimeData);

        // Set clipboard.
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);

        // Remove selected.
        ::std::shared_ptr<Operation> operation
            = RemoveOperation::create(groupItems, {}, this);
        this->doOperation(operation);
    } else if (! moduleItems.empty()) {
        // Modules
        // Get mimedata.
        QMimeData *                        mimeData = new QMimeData();
        X4SCModuleClipboardMimeDataBuilder builder;
        builder.setData(modules);
        builder.saveMimeData(mimeData);

        // Set clipboard.
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);

        // Remove selected.
        ::std::shared_ptr<Operation> operation
            = RemoveOperation::create({}, moduleItems, this);
        this->doOperation(operation);
    }

    this->updateAddToStationStatus();
    this->updatePasteStatus();
}

/**
 * @brief		Copy.
 */
void EditorWidget::copy()
{
    QVector<::std::shared_ptr<const SaveGroup>>  groups;
    QVector<::std::shared_ptr<const SaveModule>> modules;

    // Scan items.
    for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
        {
            GroupItem *item = dynamic_cast<GroupItem *>(rawItem);

            if (item != nullptr) {
                groups.append(item->group());
            }
        }

        {
            ModuleItem *item = dynamic_cast<ModuleItem *>(rawItem);

            if (item != nullptr) {
                modules.append(item->module());
            }
        }
    }

    if (modules.empty()) {
        // Groups
        // Get mimedata.
        QMimeData *                       mimeData = new QMimeData();
        X4SCGroupClipboardMimeDataBuilder builder;
        builder.setData(groups);
        builder.saveMimeData(mimeData);

        // Set clipboard.
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);
    } else if (! modules.empty()) {
        // Modules
        // Get mimedata.
        QMimeData *                        mimeData = new QMimeData();
        X4SCModuleClipboardMimeDataBuilder builder;
        builder.setData(modules);
        builder.saveMimeData(mimeData);

        // Set clipboard.
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);
    }

    this->updateAddToStationStatus();
    this->updatePasteStatus();
}

/**
 * @brief		Paste.
 */
void EditorWidget::paste()
{
    // Get clipboard.
    QClipboard *clipboard = QApplication::clipboard();

    const QMimeData *data = clipboard->mimeData();
    if (data->hasFormat(X4SCGroupClipboardMimeDataBuilder::_mimeTypeStr)) {
        // Groups.
        // Get data.
        X4SCGroupClipboardMimeDataBuilder builder;
        builder.loadMimeData(data);
        QTreeWidgetItem *item = m_treeEditor->currentItem();

        // Get position.
        GroupItem *groupItem = nullptr;
        if (m_itemGroups->childCount() > 0) {
            if (item == m_itemGroups) {
                groupItem = dynamic_cast<GroupItem *>(
                    item->child(item->childCount() - 1));
            } else {
                groupItem = dynamic_cast<GroupItem *>(item);
            }
        }

        // Do operation.
        ::std::shared_ptr<Operation> operation
            = PasteGroupOperation::create(groupItem, builder, this);
        this->doOperation(operation);
    } else if (data->hasFormat(
                   X4SCModuleClipboardMimeDataBuilder::_mimeTypeStr)) {
        // Moudles.
        // Get data.
        X4SCModuleClipboardMimeDataBuilder builder;
        builder.loadMimeData(data);
        QTreeWidgetItem *item = m_treeEditor->currentItem();

        GroupItem * groupItem  = nullptr;
        ModuleItem *moduleItem = nullptr;
        if (dynamic_cast<GroupItem *>(item) != nullptr) {
            groupItem = dynamic_cast<GroupItem *>(item);
            if (groupItem->childCount() != 0) {
                moduleItem = dynamic_cast<ModuleItem *>(
                    groupItem->child(groupItem->childCount() - 1));
            }
        } else if (dynamic_cast<ModuleItem *>(item) != nullptr) {
            groupItem = dynamic_cast<GroupItem *>(item->parent());
            Q_ASSERT(groupItem != nullptr);
            moduleItem = dynamic_cast<ModuleItem *>(item);
        }

        // Do operation.
        ::std::shared_ptr<Operation> operation = PasteModuleOperation::create(
            groupItem, moduleItem, builder, this);
        this->doOperation(operation);
    }
}

/**
 * @brief		Remove.
 */
void EditorWidget::remove()
{
    QVector<GroupItem *>  groupItems;
    QVector<ModuleItem *> moduleItems;
    for (QTreeWidgetItem *rawItem : m_treeEditor->selectedItems()) {
        {
            GroupItem *item = dynamic_cast<GroupItem *>(rawItem);

            if (item != nullptr) {
                groupItems.push_back(item);
                continue;
            }
        }

        {
            ModuleItem *item = dynamic_cast<ModuleItem *>(rawItem);
            if (item != nullptr) {
                moduleItems.push_back(item);
            }
        }
    }

    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create(groupItems, moduleItems, this);
    this->doOperation(operation);

    this->updateAddToStationStatus();
}

/**
 * @brief		Remove group item.
 */
void EditorWidget::removeGroupItem(GroupItem *item)
{
    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create({item}, {}, this);
    this->doOperation(operation);

    this->updateAddToStationStatus();
}

/**
 * @brief		Remove module item.
 */
void EditorWidget::removeModuleItem(ModuleItem *item)
{
    ::std::shared_ptr<Operation> operation
        = RemoveOperation::create({}, {item}, this);
    this->doOperation(operation);
}

/**
 * @brief		Save.
 */
void EditorWidget::save()
{
    if (m_save->path() == "") {
        this->saveAs();
    } else {
        if (m_save->write()) {
            m_savedUndoCount = m_undoStack.size();
            qDebug() << "File" << this->windowTitle() << "Saved.";
            this->updateSaveStatus();
        } else {
            QMessageBox::critical(this, STR("STR_ERROR"),
                                  STR("STR_ERR_SAVE").arg(m_save->path()));
        }
    }
}

/**
 * @brief		Save as.
 */
void EditorWidget::saveAs()
{
    // Get path to save.
    QString fileName = QFileDialog::getSaveFileName(
        this, STR("STR_TITLE_SAVE_STATION"),
        Config::instance()->getString(
            "/savePath",
            Config::instance()->getString("/openPath", QDir::homePath())),
        STR("STR_SAVE_FILE_FILTER"));

    if (fileName == "") {
        return;
    }

    QString dir = QDir(fileName).absolutePath();
    dir         = dir.left(dir.lastIndexOf("/"));
    Config::instance()->setString("/savePath", dir);

    QString oldPath = m_save->path();

    // Save file.
    if (m_save->write(fileName)) {
        m_savedUndoCount = m_undoStack.size();
        qDebug() << "File" << this->windowTitle() << "Saved.";
        this->updateTitle();
        this->updateSaveStatus();
        _opendFiles.remove(oldPath);
        _opendFiles[m_save->path()] = this;
    } else {
        QMessageBox::critical(this, STR("STR_ERROR"),
                              STR("STR_ERR_SAVE").arg(fileName));
    }
}

/**
 * @brief		Export as HTML.
 */
void EditorWidget::exportAsHTML()
{
    // Get path to save.
    QString fileName = QFileDialog::getSaveFileName(
        this, STR("STR_TITLE_EXPORT_HTML"),
        Config::instance()->getString(
            "/exportPath",
            Config::instance()->getString(
                "/savePath",
                Config::instance()->getString("/openPath", QDir::homePath()))),
        STR("STR_SAVE_HTML_FILTER"));

    if (fileName == "") {
        return;
    }

    QString dir = QDir(fileName).absolutePath();
    dir         = dir.left(dir.lastIndexOf("/"));
    Config::instance()->setString("/exportPath", dir);

    // Save file.
    if (m_save->writeHTML(fileName, this->windowTitle())) {
        m_savedUndoCount = m_undoStack.size();
        qDebug() << "File" << this->windowTitle() << "exported.";
        this->updateTitle();
        this->updateSaveStatus();
    } else {
        QMessageBox::critical(this, STR("STR_ERROR"),
                              STR("STR_ERR_EXPORT").arg(fileName));
    }
}

/**
 * @brief		Change language.
 */
void EditorWidget::onLanguageChanged()
{
    m_itemGroups->setText(0, STR("STR_STATION_MODULES"));

    m_itemSummary->setText(0, STR("STR_SUMMARY"));
    m_itemHull->setText(0, STR("STR_SUMMARY_HULL"));
    m_itemExplosionDamage->setText(0, STR("STR_SUMMARY_EXPLOSION_DAMAGE"));

    m_itemWeapons->setText(0, STR("STR_SUMMARY_WEAPONS"));
    m_itemWeaponSLaunchTube->setText(0, STR("STR_SUMMARY_S_LAUNCH_TUBE"));
    m_itemWeaponMLaunchTube->setText(0, STR("STR_SUMMARY_M_LAUNCH_TUBE"));
    m_itemWeaponMTurret->setText(0, STR("STR_SUMMARY_M_TURRET"));
    m_itemWeaponLTurret->setText(0, STR("STR_SUMMARY_L_TURRET"));

    m_itemShields->setText(0, STR("STR_SUMMARY_SHIELDS"));
    m_itemShieldMShield->setText(0, STR("STR_SUMMARY_M_SHIELD"));
    m_itemShieldLShield->setText(0, STR("STR_SUMMARY_L_SHIELD"));

    m_itemStorage->setText(0, STR("STR_SUMMARY_STORAGE"));
    m_itemStorageContainer->setText(0, STR("STR_SUMMARY_CARGO_TYPE_CONTAINER"));
    m_itemStorageSolid->setText(0, STR("STR_SUMMARY_CARGO_TYPE_SOLID"));
    m_itemStorageLiquid->setText(0, STR("STR_SUMMARY_CARGO_TYPE_LIQUID"));

    m_itemDockingbay->setText(0, STR("STR_SUMMARY_DOCKINGBAY"));
    m_itemDockingbaySDock->setText(0, STR("STR_SUMMARY_S_DOCK"));
    m_itemDockingbayMDock->setText(0, STR("STR_SUMMARY_M_DOCK"));
    m_itemDockingbayLDock->setText(0, STR("STR_SUMMARY_L_DOCK"));
    m_itemDockingbayXLDock->setText(0, STR("STR_SUMMARY_XL_DOCK"));
    m_itemDockingbayLXLDock->setText(0, STR("STR_SUMMARY_L_XL_DOCK"));

    m_itemShipStorage->setText(0, STR("STR_SUMMARY_SHIP_STORAGE"));
    m_itemShipStorageSShipCargo->setText(0, STR("STR_SUMMARY_S_SHIP_CAPACITY"));
    m_itemShipStorageMShipCargo->setText(0, STR("STR_SUMMARY_M_SHIP_CAPACITY"));

    m_itemWorkforce->setText(0, STR("STR_SUMMARY_WORKFORCE"));
    m_itemSurplusWorkforce->setText(0, STR("STR_SUMMARY_SURPLUS_WORKFORCE"));

    m_itemResources->setText(0, STR("STR_SUMMARY_RESOURCES"));
    m_itemResources->onLanguageChanged();
    m_itemIntermediates->setText(0, STR("STR_SUMMARY_INTERMEDIATES"));
    m_itemIntermediates->onLanguageChanged();
    m_itemProducts->setText(0, STR("STR_SUMMARY_PRODUCTS"));
    m_itemProducts->onLanguageChanged();
}

/**
 * @brief	Clear all warning informations.
 */
void EditorWidget::clearWarnings()
{
    while (! m_widgetsWarningInfos.empty()) {
        m_widgetsWarningInfos.back()->close();
        delete m_widgetsWarningInfos.back();
        m_widgetsWarningInfos.pop_back();
    }
}

/**
 * @brief		Show error information.
 */
void EditorWidget::addWarning(QString id)
{
    WarningWidget *warningWidget = new WarningWidget(
        ::std::shared_ptr<GenericString>(new LocaleString(id)));
    m_layout->insertWidget(0, warningWidget);
    m_widgetsWarningInfos.push_back(warningWidget);
}

/**
 * @brief		Called when item changed.
 */
void EditorWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(item);
    if (groupItem != nullptr && column == 0) {
        int index = m_itemGroups->indexOfChild(item);
        if (groupItem->text(0) != groupItem->group()->name()) {
            ::std::shared_ptr<Operation> operation
                = RenameGroupOperation::create(index,
                                               groupItem->group()->name(),
                                               groupItem->text(0), this);
            this->doOperation(operation);
            qDebug() << "Group name edited.";
        }
    }
}

/**
 * @brief		Called when item double clicked.
 */
void EditorWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // Module item.
    ModuleItem *moduleItem = dynamic_cast<ModuleItem *>(item);
    if (moduleItem != nullptr) {
        if (column != 0) {
            return;
        }
        m_infoWidget->showStationModuleInfo(moduleItem->module()->module());
        return;
    }

    // Ware item.
    WareItem *wareItem = dynamic_cast<WareItem *>(item);
    if (wareItem != nullptr) {
        m_infoWidget->showWareInfo(wareItem->ware());
        return;
    }
}

/**
 * @brief	    on amount changed.
 */
void EditorWidget::onChangeAmount(quint64     oldAmount,
                                  quint64     newAmount,
                                  ModuleItem *moduleItem)
{
    GroupItem *groupItem = dynamic_cast<GroupItem *>(moduleItem->parent());
    if (groupItem != nullptr) {
        ::std::shared_ptr<Operation> operation
            = ChangeModuleAmountOperation::create(
                m_itemGroups->indexOfChild(groupItem),
                groupItem->indexOfChild(moduleItem), oldAmount, newAmount,
                this);

        this->doOperation(operation);
    }
}

/**
 * @brief		Called when move up button of a group item clicked.
 */
void EditorWidget::onGroupMoveUp(GroupItem *item)
{
    qDebug() << "onGroupMoveUp() : " << item;

    int oldIndex = m_itemGroups->indexOfChild(item);
    Q_ASSERT(oldIndex > 0);
    int index = oldIndex - 1;

    ::std::shared_ptr<Operation> operation
        = MoveGroupOperation::create(oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move down button of a group item clicked.
 */
void EditorWidget::onGroupMoveDown(GroupItem *item)
{
    qDebug() << "onGroupMoveDown() : " << item;

    int oldIndex = m_itemGroups->indexOfChild(item);
    Q_ASSERT(oldIndex < m_itemGroups->childCount() - 1);
    int index = oldIndex + 1;

    ::std::shared_ptr<Operation> operation
        = MoveGroupOperation::create(oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move up button of a module item clicked.
 */
void EditorWidget::onModuleMoveUp(ModuleItem *item)
{
    qDebug() << "onModuleMoveUp() : " << item;

    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);

    int groupIndex = m_itemGroups->indexOfChild(groupItem);

    int oldIndex = groupItem->indexOfChild(item);
    Q_ASSERT(oldIndex > 0);
    int index = oldIndex - 1;

    ::std::shared_ptr<Operation> operation
        = MoveModuleOperation::create(groupIndex, oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when move up button of a module item clicked.
 */
void EditorWidget::onModuleMoveDown(ModuleItem *item)
{
    qDebug() << "onModuleMoveDown() : " << item;

    GroupItem *groupItem = dynamic_cast<GroupItem *>(item->parent());
    Q_ASSERT(groupItem != nullptr);

    int groupIndex = m_itemGroups->indexOfChild(groupItem);

    int oldIndex = groupItem->indexOfChild(item);
    Q_ASSERT(oldIndex < groupItem->childCount() - 1);
    int index = oldIndex + 1;

    ::std::shared_ptr<Operation> operation
        = MoveModuleOperation::create(groupIndex, oldIndex, index, this);

    this->doOperation(operation);
}

/**
 * @brief		Called when request a context menu.
 */
void EditorWidget::onCustomContextMenuRequested(const QPoint &)
{
    // Check item.
    QTreeWidgetItem *item = m_treeEditor->currentItem();
    if (item == nullptr) {
        return;
    }

    if (dynamic_cast<GroupItem *>(item) != nullptr
        || dynamic_cast<ModuleItem *>(item) != nullptr
        || item == m_itemGroups) {
        // Pop menu.
        QMenu menu;

        menu.addAction(m_editActions->actionEditNewGroup);
        menu.addSeparator();
        menu.addAction(m_editActions->actionEditUndo);
        menu.addAction(m_editActions->actionEditRedo);
        menu.addSeparator();
        menu.addAction(m_editActions->actionEditCut);
        menu.addAction(m_editActions->actionEditCopy);
        menu.addAction(m_editActions->actionEditPaste);
        menu.addSeparator();
        menu.addAction(m_editActions->actionEditRemove);

        menu.exec(QCursor::pos());
        return;
    }

    if (dynamic_cast<WareItem *>(item) != nullptr) {
        WareItem *wareItem = static_cast<WareItem *>(item);

        QMenu menu;

        QAction actFilterModuleByProduct(STR("STR_FILTER_MODULE_BY_PRODUCT"));
        menu.addAction(&actFilterModuleByProduct);
        this->connect(&actFilterModuleByProduct, &QAction::triggered,
                      [this, wareItem]() -> void {
                          emit this->filterByProduct(wareItem->ware());
                      });

        QAction actFilterModuleByResource(STR("STR_FILTER_MODULE_BY_RESOURCE"));
        menu.addAction(&actFilterModuleByResource);
        this->connect(&actFilterModuleByResource, &QAction::triggered,
                      [this, wareItem]() -> void {
                          emit this->filterByResource(wareItem->ware());
                      });

        menu.exec(QCursor::pos());
    }
}

/**
 * @brief       Active editor widget.
 */
void EditorWidget::active()
{
    // Connect actions.
    this->disconnect(m_stationModulesWidget,
                     &StationModulesWidget::addToStation, nullptr, nullptr);
    this->connect(m_stationModulesWidget, &StationModulesWidget::addToStation,
                  this, &EditorWidget::addModules);

    this->disconnect(m_fileActions->actionFileSave, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileSave, &QAction::triggered, this,
                  &EditorWidget::save);

    this->disconnect(m_fileActions->actionFileSaveAs, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileSaveAs, &QAction::triggered, this,
                  &EditorWidget::saveAs);

    this->disconnect(m_fileActions->actionFileExportAsHTML, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileExportAsHTML, &QAction::triggered,
                  this, &EditorWidget::exportAsHTML);

    this->disconnect(m_fileActions->actionFileClose, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_fileActions->actionFileClose, &QAction::triggered, this,
                  &EditorWidget::close);

    this->disconnect(m_editActions->actionEditNewGroup, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditNewGroup, &QAction::triggered, this,
                  &EditorWidget::newGroup);

    this->disconnect(m_editActions->actionEditUndo, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditUndo, &QAction::triggered, this,
                  &EditorWidget::undo);

    this->disconnect(m_editActions->actionEditRedo, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditRedo, &QAction::triggered, this,
                  &EditorWidget::redo);

    this->disconnect(m_editActions->actionEditCut, &QAction::triggered, nullptr,
                     nullptr);
    this->connect(m_editActions->actionEditCut, &QAction::triggered, this,
                  &EditorWidget::cut);

    this->disconnect(m_editActions->actionEditCopy, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditCopy, &QAction::triggered, this,
                  &EditorWidget::copy);

    this->disconnect(m_editActions->actionEditPaste, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditPaste, &QAction::triggered, this,
                  &EditorWidget::paste);

    this->disconnect(m_editActions->actionEditRemove, &QAction::triggered,
                     nullptr, nullptr);
    this->connect(m_editActions->actionEditRemove, &QAction::triggered, this,
                  &EditorWidget::remove);

    QClipboard *clipboard = QApplication::clipboard();
    this->disconnect(clipboard, &QClipboard::dataChanged, nullptr, nullptr);
    this->connect(clipboard, &QClipboard::dataChanged, this,
                  &EditorWidget::updatePasteStatus);

    // Update actions status.
    m_fileActions->actionFileSaveAs->setEnabled(true);
    m_fileActions->actionFileExportAsHTML->setEnabled(true);
    m_fileActions->actionFileClose->setEnabled(true);
    m_editActions->actionEditNewGroup->setEnabled(true);
    this->updateSaveStatus();
    this->updateAddToStationStatus();
    this->updateUndoRedoStatus();
    this->updateCutCopyRemoveStatus();
    this->updatePasteStatus();

    qDebug() << "Editor actived : " << this;
}
