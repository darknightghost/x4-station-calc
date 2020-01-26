#pragma once

#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QSharedMemory>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <common.h>
#include <design_models/singleton.h>
#include <global.h>
/**
 * @brief   Global informations.
 */
class OpenFileListener : public QObject, public Singleton<OpenFileListener> {
    Q_OBJECT
    SIGNLETON_OBJECT(OpenFileListener)

  protected:
    QQueue<QString>               m_paths;        //< Queue of paths.
    QMutex                        m_pathsLock;    //< Paths lock.
    ::std::shared_ptr<QTcpServer> m_server;       //< TCP server.
    QSharedMemory *               m_sharedMemory; //< Shared memory.
    bool                          m_opened;       //< Opened flag.
    bool                          m_block;        //< Block flag.
    volatile bool                 m_recv;         //< Receive flag.
    SimpleThread<>                m_recvThread;   //< Receive thread;

  protected:
    /**
     * @brief       Constructor.
     */
    OpenFileListener();

  public:
    /**
     * @brief		Return true if an another instance has been opened.
     */
    bool opened();

    /**
     * @brief		Block open signals.
     */
    void block();

    /**
     * @brief		Unblock open signals.
     */
    void unblock();

    /**
     * @brief       Destructor.
     */
    virtual ~OpenFileListener();

  signals:
    /**
     * @brief       Open file signal.
     *
     * @param[in]	path		Path of file.
     */
    void openFile(QString path);

    /**
     * @brief       Active main window signal.
     */
    void active();
};
