#include <QtCore/QMetaType>
#include <QtWidgets/QHeaderView>

#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/locale/q_tree_widget_item_locale.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/product_to_add_item.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/target_product_item.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardProductWidget::NewFactoryWizardProductWidget(
    NewFactoryWizard *                            wizard,
    const QSet<QString> &                         products,
    QMap<QString, NewFactoryWizard::ProductInfo> &selectedProducts) :
    NewFactoryWizardCentralWidget(wizard),
    m_selectedProducts(selectedProducts)
{
    qRegisterMetaType<TargetProductItem *>("TargetProductItem*");

    this->setWindowTitle(STR("STR_STEP_SELECT_PRODUCTS"));

    m_layout = new QGridLayout();
    this->setLayout(m_layout);

    m_lblSelectable = new QLabel(STR("STR_SELECTABLE_PRODUCTS"));
    m_layout->addWidget(m_lblSelectable, 0, 0);

    m_treeProductToAdd = new QTreeWidget();
    m_layout->addWidget(m_treeProductToAdd, 1, 0);
    m_treeProductToAdd->header()->setVisible(true);
    m_treeProductToAdd->setHeaderLabels({STR("STR_HEAD_PRODUCTS")});
    m_treeProductToAdd->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);
    m_treeProductToAdd->setSortingEnabled(true);
    this->connect(
        m_treeProductToAdd, &QTreeWidget::itemSelectionChanged, this,
        &NewFactoryWizardProductWidget::onTreeProductToAddSelectionChange);

    m_layoutBtn = new QVBoxLayout();
    m_layout->addLayout(m_layoutBtn, 0, 1, 2, 1);

    m_layoutBtn->addStretch();

    m_btnAdd = new QPushButton();
    m_layoutBtn->addWidget(m_btnAdd);
    m_btnAdd->setIcon(QIcon(":/Icons/Right.png"));
    this->connect(m_btnAdd, &QPushButton::clicked, this,
                  &NewFactoryWizardProductWidget::onBtnAddClicked);

    m_btnRemove = new QPushButton();
    m_layoutBtn->addWidget(m_btnRemove);
    m_btnRemove->setIcon(QIcon(":/Icons/Left.png"));
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &NewFactoryWizardProductWidget::onBtnRemoveClicked);

    m_layoutBtn->addStretch();

    m_lblSelected = new QLabel(STR("STR_SELECTED_PRODUCTS"));
    m_layout->addWidget(m_lblSelected, 0, 2);

    m_treeTargetProducts = new QTreeWidget();
    m_layout->addWidget(m_treeTargetProducts, 1, 2);
    m_treeTargetProducts->header()->setVisible(true);
    m_treeTargetProducts->setHeaderLabels(
        {STR("STR_HEAD_PRODUCTS"), STR("STR_HEAD_MINIMUM_PRODUCTION")});
    m_treeTargetProducts->setSelectionMode(
        QAbstractItemView::SelectionMode::ExtendedSelection);
    m_treeTargetProducts->setSortingEnabled(true);
    this->connect(
        m_treeTargetProducts, &QTreeWidget::itemSelectionChanged, this,
        &NewFactoryWizardProductWidget::onTreeTargetProductsSelectionChange);

    // Add items.
    auto wares = GameData::instance()->wares();
    auto texts = GameData::instance()->texts();
    for (auto &p : products) {
        auto iter = selectedProducts.find(p);
        if (iter == selectedProducts.end()) {
            QTreeWidgetItem *item = new ProductToAddItem(p);
            m_treeProductToAdd->addTopLevelItem(item);

        } else {
            TargetProductItem *  item = new TargetProductItem(p);
            TargetProductWidget *widget
                = new TargetProductWidget(item, iter->production);
            m_treeTargetProducts->addTopLevelItem(item);
            m_treeTargetProducts->setItemWidget(item, 1, widget);
            this->connect(widget, &TargetProductWidget::productionChanged, this,
                          &NewFactoryWizardProductWidget::onProductionChanged);
        }
    }

    m_treeProductToAdd->sortItems(0, Qt::SortOrder::AscendingOrder);
    m_treeTargetProducts->sortItems(0, Qt::SortOrder::AscendingOrder);

    this->onTreeTargetProductsSelectionChange();
    this->onTreeProductToAddSelectionChange();
    this->updateNextBtnStatus();
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardProductWidget::~NewFactoryWizardProductWidget() {}

/**
 * @brief       Update next button status.
 */
void NewFactoryWizardProductWidget::updateNextBtnStatus()
{
    this->wizard()->setNextBtnEnabled(m_treeTargetProducts->topLevelItemCount()
                                      != 0);
}

/**
 * @brief       On production changed.
 */
void NewFactoryWizardProductWidget::onProductionChanged(TargetProductItem *item,
                                                        quint64 production)
{
    m_selectedProducts[item->ware()].production = production;
}

/**
 * @brief       On button "Add" clicked.
 */
void NewFactoryWizardProductWidget::onBtnAddClicked()
{
    QList<QTreeWidgetItem *> selected = m_treeProductToAdd->selectedItems();

    for (auto item : selected) {
        // Add item.
        auto productItem = dynamic_cast<ProductToAddItem *>(item);
        Q_ASSERT(productItem != nullptr);
        m_selectedProducts[productItem->ware()] = {productItem->ware(), 1};

        TargetProductItem *targetItem
            = new TargetProductItem(productItem->ware());
        TargetProductWidget *targetWidget
            = new TargetProductWidget(targetItem, 1);
        m_treeTargetProducts->addTopLevelItem(targetItem);
        m_treeTargetProducts->setItemWidget(targetItem, 1, targetWidget);
        this->connect(targetWidget, &TargetProductWidget::productionChanged,
                      this,
                      &NewFactoryWizardProductWidget::onProductionChanged);

        // Delete item.
        delete m_treeProductToAdd->takeTopLevelItem(
            m_treeProductToAdd->indexOfTopLevelItem(item));
    }

    m_treeTargetProducts->sortItems(0, Qt::SortOrder::AscendingOrder);
    this->updateNextBtnStatus();
}

/**
 * @brief       On button "Remove" clicked.
 */
void NewFactoryWizardProductWidget::onBtnRemoveClicked()
{
    QList<QTreeWidgetItem *> selected = m_treeTargetProducts->selectedItems();

    for (auto item : selected) {
        // Add item.
        auto targetItem = dynamic_cast<TargetProductItem *>(item);
        Q_ASSERT(targetItem != nullptr);
        auto productItem = new ProductToAddItem(targetItem->ware());
        m_treeProductToAdd->addTopLevelItem(productItem);

        // Delete item.
        m_selectedProducts.remove(targetItem->ware());
        m_treeTargetProducts->removeItemWidget(item, 1);
        delete m_treeTargetProducts->takeTopLevelItem(
            m_treeTargetProducts->indexOfTopLevelItem(item));
    }

    m_treeProductToAdd->sortItems(0, Qt::SortOrder::AscendingOrder);
    this->updateNextBtnStatus();
}

/**
 * @brief       On tree products to add selection changed.
 */
void NewFactoryWizardProductWidget::onTreeProductToAddSelectionChange()
{
    if (m_treeProductToAdd->selectedItems().count() > 0) {
        m_btnAdd->setEnabled(true);

    } else {
        m_btnAdd->setEnabled(false);
    }
}

/**
 * @brief       On tree target products selection changed.
 */
void NewFactoryWizardProductWidget::onTreeTargetProductsSelectionChange()
{
    if (m_treeTargetProducts->selectedItems().count() > 0) {
        m_btnRemove->setEnabled(true);

    } else {
        m_btnRemove->setEnabled(false);
    }
}
