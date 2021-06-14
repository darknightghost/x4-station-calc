#include <common/compare.h>
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
    const QVector<QString> &orderOfProductionMethod,
    QSet<QString> &         resources) :
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

    // Workforce.
    ::std::shared_ptr<ProductTreeNode> workforceNode(
        new ProductTreeNode({{m_rootNode.get()},
                             {},
                             "workunit_busy",
                             ProductTreeNode::NodeType::Workforce,
                             ProductTreeNode::Status::Workforce,
                             1,
                             nullptr}));

    m_productNodes["workunit_busy"] = workforceNode;
    m_rootNode->nextNodes.insert(workforceNode.get());

    auto wares         = GameData::instance()->wares();
    auto workunit_busy = wares->ware("workunit_busy");
    for (auto &info : workforceInfo) {
        if (info.percentage > 0) {
            auto iter = workunit_busy->productionInfos.find(info.race);
            if (iter == workunit_busy->productionInfos.end()) {
                iter = workunit_busy->productionInfos.find("default");
            }
            Q_ASSERT(iter != workunit_busy->productionInfos.end());

            for (auto &resouce : (*iter)->resources) {
                this->insertWare(
                    resouce->id, workforceNode,
                    ProductTreeNode::NodeType::IntermediateOrResource, 2,
                    orderOfProductionMethod);
            }
        }
    }

    // Products.
    for (auto &info : selectedProducts) {
        this->insertWare(info.ware, m_rootNode,
                         ProductTreeNode::NodeType::Product, 1,
                         orderOfProductionMethod);
    }
}

/**
 * @brief       Destructor.
 */
NewFactoryWizardResourceWidget::~NewFactoryWizardResourceWidget() {}

/**
 * @brief       Insert ware node and resources.
 */
void NewFactoryWizardResourceWidget::insertWare(
    const QString &                    ware,
    ::std::shared_ptr<ProductTreeNode> parent,
    ProductTreeNode::NodeType          nodeType,
    int                                layer,
    const QVector<QString> &           orderOfProductionMethod)
{
    // Insert node.
    auto wares    = GameData::instance()->wares();
    auto wareInfo = wares->ware(ware);

    if (nodeType == ProductTreeNode::NodeType::IntermediateOrResource
        && wareInfo->productionInfos.empty()) {
        nodeType = ProductTreeNode::NodeType::Resouces;
    }

    auto node = this->insertNode(ware, parent, nodeType, layer);

    // Insert resources.
    if (! wareInfo->productionInfos.empty()) {
        auto iter = wareInfo->productionInfos.find(orderOfProductionMethod[0]);
        if (iter == wareInfo->productionInfos.end()) {
            iter = wareInfo->productionInfos.find("default");
        }

        for (auto methodIter = orderOfProductionMethod.begin() + 1;
             iter == wareInfo->productionInfos.end()
             && methodIter != orderOfProductionMethod.end();
             ++methodIter) {
            iter = wareInfo->productionInfos.find(*methodIter);
        }
        Q_ASSERT(iter != wareInfo->productionInfos.end());

        for (auto &resouce : (*iter)->resources) {
            this->insertWare(resouce->id, node,
                             ProductTreeNode::NodeType::IntermediateOrResource,
                             layer + 1, orderOfProductionMethod);
        }
    }
}

/**
 * @brief       Insert node.
 */
::std::shared_ptr<NewFactoryWizardResourceWidget::ProductTreeNode>
    NewFactoryWizardResourceWidget::insertNode(
        const QString &                    ware,
        ::std::shared_ptr<ProductTreeNode> parent,
        ProductTreeNode::NodeType          nodeType,
        int                                layer)
{
    ::std::shared_ptr<ProductTreeNode> node = nullptr;
    auto                               iter = m_productNodes.find(ware);
    if (iter != m_productNodes.end()) {
        node = *iter;
        if (nodeType == ProductTreeNode::NodeType::Product) {
            node->type   = ProductTreeNode::NodeType::Product;
            node->status = ProductTreeNode::Status::Product;
        }

    } else {
        ProductTreeNode::Status status;
        switch (nodeType) {
            case ProductTreeNode::NodeType::Product:
                status = ProductTreeNode::Status::Product;
                break;

            case ProductTreeNode::NodeType::IntermediateOrResource:
                status = ProductTreeNode::Status::Intermediate;
                break;

            case ProductTreeNode::NodeType::Resouces:
                status = ProductTreeNode::Status::Resouces;
                break;

            default:
                Q_ASSERT(false);
                status = ProductTreeNode::Status::Product;
        }

        node = ::std::shared_ptr<ProductTreeNode>(new ProductTreeNode(
            {{}, {}, ware, nodeType, status, layer, nullptr}));
        m_productNodes[ware] = node;
    }

    parent->nextNodes.insert(node.get());
    node->prevNodes.insert(parent.get());
    node->layer = max(layer, node->layer);

    return node;
}

/**
 * @brief       Paint event.
 */
void NewFactoryWizardResourceWidget::paintEvent(QPaintEvent *event)
{
    NewFactoryWizardCentralWidget::paintEvent(event);
}
