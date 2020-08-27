#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <interfaces/i_create_factory_func.h>
#include <interfaces/i_load_factory_func.h>
#include <save/save_version.h>

/**
 * @brief		Module information in a save file.
 */
class SaveModule :
    virtual public ICreateFactoryFunc<SaveModule, const QString &>,
    virtual public ILoadFactoryFunc<SaveModule,
                                    QJsonObject &,
                                    const SaveVersion &> {
    CREATE_FUNC(SaveModule, const QString &);
    LOAD_FUNC(SaveModule, QJsonObject &, const SaveVersion &);

  protected:
    QString                       m_module;    ///< Module macro.
    quint64                       m_amount;    ///< Amount.
    static QMap<QString, QString> _idMacroMap; ///< ID to macro map.

  protected:
    /**
     * @brief		Create a module information.
     *
     * @param[in]	module		Module macro.
     */
    SaveModule(const QString &module);

    /**
     * @brief		Load a module information.
     *
     * @param[in]	entry		Entery of the module.
     * @param[in]	version		Version of the input save file.
     */
    SaveModule(QJsonObject &entry, const SaveVersion &version);

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

  private:
    /**
     * @brief	Load id map of old version.
     */
    void load0_x_x();
};
