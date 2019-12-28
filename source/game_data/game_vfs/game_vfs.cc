#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <common.h>
#include <game_data/game_vfs/game_vfs.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	gamePath		Path of game.
 * @param[in]	info			Cat files info.
 * @param[in]	setTextFunc		Callback to set text.
 * @param[in]	errFunc			Callback to show error.
 */
GameVFS::GameVFS(const QString &                        gamePath,
                 const QMap<int, CatFileInfo> &         info,
                 ::std::function<void(const QString &)> setTextFunc,
                 ::std::function<void(const QString &)> errFunc) :
    m_gamePath(gamePath),
    m_datEntry({"/", true, {}, {"", 0, 0}})
{
    QDir dir(gamePath);

    /// Load cat/dat files.
    for (auto &catDatInfo : info) {
        QFile catFile(dir.absoluteFilePath(catDatInfo.cat));
        if (! catFile.open(QIODevice::OpenModeFlag::ReadOnly
                           | QIODevice::OpenModeFlag::ExistingOnly)) {
            qDebug() << "Failed to open file :" << catFile.fileName() << ".";
            errFunc(STR("STR_FAILED_OPEN_FILE").arg(catFile.fileName()));

            return;
        }

        QFile datFile(dir.absoluteFilePath(catDatInfo.dat));
        if (! datFile.open(QIODevice::OpenModeFlag::ReadOnly
                           | QIODevice::OpenModeFlag::ExistingOnly)) {
            qDebug() << "Failed to open file :" << datFile.fileName() << ".";
            errFunc(STR("STR_FAILED_OPEN_FILE").arg(datFile.fileName()));
            return;
        }

        setTextFunc(STR("STR_LOADING_CAT_DAT_FILE")
                        .arg(catDatInfo.cat)
                        .arg(catDatInfo.dat)
                        .arg(0)
                        .arg(datFile.size()));

        /// Scan cat file.
        while (! catFile.atEnd()) {
            /// Get file info
            QString line = catFile.readLine();
            line.remove(QChar('\r'));
            line.remove(QChar('\n'));
            if (line == "") {
                continue;
            }

            QStringList splittedLine = this->splitCatLine(line);
            quint64     size         = splittedLine[1].toULongLong();
            quint64     offset       = datFile.pos();
            if (splittedLine.size() != 4) {
                qDebug() << "Broken cat file :" << catFile.fileName();
                errFunc(STR("STR_FILE_BROKEN").arg(catFile.fileName()));
                return;
            }

            if ((quint64)(datFile.size() - datFile.pos()) < size) {
                qDebug() << "Broken dat file :" << datFile.fileName();
                errFunc(STR("STR_FILE_BROKEN").arg(datFile.fileName()));
                return;
            }

            /// Checksum
            if (size != 0) {
                /// Get hash
                QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);
                quint64            sizeRead = 0;
                while (sizeRead < size) {
                    /// Size to read.
                    quint64 sizeToRead
                        = min((quint64)(64 * 1024 * 1024), (quint64)(size - sizeRead));
                    hash.addData(datFile.read(sizeToRead));

                    sizeRead += sizeToRead;
                }

                /// Check
                if (hash.result().toHex() != splittedLine.back()) {
                    qDebug() << "Broken dat file :" << datFile.fileName();
                    errFunc(STR("STR_FILE_BROKEN").arg(datFile.fileName()));
                    return;
                }
            }

            /// Append file
            /// Split path
            auto splittedPath = splittedLine[0].split(
                '/', QString::SplitBehavior::SkipEmptyParts);
            if (splittedPath.empty()) {
                break;
            }

            /// Parent
            DatFileEntery *entry = &m_datEntry;
            for (auto iter = splittedPath.begin();
                 iter < splittedPath.end() - 1; iter++) {
                auto pathIter = entry->children.find(*iter);
                if (pathIter == entry->children.end()) {
                    /// Create new
                    entry->children[*iter] = {*iter, true, {}, {"", 0, 0}};
                    entry                  = &(entry->children[*iter]);
                } else {
                    entry = (&*pathIter);
                }
                if (! entry->isDirectory) {
                    qDebug() << "Broken cat file :" << catFile.fileName();
                    errFunc(STR("STR_FILE_BROKEN").arg(catFile.fileName()));
                    return;
                }
            }

            /// File
            entry->children[splittedPath.back()]
                = {splittedPath.back(),
                   false,
                   {},
                   {datFile.fileName(), offset, size}};
            qDebug() << "Packed file loaded :" << splittedLine[0] << ".";

            setTextFunc(STR("STR_LOADING_CAT_DAT_FILE")
                            .arg(catDatInfo.cat)
                            .arg(catDatInfo.dat)
                            .arg(datFile.pos())
                            .arg(datFile.size()));
        }
    }

    this->setGood();
}

/**
 * @brief		Constructor.
 *
 * @param[in]	gamePath		Path of game.
 * @param[in]	info			Cat files info.
 * @param[in]	setTextFunc		Callback to set text.
 * @param[in]	errFunc			Callback to show error.
 *
 * @return		On success, a nmew object is returned. Otherwise returns
 *				nullptr.
 */
::std::shared_ptr<GameVFS>
    GameVFS::create(const QString &                        gamePath,
                    const QMap<int, CatFileInfo> &         info,
                    ::std::function<void(const QString &)> setTextFunc,
                    ::std::function<void(const QString &)> errFunc)
{
    ::std::shared_ptr<GameVFS> ret(
        new GameVFS(gamePath, info, setTextFunc, errFunc));
    if (ret->good()) {
        ret->m_this = ret;
        return ret;
    } else {
        return nullptr;
    }
}

/**
 * @brief		Open file.
 *
 * @param[in]	path		Path of file.
 *
 * @return		On success, a \c FileReader object is returned. Otherwise
 *				returns nullptr.
 */
::std::shared_ptr<GameVFS::FileReader> GameVFS::open(const QString &path)
{
    QDir dir(m_gamePath);

    /// Try to open file
    {
        ::std::unique_ptr<QFile> file(new QFile(dir.absoluteFilePath(path)));
        if (file->open(QIODevice::OpenModeFlag::ReadOnly
                       | QIODevice::OpenModeFlag::ExistingOnly)) {
            return ::std::shared_ptr<FileReader>(
                new NormalFileReader(path, ::std::move(file), m_this.lock()));
        }
    }

    /// Search
    QStringList splittedPath
        = path.split('/', QString::SplitBehavior::SkipEmptyParts);

    DatFileEntery *entry = &m_datEntry;
    for (auto name : splittedPath) {
        if (! entry->isDirectory) {
            return nullptr;
        }

        auto pathIter = entry->children.find(name);
        if (pathIter == entry->children.end()) {
            return nullptr;
        }
        entry = &(*pathIter);
    }
    if (entry->isDirectory) {
        return nullptr;
    }

    /// Open dat file
    ::std::unique_ptr<QFile> file(
        new QFile(dir.absoluteFilePath(entry->fileInfo.datName)));
    if (file->open(QIODevice::OpenModeFlag::ReadOnly
                   | QIODevice::OpenModeFlag::ExistingOnly)) {
        return ::std::shared_ptr<FileReader>(new PackedFileReader(
            path, ::std::move(file), entry->fileInfo.offset,
            entry->fileInfo.size, m_this.lock()));
    }

    return nullptr;
}

/**
 * @brief		Open directory.
 *
 * @param[in]	path		Path of directory.
 *
 * @return		On success, a \c DirReader object is returned. Otherwise
 *				returns nullptr.
 */
::std::shared_ptr<GameVFS::DirReader> GameVFS::openDir(const QString &path)
{
    bool exists = false;
    QDir root(m_gamePath);

    /// Search filesystem
    {
        QDir dir(root.absoluteFilePath(path));
        if (dir.exists()) {
            exists = true;
        }
    }

    /// Search dat files.
    QStringList splittedPath
        = path.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);

    DatFileEntery *entry = &m_datEntry;
    for (auto name : splittedPath) {
        if (! entry->isDirectory) {
            return nullptr;
        }

        auto pathIter = entry->children.find(name);
        if (pathIter == entry->children.end()) {
            if (exists) {
                return ::std::shared_ptr<DirReader>(
                    new DirReader(path, nullptr, m_this.lock()));
            } else {
                return nullptr;
            }
        }
        entry = &(*pathIter);
    }

    if (! entry->isDirectory) {
        return nullptr;
    }

    return ::std::shared_ptr<DirReader>(
        new DirReader(path, entry, m_this.lock()));
}

/**
 * @brief	Destructor.
 */
GameVFS::~GameVFS() {}

/**
 * @brief		Open directory.
 *
 * @param[in]	line	Data in line.
 *
 * @return		Splitted data.
 */
QStringList GameVFS::splitCatLine(const QString &line)
{
    QStringList ret;
    QString     s;

    auto iter = line.rbegin();
    while (*iter == ' ' || *iter == '\t') {
        iter++;
        if (iter == line.rend()) {
            return ret;
        }
    }

    /// Hash
    s.clear();
    while (*iter != ' ' && *iter != '\t') {
        s.push_front(*iter);
        iter++;
        if (iter == line.rend()) {
            ret.push_front(s);
            return ret;
        }
    }
    ret.push_front(s);
    while (*iter == ' ' || *iter == '\t') {
        iter++;
        if (iter == line.rend()) {
            return ret;
        }
    }

    /// Timestamp
    s.clear();
    while (*iter != ' ' && *iter != '\t') {
        s.push_front(*iter);
        iter++;
        if (iter == line.rend()) {
            ret.push_front(s);
            return ret;
        }
    }
    ret.push_front(s);
    while (*iter == ' ' || *iter == '\t') {
        iter++;
        if (iter == line.rend()) {
            return ret;
        }
    }

    /// Size
    s.clear();
    while (*iter != ' ' && *iter != '\t') {
        s.push_front(*iter);
        iter++;
        if (iter == line.rend()) {
            ret.push_front(s);
            return ret;
        }
    }
    ret.push_front(s);
    while (*iter == ' ' || *iter == '\t') {
        iter++;
        if (iter == line.rend()) {
            return ret;
        }
    }

    /// Name
    s.clear();
    while (iter != line.rend()) {
        s.push_front(*iter);
        iter++;
    }
    ret.push_front(s);

    return ret;
}

/**
 * @brief		Constructor.
 *
 * @param[in]	path		Path of file.
 * @param[in]	vfs			VFS.
 */
GameVFS::FileReader::FileReader(const QString &            path,
                                ::std::shared_ptr<GameVFS> vfs) :
    m_path(path.split('/', QString::SplitBehavior::SkipEmptyParts)),
    m_name(m_path.back()), m_vfs(vfs)
{}

/**
 * @brief	Get path of the file.
 *
 * @return	Path of the file.
 */
QString GameVFS::FileReader::path() const
{
    QString ret = "";
    for (auto &n : m_path) {
        ret += "/";
        ret += n;
    }

    return ret;
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
 * @brief		Read a line.
 *
 * @return		Data in a line.
 */
QByteArray GameVFS::FileReader::readLine()
{
    QByteArray array;
    for (auto c = this->read(1); c.size() != 0; c = this->read(1)) {
        array.append(c[0]);
        if (c[0] == '\n') {
            break;
        }
    }

    return array;
}

/**
 * @brief	Destructor.
 *
 */
GameVFS::FileReader::~FileReader() {}

/**
 * @brief		Constructor.
 *
 * @param[in]	path		Path of file.
 * @param[in]	file		Qt file object for data file.
 * @param[in]	offset		Begin offset.
 * @param[in]	size		File size.
 * @param[in]	vfs			VFS.
 */
GameVFS::PackedFileReader::PackedFileReader(const QString &            path,
                                            ::std::unique_ptr<QFile>   file,
                                            quint64                    offset,
                                            quint64                    size,
                                            ::std::shared_ptr<GameVFS> vfs) :
    GameVFS::FileReader(path, vfs),
    m_file(::std::move(file)), m_offset(offset), m_size(size)
{
    m_file->seek(m_offset);
}

/**
 * @brief		Read file.
 *
 * @param[out]	buffer		Buffer to restore data read.
 * @param[in]	size		Size of buffer.
 *
 * @return		Size read.
 */
qint64 GameVFS::PackedFileReader::read(void *buffer, quint64 size)
{
    size = min(size, m_offset + m_size - m_file->pos());
    return m_file->read((char *)buffer, size);
}

/**
 * @brief		Read file.
 *
 * @param[in]	size		Size to read.
 *
 * @return		Data read.
 */
QByteArray GameVFS::PackedFileReader::read(quint64 size)
{
    size = min(size, m_offset + m_size - m_file->pos());
    return m_file->read(size);
}

/**
 * @brief		Read all data after corrent position in the file.
 *
 * @return		Data read.
 */
QByteArray GameVFS::PackedFileReader::readAll()
{
    quint64 size = m_offset + m_size - m_file->pos();
    return m_file->read(size);
}

/**
 * @brief		Seek file.
 *
 * @param[in]	offset		Offset to seek.
 * @param[in]	whence		Where to begin.
 *
 * @return		Current position.
 */
qint64 GameVFS::PackedFileReader::seek(qint64 offset, Whence whence)
{
    qint64 pos;
    switch (whence) {
        case Whence::Set:
            pos = m_offset;
            break;

        case Whence::Current:
            pos = m_file->pos();
            break;

        case Whence::End:
            pos = m_offset + m_size;
            break;
    }

    pos += offset;
    pos = min(pos, (qint64)(m_offset + m_size));
    m_file->seek(pos);

    return m_file->pos() - m_offset;
}

/**
 * @brief		Check if current position is at the end of current file.
 *
 * @return		If current position reachs the end of the file, true is
 *				returned. Otherwise returns false.
 */
bool GameVFS::PackedFileReader::atEnd()
{
    return (quint64)(m_file->pos()) >= m_offset + m_size;
}

/**
 * @brief	Destructor.
 */
GameVFS::PackedFileReader::~PackedFileReader() {}

/**
 * @brief		Constructor.
 *
 * @param[in]	path		Path of file.
 * @param[in]	file		Qt file object for data file.
 * @param[in]	vfs			VFS.
 */
GameVFS::NormalFileReader::NormalFileReader(const QString &            path,
                                            ::std::unique_ptr<QFile>   file,
                                            ::std::shared_ptr<GameVFS> vfs) :
    GameVFS::FileReader(path, vfs),
    m_file(::std::move(file))
{}

/**
 * @brief		Read file.
 *
 * @param[out]	buffer		Buffer to restore data read.
 * @param[in]	size		Size of buffer.
 *
 * @return		Size read.
 */
qint64 GameVFS::NormalFileReader::read(void *buffer, quint64 size)
{
    return m_file->read((char *)buffer, size);
}

/**
 * @brief		Read file.
 *
 * @param[in]	size		Size to read.
 *
 * @return		Data read.
 */
QByteArray GameVFS::NormalFileReader::read(quint64 size)
{
    return m_file->read(size);
}

/**
 * @brief		Read all data after corrent position in the file.
 *
 * @return		Data read.
 */
QByteArray GameVFS::NormalFileReader::readAll()
{
    return m_file->readAll();
}

/**
 * @brief		Seek file.
 *
 * @param[in]	offset		Offset to seek.
 * @param[in]	whence		Where to begin.
 *
 * @return		Current position.
 */
qint64 GameVFS::NormalFileReader::seek(qint64 offset, Whence whence)
{
    qint64 pos;
    switch (whence) {
        case Whence::Set:
            pos = 0;
            break;

        case Whence::Current:
            pos = m_file->pos();
            break;

        case Whence::End:
            pos = m_file->size();
            break;
    }

    pos += offset;
    m_file->seek(pos);

    return m_file->pos();
}

/**
 * @brief		Check if current position is at the end of current file.
 *
 * @return		If current position reachs the end of the file, true is
 *				returned. Otherwise returns false.
 */
bool GameVFS::NormalFileReader::atEnd()
{
    return m_file->atEnd();
}

/**
 * @brief	Destructor.
 */
GameVFS::NormalFileReader::~NormalFileReader() {}

/**
 * @brief		Constructor.
 *
 * @param[in]	path		Path of directory.
 * @param[in]	entry		Dat file entery. If not found, set it to
 *							nullptr.
 * @param[in]	vfs			VFS.
 */
GameVFS::DirReader::DirReader(const QString &            path,
                              DatFileEntery *            entry,
                              ::std::shared_ptr<GameVFS> vfs) :
    m_path(path.split('/', QString::SplitBehavior::SkipEmptyParts)),
    m_name(m_path.back()), m_vfs(vfs), m_enteries(new QVector<DirEntry>)
{
    QDir rootDir(m_vfs->m_gamePath);
    QDir dir(rootDir.absoluteFilePath(this->path()));
    if (dir.exists()) {
        for (auto &info : dir.entryInfoList()) {
            m_enteries->append(
                {info.fileName(),
                 (info.isDir() ? EntryType::Directory : EntryType::File)});
        }
    }
    if (entry != nullptr) {
        for (auto &datEntry : entry->children) {
            m_enteries->append(
                {datEntry.name, (datEntry.isDirectory ? EntryType::Directory
                                                      : EntryType::File)});
        }
    }
}

/**
 * @brief	Get name of the directory.
 *
 * @return	Name of the directory.
 */
const QString &GameVFS::DirReader::name() const
{
    return m_name;
}

/**
 * @brief	Get path of the directory.
 *
 * @return	Path.
 */
QString GameVFS::DirReader::path() const
{
    QString ret = "";
    for (auto &n : m_path) {
        ret += "/";
        ret += n;
    }

    return ret;
}

/**
 * @brief		Convert relative path to absolute path.
 *
 * @param[in]	path	Relative path.
 *
 * @return		Absolute path.
 */
QString GameVFS::DirReader::absPath(const QString &path) const
{
    QStringList dirList = m_path;
    for (auto &n : path.split('/', QString::SplitBehavior::SkipEmptyParts)) {
        if (n == ".") {
            continue;
        } else if (n == "..") {
            if (dirList.empty()) {
                return "";
            } else {
                dirList.pop_back();
            }
        } else {
            dirList.push_back(n);
        }
    }

    QString ret = "";
    for (auto &n : dirList) {
        ret += "/";
        ret += n;
    }

    return ret;
}

/**
 * @brief		Get iterator point to the first entery.
 *
 * @return		Iterator.
 */
GameVFS::DirReader::iterator GameVFS::DirReader::begin()
{
    return Iterator(m_enteries, m_enteries->begin());
}

/**
 * @brief		Get iterator point to the end.
 *
 * @return		Iterator.
 */
GameVFS::DirReader::iterator GameVFS::DirReader::end()
{
    return Iterator(m_enteries, m_enteries->end());
}

/**
 * @brief		Destructor.
 */
GameVFS::DirReader::~DirReader() {}

/**
 * @brief		Constructor.
 *
 * @param[in]	enteries		Directory enteries.
 * @param[in]	iter			Iterator.
 */
GameVFS::DirReader::Iterator::Iterator(
    ::std::shared_ptr<QVector<DirEntry>> enteries,
    QVector<DirEntry>::iterator          iter) :
    m_enteries(enteries),
    m_iterator(iter)
{}

/**
 * @brief		Constructor.
 *
 * @param[in]	iter			Iterator to copy..
 */
GameVFS::DirReader::Iterator::Iterator(const Iterator &iter) :
    m_enteries(iter.m_enteries), m_iterator(iter.m_iterator)
{}

/**
 * @brief		Destructor.
 */
GameVFS::DirReader::Iterator::~Iterator() {}

/// Operators
const GameVFS::DirReader::DirEntry *
    GameVFS::DirReader::Iterator::operator->() const
{
    return &(*m_iterator);
}

const GameVFS::DirReader::DirEntry &
    GameVFS::DirReader::Iterator::operator*() const
{
    return *m_iterator;
}

GameVFS::DirReader::Iterator &GameVFS::DirReader::Iterator::operator++()
{
    ++m_iterator;
    return *this;
}

GameVFS::DirReader::Iterator GameVFS::DirReader::Iterator::operator++(int)
{
    Iterator ret(*this);
    ++m_iterator;
    return ret;
}

GameVFS::DirReader::Iterator &GameVFS::DirReader::Iterator::operator--()
{
    --m_iterator;
    return *this;
}

GameVFS::DirReader::Iterator GameVFS::DirReader::Iterator::operator--(int)
{
    Iterator ret(*this);
    --m_iterator;
    return ret;
}

GameVFS::DirReader::Iterator
    GameVFS::DirReader::Iterator::operator+(int n) const
{
    Iterator ret(*this);
    ret.m_iterator += n;
    return ret;
}

GameVFS::DirReader::Iterator
    GameVFS::DirReader::Iterator::operator-(int n) const
{
    Iterator ret(*this);
    ret.m_iterator -= n;
    return ret;
}

int GameVFS::DirReader::Iterator::operator-(const Iterator &iter) const
{
    return m_iterator - iter.m_iterator;
}

GameVFS::DirReader::Iterator &
    GameVFS::DirReader::Iterator::operator=(const Iterator &iter)
{
    m_enteries = iter.m_enteries;
    m_iterator = iter.m_iterator;

    return *this;
}

GameVFS::DirReader::Iterator &GameVFS::DirReader::Iterator::operator+=(int n)
{
    m_iterator += n;
    return *this;
}

GameVFS::DirReader::Iterator &GameVFS::DirReader::Iterator::operator-=(int n)
{
    m_iterator -= n;
    return *this;
}

bool GameVFS::DirReader::Iterator::operator>(const Iterator &iter) const
{
    return m_iterator > iter.m_iterator;
}

bool GameVFS::DirReader::Iterator::operator>=(const Iterator &iter) const
{
    return m_iterator >= iter.m_iterator;
}

bool GameVFS::DirReader::Iterator::operator<(const Iterator &iter) const
{
    return m_iterator < iter.m_iterator;
}

bool GameVFS::DirReader::Iterator::operator<=(const Iterator &iter) const
{
    return m_iterator <= iter.m_iterator;
}

bool GameVFS::DirReader::Iterator::operator==(const Iterator &iter) const
{
    return m_iterator == iter.m_iterator;
}

bool GameVFS::DirReader::Iterator::operator!=(const Iterator &iter) const
{
    return m_iterator != iter.m_iterator;
}
