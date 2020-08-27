#include <QtCore/QDebug>
#include <QtCore/QWaitCondition>
#include <QtWidgets/QMessageBox>

#include <locale/string_table.h>
#include <open_file_listener.h>

#define KEY "X4_station_editor"

/**
 * @brief       Constructor.
 */
OpenFileListener::OpenFileListener() : QObject(), m_opened(false)
{
    bool firstTime = false;

    // Create shared memory.
    m_sharedMemory = new QSharedMemory(KEY, this);
    if (m_sharedMemory->create(sizeof(quint16))) {
        firstTime = true;
    } else {
        if (! m_sharedMemory->attach()) {
            QMessageBox::critical(nullptr, STR("STR_ERROR"),
                                  STR("STR_FAILED_OPEN_SHARED_MEMORY"));
            qDebug() << "Failed to open shared memory.";
            return;
        }
    }

    m_sharedMemory->lock();

    if (! firstTime) {
        // Try to connect.
        ::std::shared_ptr<QTcpSocket> s(new QTcpSocket(this));
        s->connectToHost(QHostAddress::LocalHost,
                         *(quint16 *)(m_sharedMemory->data()));
        if (s->waitForConnected(30000)) {
            // Connected
            m_opened = true;
            if (Global::instance()->hasFileToOpen()) {
                s->write(Global::instance()->fileToOpen().toUtf8());
                s->flush();
                qDebug() << "Path sent to port "
                         << *(quint16 *)(m_sharedMemory->data()) << ".";
            }
            m_sharedMemory->unlock();
            this->setInitialized();
            return;
        } else {
            firstTime = true;
        }
    }

    // Create server
    QMutex         serverCreateMutex;
    QWaitCondition serverCreateCond;
    volatile bool  serverCreateFlag = false;

    m_recvThread.setThreadFunc([this, &serverCreateMutex, &serverCreateCond,
                                &serverCreateFlag]() -> void {
        // Create server.
        serverCreateMutex.lock();
        m_server = ::std::shared_ptr<QTcpServer>(new QTcpServer());
        if (! m_server->listen(QHostAddress::LocalHost)) {
            serverCreateFlag = false;
            serverCreateCond.notify_all();
            serverCreateMutex.unlock();
            return;
        }
        *(quint16 *)(m_sharedMemory->data()) = m_server->serverPort();
        m_recv                               = true;
        m_block                              = true;
        qDebug() << "Listening, port = " << m_server->serverPort() << ".";

        serverCreateFlag = true;
        serverCreateCond.notify_all();
        serverCreateMutex.unlock();

        if (Global::instance()->hasFileToOpen()) {
            m_paths.push_back(Global::instance()->fileToOpen());
        }

        // Listen.
        while (m_recv) {
            if (m_server->waitForNewConnection(500)) {
                QTcpSocket *s = m_server->nextPendingConnection();
                s->waitForDisconnected();
                QByteArray data = s->readAll();
                m_paths.push_back(data);
                m_pathsLock.lock();
                if (! m_block) {
                    while (! m_paths.empty()) {
                        if (m_paths.front() == "") {
                            emit this->active();
                        } else {
                            emit this->openFile(m_paths.front());
                        }
                        m_paths.pop_front();
                    }
                }
                m_pathsLock.unlock();
                delete s;
            }
        }
    });

    serverCreateMutex.lock();
    m_recvThread.start();
    serverCreateCond.wait(&serverCreateMutex);
    serverCreateMutex.unlock();
    if (! serverCreateFlag) {
        m_sharedMemory->unlock();
        QMessageBox::critical(nullptr, STR("STR_ERROR"),
                              STR("STR_FAILED_LISTEN_SOCKET"));
        qDebug() << "Failed to listen.";
        return;
    }

    m_sharedMemory->unlock();
    this->setInitialized();
    return;
}

/**
 * @brief		Return true if an another instance has been opened.
 */
bool OpenFileListener::opened()
{
    return m_opened;
}

/**
 * @brief		Block open signals.
 */
void OpenFileListener::block()
{
    if (m_opened) {
        return;
    }
    m_block = true;
}

/**
 * @brief		Unblock open signals.
 */
void OpenFileListener::unblock()
{
    if (m_opened) {
        return;
    }
    if (m_block) {
        m_block = false;
        m_pathsLock.lock();
        while (! m_paths.empty()) {
            emit this->openFile(m_paths.front());
            m_paths.pop_front();
        }
        m_pathsLock.unlock();
    }
}

/**
 * @brief       Destructor.
 */
OpenFileListener::~OpenFileListener()
{
    if (m_recvThread.isRunning()) {
        m_recv = false;
        m_recvThread.wait();
    }
}
