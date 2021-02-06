#pragma once

#include <functional>

#include <QtCore/QThread>

/**
 * @brief		A simple thread whitch can be created easily.
 */
class SimpleThread : virtual public QThread
{
  protected:
    ::std::function<void()> m_threadFunc; ///< Thread function.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent.
     */
    SimpleThread(QObject *parent = nullptr) : QThread(parent) {}

    /**
     * @brief		Constructor.
     *
     * @param[in]	func		Thread function.
     * @param[in]	parent		Parent.
     */
    SimpleThread(::std::function<void()> func, QObject *parent = nullptr) :
        QThread(parent)
    {
        m_threadFunc = func;
    }

    /**
     * @brief		Set thread function.
     *
     * @param[in]	func		Thread function.
     */
    void setThreadFunc(::std::function<void()> func)
    {
        m_threadFunc = func;
    }

    /**
     * @brief		Destructor.
     */
    virtual ~SimpleThread() {}

  protected:
    /**
     * @brief		Thread function.
     */
    virtual void run() override
    {
        m_threadFunc();
    }
};
