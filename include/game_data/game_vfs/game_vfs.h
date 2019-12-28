#pragma once

#include <memory>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include <interface/i_is_good.h>

class GameVFS : public IIsGood {
  public:
    /**
     * @brief	Information of cat file.
     *
     */
    struct CatFileInfo {
        QString cat;    //< Name of cat file.
        QString catSig; //< Name of sig file of cat file.
        QString dat;    //< Name of dat file.
        QString datSig; //< Name of sig file of dat file.
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
        QString name;        //< Name.
        bool    isDirectory; //< Directory flag.
        union {
            QMap<QString, DatFileEntery> children; //< Children.
            struct {
                QString datName; //< Name of dat file.
                quint64 offset;  //< Offset.
                quint64 size;    //< Size.
            } FileInfo;          //< File infomation;
        };
    };

  private:
    QString                      m_gamePath;    //< Game path.
    QMap<QString, DatFileEntery> m_datEnteries; //< Enteries..

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gamePath		Path of game.
     * @param[in]	info			Cat files info.
     */
    GameVFS(const QString &gamePath, const QMap<int, CatFileInfo> &info);

    /**
     * @brief		Open file.
     *
     * @param[in]	path		Path of file.
     *
     * @return		On success, a \c FileReader object is returned. Otherwise
     *				returns nullptr.
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
};

/**
 * @brief	FileReader.
 *
 */
class GameVFS::FileReader {
  protected:
    QString m_path; //< Path of the file.
    QString m_name; //< Name of the file.

  public:
    /**
     * @brief	Whence.
     */
    enum Whence {
        Set     = 0x01, //< From begining of the file.
        Current = 0x02, //< From current position of the file.
        End     = 0x03  //< From the end of the file.
    };

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     */
    FileReader(const QString &path);

    /**
     * @brief	Get path of the file.
     *
     * @return	Path of the file.
     */
    const QString &path() const;

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
class GameVFS::PackedFileReader : public GameVFS::FileReader {
  protected:
    QFile   m_file;   //< File object.
    quint64 m_offset; //< Offset.
    quint64 m_size;   //< File size.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     * @param[in]	file		Qt file object for data file.
     * @param[in]	offset		Begin offset.
     * @param[in]	size		File size.
     */
    PackedFileReader(const QString &path,
                     QFile &&       file,
                     quint64        offset,
                     quint64        size);

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
     * @brief	Destructor.
     */
    virtual ~PackedFileReader();
};

/**
 * @brief	FileReader for normal files.
 */
class GameVFS::NormalFileReader : public GameVFS::FileReader {
  protected:
    QFile m_file; //< File object.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of file.
     * @param[in]	file		Qt file object for data file.
     */
    NormalFileReader(const QString &path, QFile &&file);

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
     * @brief	Destructor.
     */
    virtual ~NormalFileReader();
};

/**
 * @brief	Directory reader.
 */
class GameVFS::DirReader {
  protected:
    QString m_path; //< Path of the file.
    QString m_name; //< Name of the file.

  public:
    /**
     * @brief	Type of directory entery.
     */
    enum EntryType {
        File,     //< Normal file.
        Directory //< Directory.
    };

    /**
     * @brief	Directory entery.
     */
    struct DirEntry {
        QString   name; //< Name.
        EntryType type; //< Type.
    };

    /**
     * @brief	Iterator.
     */
    class Iterator;

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of directory.
     */
    DirReader(const QString &path);

    /**
     * @brief		Open file in this directory.
     *
     * @param[in]	path		Path of file.
     *
     * @return		On success, a \c FileReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<FileReader> open(const QString &path) = 0;

    /**
     * @brief		Open sub directory in this directory.
     *
     * @param[in]	path		Path of directory.
     *
     * @return		On success, a \c DirReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<DirReader> openDir(const QString &path) = 0;

    /**
     * @brief		Get iterator point to the first entery.
     *
     * @return		Iterator.
     */
    virtual Iterator begin() = 0;

    /**
     * @brief		Get iterator point to the end.
     *
     * @return		Iterator.
     */
    virtual Iterator end() = 0;

    /**
     * @brief		Destructor.
     */
    virtual ~DirReader();
};

/**
 * @brief	Iterator.
 */
class GameVFS::DirReader::Iterator {
  private:
    ::std::shared_ptr<QVector<DirEntry>> m_enteries; //< Enteries.
    QVector<DirEntry>::iterator          m_iterator; //< Iterator.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	enteries		Directory enteries.
     */
    Iterator(::std::shared_ptr<QVector<DirEntry>> enteries);

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

    /// Operators
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
};

/**
 * @brief	Directory reader for packed files.
 */
class GameVFS::PackedDirReader : public GameVFS::DirReader {
  protected:
    const QMap<QString, DatFileEntery> &m_enteries; //< Enteries.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of directory.
     * @param[in]	enteries	Enteries.
     */
    PackedDirReader(const QString &                     path,
                    const QMap<QString, DatFileEntery> &enteries);

    /**
     * @brief		Open file in this directory.
     *
     * @param[in]	path		Path of file.
     *
     * @return		On success, a \c FileReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<FileReader> open(const QString &path) override;

    /**
     * @brief		Open sub directory in this directory.
     *
     * @param[in]	path		Path of directory.
     *
     * @return		On success, a \c DirReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<DirReader> openDir(const QString &path) override;

    /**
     * @brief		Get iterator point to the first entery.
     *
     * @return		Iterator.
     */
    virtual Iterator begin() override;

    /**
     * @brief		Get iterator point to the end.
     *
     * @return		Iterator.
     */
    virtual Iterator end() override;

    /**
     * @brief		Destructor.
     */
    virtual ~PackedDirReader();
};

/**
 * @brief	Directory reader for normal files.
 */
class GameVFS::NormalDirReader : public GameVFS::DirReader {
  protected:
    QDir m_dir; //< Directory object.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	path		Path of directory.
     * @param[in]	dir			Directory object.
     */
    NormalDirReader(const QString &path, QDir &&dir);

    /**
     * @brief		Open file in this directory.
     *
     * @param[in]	path		Path of file.
     *
     * @return		On success, a \c FileReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<FileReader> open(const QString &path) override;

    /**
     * @brief		Open sub directory in this directory.
     *
     * @param[in]	path		Path of directory.
     *
     * @return		On success, a \c DirReader object is returned. Otherwise
     *				returns nullptr.
     */
    virtual ::std::shared_ptr<DirReader> openDir(const QString &path) override;

    /**
     * @brief		Get iterator point to the first entery.
     *
     * @return		Iterator.
     */
    virtual Iterator begin() override;

    /**
     * @brief		Get iterator point to the end.
     *
     * @return		Iterator.
     */
    virtual Iterator end() override;

    /**
     * @brief		Destructor.
     */
    virtual ~NormalDirReader();
};
