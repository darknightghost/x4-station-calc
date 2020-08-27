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
    virtual public ICreateFactoryFunc<SaveGroup>,
    virtual public ILoadFactoryFunc<SaveGroup,
                                    QJsonObject &,
                                    const SaveVersion &> {
    CREATE_FUNC(SaveGroup);
    LOAD_FUNC(SaveGroup, QJsonObject &, const SaveVersion &);

  protected:
    QString                                m_name;    ///< Name of group.
    QVector<::std::shared_ptr<SaveModule>> m_modules; ///< Modules in the group.
    QMap<QString, int> m_modulesMacroIndex; ///< Macro index of modules.

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
    SaveGroup(QJsonObject &entry, const SaveVersion &version);

  public:
    /**
     * @brief		Get name of the group.
     *
     * @return		Name of the group.
     */
    const QString &name() const;

    /**
     * @brief		Set name of the group.
     *
     * @param[in]	name	New name.
     */
    void setName(QString name);

    /**
     * @brief		Get module.
     *
     * @return		Modules.
     */
    const QVector<::std::shared_ptr<SaveModule>> &modules() const;

    /**
     * @brief		Get modules.
     *
     * @param[in]	index		Index of the module.
     *
     * @return		List of modules.
     */
    std::shared_ptr<SaveModule> module(int index);

    /**
     * @brief		Set module index.
     *
     * @param[in]	oldIndex	Old index.
     * @param[in]	index		Index in the group, if index = -1, the module
     *							will be move to the end of the group.
     */
    void setIndex(int oldIndex, int index);

    /**
     * @brief		Insert module.
     *
     * @param[in]	index	Index to insert.
     * @param[in]	macro	Module macro.
     * @param[in]	count	Count of the module.
     *
     * @return		Index of the new module. If failed, \c -1 is returned.
     */
    int insertModule(int index, const QString &macro, quint64 count);

    /**
     * @brief		Remove module.
     *
     * @param[in]	index		Module index.
     */
    void removeModule(int index);

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
