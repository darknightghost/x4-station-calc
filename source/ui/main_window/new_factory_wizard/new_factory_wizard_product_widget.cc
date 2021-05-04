#include <QtWidgets/QHeaderView>

#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/locale/q_tree_widget_item_locale.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/product_to_add_item.h>

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

    m_layoutBtn = new QVBoxLayout();
    m_layout->addLayout(m_layoutBtn, 0, 1, 2, 1);

    m_layoutBtn->addStretch();

    m_btnAdd = new QPushButton();
    m_layoutBtn->addWidget(m_btnAdd);
    m_btnAdd->setIcon(QIcon(":/Icons/Right.png"));

    m_btnRemove = new QPushButton();
    m_layoutBtn->addWidget(m_btnRemove);
    m_btnRemove->setIcon(QIcon(":/Icons/Left.png"));

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

    // Add items.
    auto wares = GameData::instance()->wares();
    auto texts = GameData::instance()->texts();
    for (auto &p : products) {
        if (selectedProducts.find(p) == selectedProducts.end()) {
            QTreeWidgetItem *item = new ProductToAddItem(p);
            m_treeProductToAdd->addTopLevelItem(item);

        } else {
        }
    }

    m_treeProductToAdd->sortItems(0, Qt::SortOrder::AscendingOrder);
    m_treeTargetProducts->sortItems(0, Qt::SortOrder::AscendingOrder);
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardProductWidget::~NewFactoryWizardProductWidget() {}
