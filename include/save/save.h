#pragma once

#include <QtCore/QMap>
#include <QtCore/QVector>

#include <interfaces/i_create_factory_func.h>
#include <interfaces/i_load_factory_func.h>
#include <save/save_group.h>
#include <save/save_module.h>

/**
 * @brief		Save.
 */
class Save :
    virtual public ICreateFactoryFunc<Save()>,
    virtual public ILoadFactoryFunc<Save(const QString &)> {
    CREATE_FUNC(Save);
    LOAD_FUNC(Save, const QString &);

  public:
    static const SaveVersion _currentVersion; ///< Current version.

  protected:
    QString                                 m_path;   ///< Path of the file.
    QVector<::std::shared_ptr<SaveGroup>>   m_groups; ///< Groups.
    QMap<::std::shared_ptr<SaveGroup>, int> m_groupsIndex; ///< Index of groups.
  protected:
    /**
     * @brief		Create a save.
     */
    Save();

    /**
     * @brief		Load a save file.
     *
     * @param[in]	path		Path of the save file.
     */
    Save(const QString &path);

  public:
    /**
     * @brief		Get file path.
     */
    const QString &path() const;

    /**
     * @brief		Set file path.
     *
     * @param[in]	path		Path.
     */
    void setPath(const QString &path);

    /**
     * @brief		Get groups.
     *
     * @return		List of groups.
     */
    const QVector<::std::shared_ptr<SaveGroup>> &groups() const;

    /**
     * @brief		Get group.
     *
     * @param[in]	index	Index of the group.
     *
     * @return		Group.
     */
    ::std::shared_ptr<SaveGroup> group(int index);

    /**
     * @brief		Set group index.
     *
     * @param[in]	oldIndex	Old index.
     * @param[in]	index		New index.
     */
    void setIndex(int oldIndex, int index);

    /**
     * @brief		Insert group.
     *
     * @param[in]	index	Index to insert.
     * @param[in]	group	Group.
     *
     * @return		Group index.
     */
    int insertGroup(int index, ::std::shared_ptr<SaveGroup> group);

    /**
     * @brief		Remove module.
     *
     * @param[in]	index		Group index.
     */
    void removeGroup(int index);

    /**
     * @brief		Write save file.
     *
     * @return		On success, the method returns \c true, otherwise returns
     *				\c false.
     */
    bool write() const;

    /**
     * @brief		Write save file.
     *
     * @param[in]   path    Path to write.
     *
     * @return		On success, the method returns \c true and the path of the
     *              save is setted to \c path, otherwise returns \c false.
     */
    bool write(const QString &path);

    /**
     * @brief		Destructor.
     */
    virtual ~Save();
};
