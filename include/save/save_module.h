#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QVersionNumber>

#include <interfaces/i_create_factory_func.h>
#include <interfaces/i_load_factory_func.h>

/**
 * @brief		Module information in a save file.
 */
class SaveModule :
    virtual public ICreateFactoryFunc<SaveModule(const QString &)>,
    virtual public ILoadFactoryFunc<SaveModule(QJsonObject &,
                                               QVersionNumber &)> {
    CREATE_FUNC(SaveModule, const QString &);
    LOAD_FUNC(SaveModule, QJsonObject &, QVersionNumber &);

  protected:
    QString m_module; ///< Module ID.
    quint64 m_amount; ///< Amount.

  protected:
    /**
     * @brief		Create a module information.
     *
     * @param[in]	module		Module ID.
     */
    SaveModule(const QString &module);

    /**
     * @brief		Load a module information.
     *
     * @param[in]	entry		Entery of the module.
     * @param[in]	version		Version of the input save file.
     */
    SaveModule(QJsonObject &entry, QVersionNumber &version);

  public:
    /**
     * @brief		Get module ID.
     *
     * @return		ID of the station module.
     */
    const QString &module() const;

    /**
     * @brief		Get amount.
     *
     * @return		Amount.
     */
    quint64 amount() const;

    /**
     * @brief		Set amount.
     *
     * @param[in]	amount		Amount.
     */
    void setAmount(quint64 amount);

    /**
     * @brief		Parse to json object.
     *
     * @return		Json object.
     */
    QJsonObject toJson();

    /**
     * @brief		Destructor.
     */
    virtual ~SaveModule();
};
