#include <atomic>

#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

#include <common.h>
#include <game_data/game_vfs.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameVFS::GameVFS(const QString &                        gamePath,
                 const QMap<int, CatFileInfo> &         info,
                 ::std::function<void(const QString &)> setTextFunc,
                 ::std::function<void(const QString &)> errFunc) :
    m_gamePath(gamePath),
    m_datEntry(new DatFileEntery("/"))
{
    QDir dir(gamePath);

    // Load cat/dat files.
    for (auto &catDatInfo : info) {
        QFile catFile(dir.absoluteFilePath(catDatInfo.cat));
        if (! catFile.open(QIODevice::OpenModeFlag::ReadOnly
                           | QIODevice::OpenModeFlag::ExistingOnly)) {
            qDebug() << "Failed to open file :" << catFile.fileName() << ".";
            errFunc(STR("STR_FAILED_OPEN_FILE").arg(catFile.fileName()));

            return;
        }

        QMutex                 catLock;
        QMutex                 printLock;
        ::std::atomic<bool>    errFlag;
        ::std::atomic<quint64> total;
        ::std::atomic<quint64> printTm;
        printTm = 0;
        errFlag = false;
        total   = 0;

        MultiRun loadTask(::std::function<void()>([&]() -> void {
            QFile              datFile(dir.absoluteFilePath(catDatInfo.dat));
            QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);
            if (! datFile.open(QIODevice::OpenModeFlag::ReadOnly
                               | QIODevice::OpenModeFlag::ExistingOnly)) {
                if (! errFlag.exchange(true)) {
                    qDebug()
                        << "Failed to open file :" << datFile.fileName() << ".";
                    errFunc(
                        STR("STR_FAILED_OPEN_FILE").arg(datFile.fileName()));
                }
                return;
            }

            setTextFunc(STR("STR_LOADING_CAT_DAT_FILE")
                            .arg(catDatInfo.cat)
                            .arg(catDatInfo.dat)
                            .arg(total)
                            .arg(datFile.size()));

            // Scan cat file.
            catLock.lock();
            while ((! catFile.atEnd()) && (! errFlag)) {
                // Get file info
                QString line = catFile.readLine();
                line.remove(QChar('\r'));
                line.remove(QChar('\n'));
                if (line == "") {
                    continue;
                }

                QStringList splittedLine = this->splitCatLine(line);
                quint64     size         = splittedLine[1].toULongLong();
                quint64     offset       = total;
                total += size;
                catLock.unlock();
                if (splittedLine.size() != 4) {
                    if (! errFlag.exchange(true)) {
                        qDebug() << "Broken cat file :" << catFile.fileName();
                        errFunc(STR("STR_FILE_BROKEN").arg(catFile.fileName()));
                    }
                    return;
                }

                if ((qint64)(datFile.size()) - (qint64)offset < (qint64)size) {
                    if (! errFlag.exchange(true)) {
                        qDebug() << "Broken dat file :" << datFile.fileName();
                        errFunc(STR("STR_FILE_BROKEN").arg(datFile.fileName()));
                    }
                    return;
                }

                // Checksum
                if (size != 0) {
                    // Get hash
                    hash.reset();
                    uchar *data = datFile.map(offset, size);
                    hash.addData((const char *)data, size);
                    datFile.unmap(data);

                    // Check
                    if (hash.result().toHex() != splittedLine.back()) {
                        if (! errFlag.exchange(true)) {
                            qDebug()
                                << "Broken dat file :" << datFile.fileName();
                            errFunc(
                                STR("STR_FILE_BROKEN").arg(datFile.fileName()));
                        }
                        return;
                    }
                }

                // Append file
                // Split path
                auto splittedPath = splittedLine[0].split(
                    '/', QString::SplitBehavior::SkipEmptyParts);
                if (splittedPath.empty()) {
                    break;
                }

                // Parent
                ::std::shared_ptr<DatFileEntery> entry = m_datEntry;
                for (auto iter = splittedPath.begin();
                     iter < splittedPath.end() - 1; iter++) {
                    {
                        QMutexLocker locker(&(entry->lock));
                        auto         pathIter = entry->children.find(*iter);
                        if (pathIter == entry->children.end()) {
                            // Create new
                            pathIter = entry->children.find(*iter);
                            entry->children[*iter]
                                = ::std::shared_ptr<DatFileEntery>(
                                    new DatFileEntery(*iter));
                            entry = entry->children[*iter];

                        } else {
                            entry = *pathIter;
                        }
                    }
                    if (! entry->isDirectory) {
                        if (! errFlag.exchange(true)) {
                            qDebug()
                                << "Broken cat file :" << catFile.fileName();
                            errFunc(
                                STR("STR_FILE_BROKEN").arg(catFile.fileName()));
                        }
                        return;
                    }
                }

                // File
                {
                    QMutexLocker locker(&(entry->lock));
                    entry->children[splittedPath.back()]
                        = ::std::shared_ptr<DatFileEntery>(new DatFileEntery(
                            splittedPath.back(), datFile.fileName(), offset,
                            size));
                }
                qDebug() << "Packed file loaded :" << splittedLine[0] << ".";

                {
                    quint64 tm = QDateTime::currentMSecsSinceEpoch();
                    printLock.lock();
                    if (tm - printTm > 100) {
                        printTm = tm;
                        printLock.unlock();
                        setTextFunc(STR("STR_LOADING_CAT_DAT_FILE")
                                        .arg(catDatInfo.cat)
                                        .arg(catDatInfo.dat)
                                        .arg(total)
                                        .arg(datFile.size()));
                    } else {
                        printLock.unlock();
                    }
                }
                catLock.lock();
            }
            setTextFunc(STR("STR_LOADING_CAT_DAT_FILE")
                            .arg(catDatInfo.cat)
                            .arg(catDatInfo.dat)
                            .arg(total)
                            .arg(datFile.size()));
            catLock.unlock();
        }));
        loadTask.run();
        if (errFlag) {
            return;
        }
    }

    this->setGood();
}

/**
 * @brief		Constructor.
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
 */
::std::shared_ptr<GameVFS::FileReader> GameVFS::open(const QString &path)
{
    QDir dir(m_gamePath);

    // Try to open file
    {
        ::std::unique_ptr<QFile> file;
        if (path.front() == '/') {
            file = ::std::unique_ptr<QFile>(
                new QFile(dir.absoluteFilePath("." + path)));
        } else {
            file = ::std::unique_ptr<QFile>(
                new QFile(dir.absoluteFilePath(path)));
        }
        if (file->open(QIODevice::OpenModeFlag::ReadOnly
                       | QIODevice::OpenModeFlag::ExistingOnly)) {
            return ::std::shared_ptr<FileReader>(
                new NormalFileReader(path, ::std::move(file), m_this.lock()));
        }
    }

    // Search
    QStringList splittedPath
        = path.split('/', QString::SplitBehavior::SkipEmptyParts);

    ::std::shared_ptr<DatFileEntery> entry = m_datEntry;
    for (auto name : splittedPath) {
        if (! entry->isDirectory) {
            return nullptr;
        }

        auto pathIter = entry->children.find(name);
        if (pathIter == entry->children.end()) {
            return nullptr;
        }
        entry = *pathIter;
    }
    if (entry->isDirectory) {
        return nullptr;
    }

    // Open dat file
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
 */
::std::shared_ptr<GameVFS::DirReader> GameVFS::openDir(const QString &path)
{
    bool exists = false;
    QDir root(m_gamePath);

    // Search filesystem
    {
        QDir dir(root.absoluteFilePath(path));
        if (dir.exists()) {
            exists = true;
        }
    }

    // Search dat files.
    QStringList splittedPath
        = path.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);

    ::std::shared_ptr<DatFileEntery> entry = m_datEntry;
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
        entry = *pathIter;
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

    // Hash
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

    // Timestamp
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

    // Size
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

    // Name
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
 */
GameVFS::FileReader::FileReader(const QString &            path,
                                ::std::shared_ptr<GameVFS> vfs) :
    m_path(path.split('/', QString::SplitBehavior::SkipEmptyParts)),
    m_name(m_path.back()), m_vfs(vfs)
{}

/**
 * @brief	Get path of the file.
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
 */
const QString &GameVFS::FileReader::name() const
{
    return m_name;
}

/**
 * @brief		Read a line.
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
 */
qint64 GameVFS::PackedFileReader::read(void *buffer, quint64 size)
{
    size = min(size, m_offset + m_size - m_file->pos());
    return m_file->read((char *)buffer, size);
}

/**
 * @brief		Read file.
 */
QByteArray GameVFS::PackedFileReader::read(quint64 size)
{
    size = min(size, m_offset + m_size - m_file->pos());
    return m_file->read(size);
}

/**
 * @brief		Read all data after corrent position in the file.
 */
QByteArray GameVFS::PackedFileReader::readAll()
{
    quint64 size = m_offset + m_size - m_file->pos();
    return m_file->read(size);
}

/**
 * @brief		Seek file.
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
 */
GameVFS::NormalFileReader::NormalFileReader(const QString &            path,
                                            ::std::unique_ptr<QFile>   file,
                                            ::std::shared_ptr<GameVFS> vfs) :
    GameVFS::FileReader(path, vfs),
    m_file(::std::move(file))
{}

/**
 * @brief		Read file.
 */
qint64 GameVFS::NormalFileReader::read(void *buffer, quint64 size)
{
    return m_file->read((char *)buffer, size);
}

/**
 * @brief		Read file.
 */
QByteArray GameVFS::NormalFileReader::read(quint64 size)
{
    return m_file->read(size);
}

/**
 * @brief		Read all data after corrent position in the file.
 */
QByteArray GameVFS::NormalFileReader::readAll()
{
    return m_file->readAll();
}

/**
 * @brief		Seek file.
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
 */
GameVFS::DirReader::DirReader(const QString &                  path,
                              ::std::shared_ptr<DatFileEntery> entry,
                              ::std::shared_ptr<GameVFS>       vfs) :
    m_path(path.split('/', QString::SplitBehavior::SkipEmptyParts)),
    m_name(m_path.back()), m_vfs(vfs), m_enteries(new QVector<DirEntry>)
{
    QDir rootDir(m_vfs->m_gamePath);
    QDir dir(rootDir.absoluteFilePath(QString(".") + this->path()));
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
                {datEntry->name, (datEntry->isDirectory ? EntryType::Directory
                                                        : EntryType::File)});
        }
    }
}

/**
 * @brief	Get name of the directory.
 */
const QString &GameVFS::DirReader::name() const
{
    return m_name;
}

/**
 * @brief	Get path of the directory.
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
 */
GameVFS::DirReader::iterator GameVFS::DirReader::begin()
{
    return Iterator(m_enteries, m_enteries->begin());
}

/**
 * @brief		Get iterator point to the end.
 */
GameVFS::DirReader::iterator GameVFS::DirReader::end()
{
    return Iterator(m_enteries, m_enteries->end());
}

/**
 * @brief		Get number of files in the directory.
 */
quint64 GameVFS::DirReader::count()
{
    return m_enteries->size();
}

/**
 * @brief		Destructor.
 */
GameVFS::DirReader::~DirReader() {}

/**
 * @brief		Constructor.
 */
GameVFS::DirReader::Iterator::Iterator() : m_enteries(nullptr) {}

/**
 * @brief		Constructor.
 */
GameVFS::DirReader::Iterator::Iterator(
    ::std::shared_ptr<QVector<DirEntry>> enteries,
    QVector<DirEntry>::iterator          iter) :
    m_enteries(enteries),
    m_iterator(iter)
{}

/**
 * @brief		Constructor.
 */
GameVFS::DirReader::Iterator::Iterator(const Iterator &iter) :
    m_enteries(iter.m_enteries), m_iterator(iter.m_iterator)
{}

/**
 * @brief		Destructor.
 */
GameVFS::DirReader::Iterator::~Iterator() {}

// Operators
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
