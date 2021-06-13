#pragma once

#include <QtGui/QPaintEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
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
        QSet<::std::weak_ptr<ProductTreeNode>> prevNodes; ///< Previous nodes.
        QSet<::std::weak_ptr<ProductTreeNode>> nextNodes; ///< Next nodes.

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

  private:
    QVBoxLayout *m_layout; ///< Layout.

    QMap<QString, ::std::shared_ptr<ProductTreeNode>>
                                       m_productNodes; ///< Product nodes.
    ::std::shared_ptr<ProductTreeNode> m_rootNode;     ///< Root node.
    QSet<QString> &                    m_resources;    ///< Resources.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   wizard              Wizard widget.
     * @param[in]   selectedProducts    Products selected.
     * @param[in]   workforceInfo       Workforce information.
     * @param[out]  resources           Resources.
     */
    NewFactoryWizardResourceWidget(
        NewFactoryWizard *                                    wizard,
        const QMap<QString, NewFactoryWizard::ProductInfo> &  selectedProducts,
        const QMap<QString, NewFactoryWizard::WorkforceInfo> &workforceInfo,
        QSet<QString> &                                       resources);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardResourceWidget();

  private:
    /**
     * @brief       Insert node.
     *
     * @param[in]   ware        Ware to insert.
     * @param[in]   parent      Parent node.
     * @param[in]   layer       Layer.
     */
    void insertNode(const QString &                    ware,
                    ::std::shared_ptr<ProductTreeNode> parent,
                    int                                layer);

    /**
     * @brief       Paint event.
     *
     * @param[in]   event   Event.
     */
    virtual void paintEvent(QPaintEvent *event) override;
};
