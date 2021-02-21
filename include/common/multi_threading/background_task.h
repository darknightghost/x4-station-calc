#pragma once

#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

/**
 * @brief	Background task thread.
 */
class BackgroundTaskThread : public QThread {
    Q_OBJECT;

  private:
    ::std::function<void()> m_task; ///< Task function.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	task		Task to run.
     * @param[in]	parent		Parent.
     */
    BackgroundTaskThread(::std::function<void()> task, QObject *parent);

    /**
     * @brief		Destructor.
     */
    virtual ~BackgroundTaskThread();

  private:
    /**
     * @brief		Thread function.
     */
    virtual void run() override;

  signals:
    /**
     * @brief		Emitted when the task has been finished.
     *
     * @param[in]	thread		Current thread.
     */
    void finished(BackgroundTaskThread *thread);
};

/**
 * @brief	Background task.
 */
class BackgroundTask : public QObject {
    Q_OBJECT;

  public:
    /**
     * @brief	How to run tasks.
     */
    enum RunType {
        Immediately, ///< Run the task immetiately.
        Queued,      ///< Put the task in a queu and run it one by one.
        Newest       ///< Run the newest task.
    };

  private:
    RunType        m_runType;       ///< Run type.
    QMutex         m_lock;          ///< Lock.
    quint64        m_threadCount;   ///< Count of task thread.
    QWaitCondition m_waitCondition; ///< Contidional vatiable for \c wait().
    ::std::function<void()>         m_newestTask; ///< Newest task.
    QQueue<::std::function<void()>> m_taskQueue;  ///< Task queue.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	runType		Run type.
     * @param[in]	parent		Parent.
     */
    BackgroundTask(RunType runType, QObject *parent = nullptr);

    /**
     * @brief		Run task.
     *
     * @brief		task		Tash function.
     */
    void runTask(::std::function<void()> task);

    /**
     * @brief		Cancel all tasks whitch are not running..
     */
    void cancle();

    /**
     * @brief		Wait until all tasks has been finished.
     */
    void wait();

    /**
     * @brief		Destructor.
     */
    virtual ~BackgroundTask();

  private:
    /**
     * @brief		Call next task.
     */
    void nextTask();

  private slots:
    /**
     * @brief		Called when a task has been finished.
     *
     * @param[in]	thread		Task thread.
     */
    void onTaskFinished(BackgroundTaskThread *thread);

  signals:
    /**
     * @brief		Emitted when all tasks has been finished.
     */
    void finished();
};
