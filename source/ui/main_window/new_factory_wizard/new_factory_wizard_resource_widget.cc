#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_resource_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizardResourceWidget::NewFactoryWizardResourceWidget(
    NewFactoryWizard *                                    wizard,
    const QMap<QString, NewFactoryWizard::ProductInfo> &  selectedProducts,
    const QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
    QSet<QString> &                                       resources) :
    NewFactoryWizardCentralWidget(wizard),
    m_resources(resources)
{
    this->setWindowTitle(STR("STR_STEP_SET_RESOURCE"));

    m_rootNode = ::std::shared_ptr<ProductTreeNode>(
        new ProductTreeNode({{},
                             {},
                             "",
                             ProductTreeNode::NodeType::Root,
                             ProductTreeNode::Status::Root,
                             0,
                             nullptr}));

    /**
    for (auto &info : selectedProducts) {
        ::std::shared_ptr<ProductTreeNode> node(
            new ProductTreeNode({{m_rootNode},
                                 {},
                                 info.ware,
                                 ProductTreeNode::NodeType::Product,
                                 ProductTreeNode::Status::Product,
                                 1,
                                 nullptr}));
        m_rootNode->nextNodes.insert(node);
        m_productNodes[info.ware] = node;
    }

    auto wares         = GameData::instance()->wares();
    auto workunit_busy = wares->ware("workunit_busy");
    for (auto &info : workforceInfo) {
        if (info.percentage > 0) {
            m_products.insert(workunit_busy->productionInfos[info.race]->id);
        }
    }
*/
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardResourceWidget::~NewFactoryWizardResourceWidget() {}

/**
 * @brief       Insert node.
 */
void NewFactoryWizardResourceWidget::insertNode(
    const QString &ware, ::std::shared_ptr<ProductTreeNode> parent, int layer)
{
    ::std::shared_ptr<ProductTreeNode> node = nullptr;
    auto                               iter = m_productNodes.find(ware);
}

/**
 * @brief       Paint event.
 */
void NewFactoryWizardResourceWidget::paintEvent(QPaintEvent *event)
{
    NewFactoryWizardCentralWidget::paintEvent(event);
}
