#pragma once

#include <QtCore/QMap>
#include <QtCore/QVector>

#include <interfaces/i_create_factory_func.h>
#include <interfaces/i_load_factory_func.h>
#include <save/save_module.h>

/**
 * @brief	Information of module group in a save file.
 */
class SaveGroup :
    virtual public ICreateFactoryFunc<SaveGroup()>,
    virtual public ILoadFactoryFunc<SaveGroup(QJsonObject &,
                                              QVersionNumber &)> {
    CREATE_FUNC(SaveGroup);
    LOAD_FUNC(SaveGroup, QJsonObject &, QVersionNumber &);

  protected:
    QVector<::std::shared_ptr<SaveModule>> m_modules; ///< Modules in the group.
    QMap<::std::shared_ptr<SaveModule>, int>
        m_modulesIndex; ///< Index of modules.
    QMap<QString, ::std::shared_ptr<SaveModule>>
        m_modulesMacroIndex; ///< Macro index of modules.

  protected:
    /**
     * @brief		Create a module group information.
     */
    SaveGroup();

    /**
     * @brief		Load a module group information.
     *
     * @param[in]	entry		Entery of the module.
     * @param[in]	version		Version of the input save file.
     */
    SaveGroup(QJsonObject &entry, QVersionNumber &version);

  public:
    /**
     * @brief		Get modules.
     *
     * @return		List of modules.
     */
    const QVector<::std::shared_ptr<SaveModule>> &modules() const;

    /**
     * @brief		Set module index.
     *
     * @param[in]	module	Module.
     * @param[in]	index	Index in the group, if index = -1, the module will
     *						be move to the end of the group.
     */
    void setIndex(::std::shared_ptr<SaveModule> module, int index);

    /**
     * @brief		Add module.
     *
     * @param[in]	macro	Module macro.
     * @param[in]	count	Count of the module.
     *
     * @return		New module information. If failed, \c nullptr is returned.
     */
    ::std::shared_ptr<SaveModule> addModule(const QString &macro,
                                            quint64        count);

    /**
     * @brief		Remove module.
     *
     * @param[in]	macro		Module macro.
     */
    void removeModule(const QString &macro);

    /**
     * @brief		Parse to json object.
     *
     * @return		Json object.
     */
    QJsonObject toJson();

    /**
     * @brief		Destructor.
     */
    virtual ~SaveGroup();
};
