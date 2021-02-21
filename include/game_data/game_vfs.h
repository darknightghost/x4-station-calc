#pragma once

#include <functional>
#include <memory>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include <interfaces/i_initialized.h>

/**
 * @brief	Virtual filesystem of the game.
 */
class GameVFS : private IInitialized
{
  public:
    /**
     * @brief	Information of cat file.
     *
     */
    struct CatFileInfo {
        QString cat; ///< Name of cat file.
        QString dat; ///< Name of dat file.
    };

    /**
     * @brief	File reader.
     */
    class FileReader;

    /**
     * @brief	File eeader for packed files.
     */
    class PackedFileReader;

    /**
     * @brief	File reader for normal files.
     */
    class NormalFileReader;

    /**
     * @brief	Directory reader.
     */
    class DirReader;

    /**
     * @brief	Directory reader for packed files.
     */
    class PackedDirReader;

    /**
     * @brief	Directory reader for normal files.
     */
    class NormalDirReader;

  protected:
    struct DatFileEntery;
    /**
     * @brief	Dat file entery
     */
    struct DatFileEntery {
        QString name;        ///< Name.
        bool    isDirectory; ///< Directory flag.
        QMap<QString, ::std::shared_ptr<DatFileEntery>> children; ///< Children.
        QMutex                                          lock;     ///< Lock.
        struct _tmp1 {
            QString datName; ///< Name of dat file.
            quint64 offset;  ///< Offset.
            quint64 size;    ///< Size.
            QString hash;    ///< File hash.
            bool    checked; ///< File checked flag.
        } fileInfo;          ///< File infomation;

        /**
         * @brief		Constructor.
         *
         */
        DatFileEntery() {}

        /**
         * @brief		Construct a directory node.
         *
         * @param[in]	name	Name.
         */
        DatFileEntery(const QString &name) :
            name(name), isDirectory(true), children({}),
            fileInfo({"", 0, 0, QString(), false})
        {}

        /**
         * @brief		Construct a file node.
         *
         * @param[in]	name	Name.
         * @param[in]	datName	Name of dat file.
         * @param[in]	offset	Offset in dat file.
         * @param[in]	size	File size.
         * @param[in]	hash    Hash.
         */
        DatFileEntery(const QString &name,
                      const QString &datName,
                      quint64        offset,
                      quint64        size,
                      const QString &hash) :
            name(name),
            isDirectory(false), children({}),
            fileInfo({datName, offset, size, hash, false})
        {}

        DatFileEntery(const DatFileEntery &) = delete;
        DatFileEntery(DatFileEntery &&)      = delete;
    };

  private:
    QString                          m_gamePath; ///< Game path.
    ::std::shared_ptr<DatFileEntery> m_datEntry; ///< Enteries.
    ::std::weak_ptr<GameVFS>         m_this;     ///< This reference.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gamePath		Path of game.
     * @param[in]	info			Cat files info.
     * @param[in]	setTextFunc		Callback to set text.
     * @param[in]	errFunc			Callback to show error.
     */
    GameVFS(const QString &                        gamePath,
            const QMap<QString, CatFileInfo> &     info,
            ::std::function<void(const QString &)> setTextFunc,
            ::std::function<void(const QString &)> errFunc);

  public:
    /**
     * @brief		Create a VFS object.
     *
     * @param[in]	gamePath		Path of game.
     * @param[in]	info			Cat files info.
     * @param[in]	setTextFunc		Callback to set text.
     * @param[in]	errFunc			Callback to show error.
     *
     * @return		On success, a nmew object is returned. Otherwise returns
     *				nullptr.
     */
    static ::std::shared_ptr<GameVFS>
        create(const QString &                        gamePath,
               const QMap<QString, CatFileInfo> &     info,
               ::std::function<void(const QString &)> setTextFunc,
               ::std::function<void(const QString &)> errFunc);

    /**
     * @brief		Open file.
     *
     * @param[in]	path		Path of file.
     *
     * @return		On success, a \c FileReader object is returned.
     *Otherwise returns nullptr.
     */
    ::std::shared_ptr<FileReader> open(const QString &path);

    /**
     * @brief		Open directory.
     *
     * @param[in]	path		Path of directory.
     *
     * @return		On success, a \c DirReader object is returned. Otherwise
     *				returns nullptr.
     */
    ::std::shared_ptr<DirReader> openDir(const QString &path);

    /**
     * @brief	Destructor.
     */
    virtual ~GameVFS();

  private:
    /**
     * @brief		Open directory.
     *
     * @param[in]	line	Data in line.
     *
     * @return		Splitted data.
     */
    QStringList splitCatLine(const QString &line);
};

/**
 * @brief	FileReader.
 *
 */
class GameVFS::FileReader
{
  protected:
    QStringList                m_path; ///< Path of the file.
    QString                    m_name; ///< Name of the file.
    ::std::shared_ptr<GameVFS> m_vfs;  ///< VFS.

  public:
    /**
     * @brief	Whence.
     */
    enum Whence
    {
        Set     = 0x01, ///< From begining of the file.
        Current = 0x02, ///< From current position of the file.
        End     = 0x03  ///< From the end of the file.
    };

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     * @param[in]	vfs			VFS.
     */
    FileReader(const QString &path, ::std::shared_ptr<GameVFS> vfs);

    /**
     * @brief	Get path of the file.
     *
     * @return	Path of the file.
     */
    QString path() const;

    /**
     * @brief	Get name of the file.
     *
     * @return	Name of the file.
     */
    const QString &name() const;

    /**
     * @brief		Read file.
     *
     * @param[out]	buffer		Buffer to restore data read.
     * @param[in]	size		Size of buffer.
     *
     * @return		Size read.
     */
    virtual qint64 read(void *buffer, quint64 size) = 0;

    /**
     * @brief		Read file.
     *
     * @param[in]	size		Size to read.
     *
     * @return		Data read.
     */
    virtual QByteArray read(quint64 size) = 0;

    /**
     * @brief		Read all data after corrent position in the file.
     *
     * @return		Data read.
     */
    virtual QByteArray readAll() = 0;

    /**
     * @brief		Read a line.
     *
     * @return		Data in a line.
     */
    QByteArray readLine();

    /**
     * @brief		Check if current position is at the end of current file.
     *
     * @return		If current position reachs the end of the file, true is
     *				returned. Otherwise returns false.
     */
    virtual bool atEnd() = 0;

    /**
     * @brief		Seek file.
     *
     * @param[in]	offset		Offset to seek.
     * @param[in]	whence		Where to begin.
     *
     * @return		Current position.
     */
    virtual qint64 seek(qint64 offset, Whence whence = Whence::Current) = 0;

    /**
     * @brief	Destructor.
     */
    virtual ~FileReader();
};

/**
 * @brief	FileReader for packed files.
 */
class GameVFS::PackedFileReader : public GameVFS::FileReader
{
  protected:
    ::std::unique_ptr<QFile> m_file;   ///< File object.
    quint64                  m_offset; ///< Offset.
    quint64                  m_size;   ///< File size.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     * @param[in]	file		Qt file object for data file.
     * @param[in]	offset		Begin offset.
     * @param[in]	size		File size.
     * @param[in]	vfs			VFS.
     */
    PackedFileReader(const QString &            path,
                     ::std::unique_ptr<QFile>   file,
                     quint64                    offset,
                     quint64                    size,
                     ::std::shared_ptr<GameVFS> vfs);

    /**
     * @brief		Read file.
     *
     * @param[out]	buffer		Buffer to restore data read.
     * @param[in]	size		Size of buffer.
     *
     * @return		Size read.
     */
    virtual qint64 read(void *buffer, quint64 size) override;

    /**
     * @brief		Read file.
     *
     * @param[in]	size		Size to read.
     *
     * @return		Data read.
     */
    virtual QByteArray read(quint64 size) override;

    /**
     * @brief		Read all data after corrent position in the file.
     *
     * @return		Data read.
     */
    virtual QByteArray readAll() override;

    /**
     * @brief		Seek file.
     *
     * @param[in]	offset		Offset to seek.
     * @param[in]	whence		Where to begin.
     *
     * @return		Current position.
     */
    virtual qint64 seek(qint64 offset,
                        Whence whence = Whence::Current) override;

    /**
     * @brief		Check if current position is at the end of current file.
     *
     * @return		If current position reachs the end of the file, true is
     *				returned. Otherwise returns false.
     */
    virtual bool atEnd() override;

    /**
     * @brief	Destructor.
     */
    virtual ~PackedFileReader();
};

/**
 * @brief	FileReader for normal files.
 */
class GameVFS::NormalFileReader : public GameVFS::FileReader
{
  protected:
    ::std::unique_ptr<QFile> m_file; ///< File object.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     * @param[in]	file		Qt file object for data file.
     * @param[in]	vfs			VFS.
     */
    NormalFileReader(const QString &            path,
                     ::std::unique_ptr<QFile>   file,
                     ::std::shared_ptr<GameVFS> vfs);

    /**
     * @brief		Read file.
     *
     * @param[out]	buffer		Buffer to restore data read.
     * @param[in]	size		Size of buffer.
     *
     * @return		Size read.
     */
    virtual qint64 read(void *buffer, quint64 size) override;

    /**
     * @brief		Read file.
     *
     * @param[in]	size		Size to read.
     *
     * @return		Data read.
     */
    virtual QByteArray read(quint64 size) override;

    /**
     * @brief		Read all data after corrent position in the file.
     *
     * @return		Data read.
     */
    virtual QByteArray readAll() override;

    /**
     * @brief		Seek file.
     *
     * @param[in]	offset		Offset to seek.
     * @param[in]	whence		Where to begin.
     *
     * @return		Current position.
     */
    virtual qint64 seek(qint64 offset,
                        Whence whence = Whence::Current) override;

    /**
     * @brief		Check if current position is at the end of current file.
     *
     * @return		If current position reachs the end of the file, true is
     *				returned. Otherwise returns false.
     */
    virtual bool atEnd() override;

    /**
     * @brief	Destructor.
     */
    virtual ~NormalFileReader();
};

/**
 * @brief	Directory reader.
 */
class GameVFS::DirReader
{
  public:
    /**
     * @brief	Type of directory entery.
     */
    enum EntryType
    {
        File,     ///< Normal file.
        Directory ///< Directory.
    };

    /**
     * @brief	Directory entery.
     */
    struct DirEntry {
        QString   name; ///< Name.
        EntryType type; ///< Type.
    };

    /**
     * @brief	Iterator.
     */
    class Iterator;

    typedef Iterator iterator;

  protected:
    QStringList                          m_path;     ///< Path of the file.
    QString                              m_name;     ///< Name of the file.
    ::std::shared_ptr<GameVFS>           m_vfs;      ///< VFS.
    ::std::shared_ptr<QVector<DirEntry>> m_enteries; ///< Enteries.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of directory.
     * @param[in]	entry		Dat file entery. If not found, set it to
     *							nullptr.
     * @param[in]	vfs			VFS.
     */
    DirReader(const QString &                  path,
              ::std::shared_ptr<DatFileEntery> entry,
              ::std::shared_ptr<GameVFS>       vfs);

    /**
     * @brief	Get name of the directory.
     *
     * @return	Name of the directory.
     */
    const QString &name() const;

    /**
     * @brief	Get path of the directory.
     *
     * @return	Path.
     */
    QString path() const;

    /**
     * @brief		Convert relative path to absolute path.
     *
     * @param[in]	path	Relative path.
     *
     * @return		Absolute path.
     */
    QString absPath(const QString &path) const;

    /**
     * @brief		Get iterator point to the first entery.
     *
     * @return		Iterator.
     */
    iterator begin();

    /**
     * @brief		Get iterator point to the end.
     *
     * @return		Iterator.
     */
    iterator end();

    /**
     * @brief		Get number of files in the directory.
     *
     * @return		Number.
     */
    quint64 count();

    /**
     * @brief		Destructor.
     */
    ~DirReader();
};

/**
 * @brief	Iterator.
 */
class GameVFS::DirReader::Iterator
{
  private:
    ::std::shared_ptr<QVector<DirEntry>> m_enteries; ///< Enteries.
    QVector<DirEntry>::iterator          m_iterator; ///< Iterator.

  public:
    /**
     * @brief		Constructor.
     */
    Iterator();

    /**
     * @brief		Constructor.
     *
     * @param[in]	enteries		Directory enteries.
     * @param[in]	iter			Iterator.
     */
    Iterator(::std::shared_ptr<QVector<DirEntry>> enteries,
             QVector<DirEntry>::iterator          iter);

    /**
     * @brief		Constructor.
     *
     * @param[in]	iter			Iterator to copy..
     */
    Iterator(const Iterator &iter);

    /**
     * @brief		Destructor.
     */
    virtual ~Iterator();

    /**
     * @defgroup	Operators.
     * @{
     */
    const DirEntry *operator->() const;
    const DirEntry &operator*() const;

    Iterator &operator++();
    Iterator  operator++(int);

    Iterator &operator--();
    Iterator  operator--(int);

    Iterator operator+(int n) const;

    Iterator operator-(int n) const;
    int      operator-(const Iterator &iter) const;

    Iterator &operator=(const Iterator &iter);

    Iterator &operator+=(int n);
    Iterator &operator-=(int n);

    bool operator>(const Iterator &iter) const;
    bool operator>=(const Iterator &iter) const;

    bool operator<(const Iterator &iter) const;
    bool operator<=(const Iterator &iter) const;

    bool operator==(const Iterator &iter) const;
    bool operator!=(const Iterator &iter) const;
    /**
     * @}
     */
};
