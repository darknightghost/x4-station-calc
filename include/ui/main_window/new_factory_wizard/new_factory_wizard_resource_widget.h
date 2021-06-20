#pragma once

#include <QtGui/QPaintEvent>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

#include <save/save.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_resource_widget/ware_button.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardResourceWidget : public NewFactoryWizardCentralWidget {
    Q_OBJECT

  private:
    /**
     * @brief       Node of product tree.
     */
    struct ProductTreeNode {
        QSet<ProductTreeNode *> prevNodes; ///< Previous nodes.
        QSet<ProductTreeNode *> nextNodes; ///< Next nodes.

        QString ware; ///< Ware.
        enum class NodeType {
            Root,                   ///< Root node.
            Workforce,              ///< Workforce.
            Product,                ///< Product.
            IntermediateOrResource, ///< Intermediate or resource.
            Resouces                ///< Resource.
        } type;
        enum class Status {
            Root,         ///< Root node.
            Workforce,    ///< Workforce.
            Product,      ///< Product.
            Intermediate, ///< Intermediate or resource.
            Resouces      ///< Resource.
        } status;
        int         layer;  ///< Layer.
        WareButton *button; ///< Button.
    };

    /**
     * @brief       Button row.
     */
    struct ButtonRow {
        int index; ///< Index.
    };

    /**
     * @brief       Button row.
     */
    struct ButtonInfo {
        int index; ///< Index.
    };

  private:
    QMap<QString, ::std::shared_ptr<ProductTreeNode>>
                                       m_productNodes; ///< Product nodes.
    ::std::shared_ptr<ProductTreeNode> m_rootNode;     ///< Root node.
    QSet<QString> &                    m_resources;    ///< Resources.

    QGridLayout *m_layout; ///< Layout.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   wizard                  Wizard widget.
     * @param[in]   selectedProducts        Products selected.
     * @param[in]   workforceInfo           Workforce information.
     * @param[in]   orderOfProductionMethod Order of production method.
     * @param[out]  resources               Resources.
     */
    NewFactoryWizardResourceWidget(
        NewFactoryWizard *                                    wizard,
        const QMap<QString, NewFactoryWizard::ProductInfo> &  selectedProducts,
        const QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
        const QVector<QString> &orderOfProductionMethod,
        QSet<QString> &         resources);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardResourceWidget();

  private:
    /**
     * @brief       Create buttons.
     */
    void createButtons();

    /**
     * @brief       Insert ware node and resources.
     *
     * @param[in]   ware                    Ware to insert.
     * @param[in]   parent                  Parent node.
     * @param[in]   nodeType                Node type.
     * @param[in]   layer                   Layer.
     * @param[in]   orderOfProductionMethod Order of production method.
     */
    void insertWare(const QString &                    ware,
                    ::std::shared_ptr<ProductTreeNode> parent,
                    ProductTreeNode::NodeType          nodeType,
                    int                                layer,
                    const QVector<QString> &           orderOfProductionMethod);

    /**
     * @brief       Insert node.
     *
     * @param[in]   ware        Ware to insert.
     * @param[in]   parent      Parent node.
     * @param[in]   nodeType    Node type.
     * @param[in]   layer       Layer.
     *
     * @return      Node.
     */
    ::std::shared_ptr<NewFactoryWizardResourceWidget::ProductTreeNode>
        insertNode(const QString &                    ware,
                   ::std::shared_ptr<ProductTreeNode> parent,
                   ProductTreeNode::NodeType          nodeType,
                   int                                layer);

    /**
     * @brief       Paint event.
     *
     * @param[in]   event   Event.
     */
    virtual void paintEvent(QPaintEvent *event) override;
};
