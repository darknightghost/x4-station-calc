#pragma once

#include <QtCore/QMetaType>
#include <QtGui/QIntValidator>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

#include <ui/locale/q_tree_widget_item_locale.h>

/**
 * @brief   Target product.
 */
class TargetProductItem : public QTreeWidgetItemLocale {
  private:
    QString m_ware; ///< Ware ID.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ware        Ware.
     */
    TargetProductItem(const QString &ware);

    /**
     * @brief   Get ware ID.
     *
     * @return  Ware ID.
     */
    const QString &ware() const;

    /**
     * @brief       Destructor.
     */
    virtual ~TargetProductItem();
};

Q_DECLARE_METATYPE(TargetProductItem *);

/**
 * @brief   Production validator.
 */
class ProductionValidator : public QIntValidator {
    Q_OBJECT
  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent objuect.
     */
    ProductionValidator(QObject *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~ProductionValidator();

  public:
    /**
     * @brief           Fix up.
     *
     * @param[in, out]  input       Input.
     */
    virtual void fixup(QString &input) const override;
};

/**
 * @brief   Target product widget.
 */
class TargetProductWidget : public QWidget {
    Q_OBJECT

  private:
    TargetProductItem *m_item;       ///< Item.
    quint64            m_production; ///< Production.

    QHBoxLayout *        m_layout;        ///< Layout.
    ProductionValidator *m_validator;     ///< Validator.
    QLineEdit *          m_txtProduction; ///< Amount.
    QLabel *             m_lblUnit;       ///< Label unit.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   item        Item.
     * @param[in]   production  Production.
     */
    TargetProductWidget(TargetProductItem *item, quint64 production);

    /**
     * @brief       Destructor.
     */
    virtual ~TargetProductWidget();

  signals:
    /**
     * @brief       Emitted when the production changed.
     *
     * @param[in]   item        Item.
     * @param[in]   production  New production.
     */
    void productionChanged(TargetProductItem *item, quint64 production);

  private slots:
    /**
     * @brief       Called when editing finished.
     */
    void onEditingFinished();
};
