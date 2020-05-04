#pragma once

#include <functional>

#include <QtCore/QThread>

/**
 * @brief		A simple thread whitch can be created easily.
 *
 * @tparam		Args	Types of the arguments of the thread function.
 */
template<typename... Args>
class SimpleThread : virtual public QThread {
  protected:
    ::std::function<void()> m_threadFunc; ///< Thread function.

  public:
    /**
     * @brief		Constructor.
     */
    SimpleThread() : QThread(nullptr) {}

    /**
     * @brief		Constructor.
     *
     * @param[in]	func		Thread function.
     * @param[in]	args		Arguments.
     */
    SimpleThread(::std::function<void(Args...)> func, Args... args) :
        QThread(nullptr)
    {
        m_threadFunc = ::std::bind(func, args...);
    }

    /**
     * @brief		Set thread function.
     *
     * @param[in]	func		Thread function.
     * @param[in]	args		Arguments.
     */
    void setThreadFunc(::std::function<void(Args...)> func, Args... args)
    {
        m_threadFunc = ::std::bind(func, args...);
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
