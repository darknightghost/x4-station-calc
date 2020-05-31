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
class Save {
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
     * @param[in]	entry		Entery of the module.
     * @param[in]	version		Version of the input save file.
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
     * @brief		Set group index.
     *
     * @param[in]	group	Group.
     * @param[in]	index	Index in the group, if index = -1, the module will
     *						be move to the end of the group.
     */
    void setIndex(::std::shared_ptr<SaveGroup> group, int index);

    /**
     * @brief		Add group.
     *
     * @param[in]	group	Group.
     */
    void addGroup(::std::shared_ptr<SaveGroup> group);

    /**
     * @brief		Remove module.
     *
     * @param[in]	group	Group.
     */
    void removeGroup(::std::shared_ptr<SaveGroup> group);

    /**
     * @brief		Write save file.
     *
     * @return		On success, the method returns \c true, otherwise returns
     *				\c false.
     */
    bool write() const;

    /**
     * @brief		Destructor.
     */
    virtual ~Save();
};
