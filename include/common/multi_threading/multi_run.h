#pragma once

#include <functional>

#include <QtCore/QThread>
#include <QtCore/QVector>

/**
 * @brief	Thread to run task.
 */
class MultiRunThread : public QThread
{
    Q_OBJECT
  private:
    ::std::function<void()> m_task; ///< Tash to run.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	task			Tash to run.
     * @param[in]	parent			Parent object.
     */
    MultiRunThread(::std::function<void()> task, QObject *parent);

    /**
     * @brief	Destructor.
     */
    virtual ~MultiRunThread();

  protected:
    /**
     * @brief	Thread functiuon
     */
    virtual void run() override;
};

/**
 * @brief	Run task in multiple threads.
 */
class MultiRun : public QObject
{
    Q_OBJECT
  private:
    QVector<MultiRunThread *> m_threads; ///< Threads.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	task			Task to run.
     */
    MultiRun(::std::function<void()> task);

    /**
     * @brief		Run task.
     *
     * @param[in]	signleThread	True if run in signle thread.
     */
    void run(bool signleThread = false);

    /**
     * @brief	Destructor.
     */
    virtual ~MultiRun();
};
