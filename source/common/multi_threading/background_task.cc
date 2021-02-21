#include <QtCore/QMutexLocker>

#include <common/multi_threading/background_task.h>

/**
 * @brief		Constructor.
 */
BackgroundTaskThread::BackgroundTaskThread(::std::function<void()> task,
                                           QObject *               parent) :
    QThread(parent),
    m_task(::std::move(task))
{}

/**
 * @brief		Destructor.
 */
BackgroundTaskThread::~BackgroundTaskThread() {}

/**
 * @brief		Thread function.
 */
void BackgroundTaskThread::run()
{
    m_task();
    emit this->finished(this);
}

/**
 * @brief		Constructor.
 */
BackgroundTask::BackgroundTask(RunType runType, QObject *parent) :
    QObject(parent), m_runType(runType), m_threadCount(0)
{}

/**
 * @brief		Run task.
 */
void BackgroundTask::runTask(::std::function<void()> task)
{
    QMutexLocker locker(&m_lock);

    switch (m_runType) {
        case RunType::Immediately: {
            BackgroundTaskThread *taskThread
                = new BackgroundTaskThread(task, this);
            this->connect(taskThread, &BackgroundTaskThread::finished, this,
                          &BackgroundTask::onTaskFinished,
                          Qt::ConnectionType::QueuedConnection);
            taskThread->start();
            return;
        } break;

        case RunType::Queued:
            m_taskQueue.push_back(::std::move(task));
            break;

        case RunType::Newest:
            m_newestTask = ::std::move(task);
            break;
    }

    if (m_threadCount == 0) {
        this->nextTask();
    }
}

/**
 * @brief		Cancel all tasks whitch are not running..
 */
void BackgroundTask::cancle()
{
    QMutexLocker locker(&m_lock);

    bool hasTask = false;
    switch (m_runType) {
        case RunType::Queued:
            if (! m_taskQueue.empty()) {
                hasTask = true;
                m_taskQueue.clear();
            }
            break;

        case RunType::Newest:
            if (m_newestTask != nullptr) {
                hasTask      = true;
                m_newestTask = nullptr;
            }
            break;

        default:
            return;
    }

    if (hasTask && m_threadCount == 0) {
        emit this->finished();
        m_waitCondition.notify_all();
    }
}

/**
 * @brief		Wait until all tasks has been finished.
 */
void BackgroundTask::wait()
{
    QMutexLocker locker(&m_lock);
    if (m_threadCount > 0
        || (m_runType == RunType::Queued && ! m_taskQueue.empty())
        || (m_runType == RunType::Newest && m_newestTask != nullptr)) {
        m_waitCondition.wait(&m_lock);
    }

    return;
}

/**
 * @brief		Destructor.
 */
BackgroundTask::~BackgroundTask()
{
    this->wait();
}

/**
 * @brief		Call next task.
 */
void BackgroundTask::nextTask()
{
    if (m_threadCount > 0) {
        return;
    }

    switch (m_runType) {
        case RunType::Queued: {
            // Get task.
            if (m_taskQueue.empty()) {
                return;
            }
            ::std::function<void()> task = ::std::move(m_taskQueue.front());
            m_taskQueue.pop_front();

            // Run task
            BackgroundTaskThread *taskThread
                = new BackgroundTaskThread(task, this);
            this->connect(taskThread, &BackgroundTaskThread::finished, this,
                          &BackgroundTask::onTaskFinished,
                          Qt::ConnectionType::QueuedConnection);
            taskThread->start();
        } break;

        case RunType::Newest: {
            // Get task.
            if (m_newestTask == nullptr) {
                return;
            }

            // Run task
            BackgroundTaskThread *taskThread
                = new BackgroundTaskThread(::std::move(m_newestTask), this);
            this->connect(taskThread, &BackgroundTaskThread::finished, this,
                          &BackgroundTask::onTaskFinished,
                          Qt::ConnectionType::QueuedConnection);
            taskThread->start();
        } break;

        default:
            return;
    }

    ++m_threadCount;
}

/**
 * @brief		Called when a task has been finished.
 */
void BackgroundTask::onTaskFinished(BackgroundTaskThread *thread)
{
    QMutexLocker locker(&m_lock);
    delete thread;
    --m_threadCount;

    this->nextTask();

    if (m_threadCount == 0) {
        emit this->finished();
        m_waitCondition.notify_all();
    }
}
