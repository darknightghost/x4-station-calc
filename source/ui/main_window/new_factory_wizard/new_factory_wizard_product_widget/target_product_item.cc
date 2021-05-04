#include <QtCore/QDebug>

#include <common/compare.h>
#include <game_data/game_data.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget/target_product_item.h>

/**
 * @brief       Constructor.
 */
TargetProductItem::TargetProductItem(const QString &ware) : m_ware(ware)
{
    auto gameData = GameData::instance();
    auto texts    = gameData->texts();
    auto wares    = gameData->wares();

    this->setText(0, texts->text(wares->ware(m_ware)->name));
}

/**
 * @brief   Get ware ID.
 */
const QString &TargetProductItem::ware() const
{
    return m_ware;
}

/**
 * @brief       Destructor.
 */
TargetProductItem::~TargetProductItem() {}

/**
 * @brief       Constructor.
 */
ProductionValidator::ProductionValidator(QObject *parent) :
    QIntValidator(1, 2147483647, parent)
{}

/**
 * @brief       Destructor.
 */
ProductionValidator::~ProductionValidator() {}

/**
 * @brief           Fix up.
 */
void ProductionValidator::fixup(QString &input) const
{
    input = "1";
}

/**
 * @brief       Constructor.
 */
TargetProductWidget::TargetProductWidget(TargetProductItem *item,
                                         quint64            production) :
    QWidget(),
    m_item(item), m_production(max(production, (quint64)1))
{
    m_layout = new QHBoxLayout();
    this->setLayout(m_layout);

    m_txtProduction = new QLineEdit(QString("%1").arg(m_production));
    m_layout->addWidget(m_txtProduction);

    m_lblUnit = new QLabel("/h");
    m_layout->addWidget(m_lblUnit);

    m_validator = new ProductionValidator(this);
    m_txtProduction->setValidator(m_validator);

    this->connect(m_txtProduction, &QLineEdit::editingFinished, this,
                  &TargetProductWidget::onEditingFinished);
}

/**
 * @brief       Destructor.
 */
TargetProductWidget::~TargetProductWidget() {}

/**
 * @brief       Called when editing finished.
 */
void TargetProductWidget::onEditingFinished()
{
    quint64 value = static_cast<quint64>(m_txtProduction->text().toULongLong());
    if (value != m_production) {
        m_production = value;
        emit this->productionChanged(m_item, m_production);
    }
}
