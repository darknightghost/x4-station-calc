#include <thread>

#include <QtCore/QDebug>

#include <common/multi_threading/multi_run.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	task			Tash to run.
 * @param[in]	parent			Parent object.
 */
MultiRunThread::MultiRunThread(::std::function<void()> task, QObject *parent) :
    QThread(parent), m_task(task)
{}

/**
 * @brief	Destructor.
 */
MultiRunThread::~MultiRunThread() {}

/**
 * @brief	Thread functiuon
 */
void MultiRunThread::run()
{
    m_task();
}

/**
 * @brief		Constructor.
 *
 * @param[in]	task			Task to run.
 */
MultiRun::MultiRun(::std::function<void()> task)
{
    size_t threadNum = ::std::thread::hardware_concurrency();
    for (size_t i = 0; i < threadNum; i++) {
        m_threads.push_back(new MultiRunThread(task, this));
    }
}

/**
 * @brief	Run task.
 */
void MultiRun::run()
{
    for (auto &thread : m_threads) {
        thread->start();
    }

    qDebug() << "Multi-threading task started, number of thread is "
             << m_threads.size() << ".";

    for (auto &thread : m_threads) {
        thread->wait();
    }
}

/**
 * @brief	Destructor.
 */
MultiRun::~MultiRun() {}
