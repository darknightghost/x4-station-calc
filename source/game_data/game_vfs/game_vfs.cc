#include <game_data/game_vfs/game_vfs.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	path		Path of file.
 */
GameVFS::FileReader::FileReader(const QString &path) : m_path(path)
{
    QStringList splitted
        = path.split('/', QString::SplitBehavior::SkipEmptyParts);
    m_name = splitted.back();
}

/**
 * @brief	Get path of the file.
 *
 * @return	Path of the file.
 */
const QString &GameVFS::FileReader::path() const
{
    return m_path;
}

/**
 * @brief	Get name of the file.
 *
 * @return	Name of the file.
 */
const QString &GameVFS::FileReader::name() const
{
    return m_name;
}

/**
 * @brief	Destructor.
 *
 */
GameVFS::FileReader::~FileReader() {}
