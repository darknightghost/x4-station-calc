#include <common/compare.h>
#include <game_data/game_data.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_intermediate_widget/ware_button.h>

/**
 * @brief       Constructor.
 */
WareButton::WareButton(const QString &ware, Type type, QWidget *parent) :
    QPushButton(parent), m_ware(ware), m_type(type)
{
    switch (m_type) {
        case Type::Product:
            this->changeToStatusProduct();
            break;

        case Type::IntermediateOrResource:
            this->changeToStatusIntermediate();
            break;

        case Type::ResourceOnly:
            this->changeToStatusResource();
            break;
    }

    this->connect(this, &QPushButton::clicked, this, &WareButton::onClicked);
}

/**
 * @brief       Destructor.
 */
WareButton::~WareButton() {}

/**
 * @brief   Get ware.
 */
const QString &WareButton::ware() const
{
    return m_ware;
}

/**
 * @brief   Get type.
 */
WareButton::Type WareButton::type() const
{
    return m_type;
}

/**
 * @brief   Get status.
 */
WareButton::Status WareButton::status() const
{
    return m_status;
}

/**
 * @brief       Set status.
 */
void WareButton::setStatus(Status status)
{
    if (status != m_status) {
        switch (m_type) {
            case Type::Product:
                break;

            case Type::IntermediateOrResource:
                switch (status) {
                    case Status::Intermediate:
                        this->changeToStatusIntermediate();
                        emit this->changed(this);
                        break;

                    case Status::Resource:
                        this->changeToStatusResource();
                        emit this->changed(this);
                        break;

                    default:
                        break;
                }
                break;

            case Type::ResourceOnly:
                break;
        }
    }
}

/**
 * @brief   Change to status "Pruduct".
 */
void WareButton::changeToStatusProduct()
{
    m_status       = Status::Product;
    auto gameTexts = GameData::instance()->texts();
    auto gameWares = GameData::instance()->wares();
    this->setText(STR("STR_WARE_BUTTON_PRODUCT")
                      .arg(gameTexts->text(gameWares->ware(m_ware)->name)));
    this->setObjectName("Product");
}

/**
 * @brief   Change to status "Intermediate".
 */
void WareButton::changeToStatusIntermediate()
{
    m_status       = Status::Intermediate;
    auto gameTexts = GameData::instance()->texts();
    auto gameWares = GameData::instance()->wares();
    this->setText(STR("STR_WARE_BUTTON_INTERMEDIATE")
                      .arg(gameTexts->text(gameWares->ware(m_ware)->name)));
    this->setObjectName("Intermediate");
}

/**
 * @brief   Change to status "Resource".
 */
void WareButton::changeToStatusResource()
{
    m_status       = Status::Resource;
    auto gameTexts = GameData::instance()->texts();
    auto gameWares = GameData::instance()->wares();
    this->setText(STR("STR_WARE_BUTTON_RESOURCE")
                      .arg(gameTexts->text(gameWares->ware(m_ware)->name)));
    this->setObjectName("Resource");
}

/**
 * @brief     On cliicked.
 */
void WareButton::onClicked()
{
    if (m_type == Type::IntermediateOrResource) {
        switch (m_status) {
            case Status::Intermediate:
                this->changeToStatusResource();
                emit this->changed(this);
                break;

            case Status::Resource:
                this->changeToStatusIntermediate();
                emit this->changed(this);
                break;

            default:
                break;
        }
    }
}
