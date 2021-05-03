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
};
