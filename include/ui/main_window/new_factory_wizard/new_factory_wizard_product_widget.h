#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <save/save.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_central_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/target_product_item.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardProductWidget : public NewFactoryWizardCentralWidget {
    Q_OBJECT

  private:
    QGridLayout *m_layout; ///< Layout.

    QLabel *     m_lblSelectable;      ///< Label selectable products.
    QLabel *     m_lblSelected;        ///< Label selected products.
    QTreeWidget *m_treeProductToAdd;   ///< Products to add.
    QTreeWidget *m_treeTargetProducts; ///< Target products.

    QVBoxLayout *m_layoutBtn; ///< Button layout.
    QPushButton *m_btnAdd;    ///< Button "Add".
    QPushButton *m_btnRemove; ///< Button "Remove".

    QMap<QString, NewFactoryWizard::ProductInfo>
        &m_selectedProducts; ///< Selected products.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   wizard              Wizard widget.
     * @param[in]   products            Products.
     * @param[out]  selectedProducts    Products selected.
     */
    NewFactoryWizardProductWidget(
        NewFactoryWizard *                            wizard,
        const QSet<QString> &                         products,
        QMap<QString, NewFactoryWizard::ProductInfo> &selectedProducts);

    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizardProductWidget();

  private:
    /**
     * @brief       Update next button status.
     */
    void updateNextBtnStatus();

  private slots:
    /**
     * @brief       On production changed.
     *
     * @param[in]   item        Item.
     * @param[in]   production  New production.
     */
    void onProductionChanged(TargetProductItem *item, quint64 production);

    /**
     * @brief       On button "Add" clicked.
     */
    void onBtnAddClicked();

    /**
     * @brief       On button "Remove" clicked.
     */
    void onBtnRemoveClicked();

    /**
     * @brief       On tree products to add selection changed.
     */
    void onTreeProductToAddSelectionChange();

    /**
     * @brief       On tree target products selection changed.
     */
    void onTreeTargetProductsSelectionChange();
};
