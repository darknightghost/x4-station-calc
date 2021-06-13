#pragma once

#include <QtWidgets/QPushButton>

/**
 * @brief   Intermediate button.
 */
class WareButton : public QPushButton {
    Q_OBJECT

  public:
    /**
     * @brief   Status.
     */
    enum class Status {
        Product,      ///< Product.
        Intermediate, ///< Intermediate.
        Resource,     ///< Resource.
    };

    /**
     * @brief   Type.
     */
    enum class Type {
        Workforce,              ///< Workforce.
        Product,                ///< Product.
        IntermediateOrResource, ///< Intermediate or resource.
        ResourceOnly,           ///< Resource only.
    };

  private:
    QString m_ware;   ///< Ware.
    Type    m_type;   ///< Type.
    Status  m_status; ///< Status.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ware        Ware.
     * @param[in]   type        Type.
     * @param[in]   parent      Parent widget.
     */
    WareButton(const QString &ware, Type type, QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~WareButton();

  public:
    /**
     * @brief   Get ware.
     *
     * @return  Ware ID.
     */
    const QString &ware() const;

    /**
     * @brief   Get type.
     *
     * @return  Type.
     */
    Type type() const;

    /**
     * @brief   Get status.
     *
     * @return  Status.
     */
    Status status() const;

  public slots:
    /**
     * @brief       Set status.
     *
     * @param[in]   status      New status.
     */
    void setStatus(Status status);

  signals:
    /**
     * @brief       Emitted when status has changed.
     *
     * @param[in]   button       Button changed.
     */
    void changed(WareButton *button);

  private:
    using QPushButton::clicked;

    /**
     * @brief   Change to status "Pruduct".
     */
    void changeToStatusProduct();

    /**
     * @brief   Change to status "Intermediate".
     */
    void changeToStatusIntermediate();

    /**
     * @brief   Change to status "Resource".
     */
    void changeToStatusResource();

  private slots:
    /**
     * @brief     On cliicked.
     */
    void onClicked();
};
