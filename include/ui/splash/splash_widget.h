#pragma once

#include <functional>
#include <type_traits>

#include <QtCore/QEventLoop>
#include <QtCore/QMetaType>
#include <QtCore/QMutex>
#include <QtCore/QReadWriteLock>
#include <QtCore/QTimer>
#include <QtCore/QWaitCondition>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include <ui/splash/splash_thread.h>

Q_DECLARE_METATYPE(::std::function<void()>);

class SplashWidget : public QWidget {
    Q_OBJECT
  private:
    SplashThread * m_thread;     //< Working thread.
    QStringList    m_text;       //< Text.
    QReadWriteLock m_textLock;   //< Text lock.
    QImage         m_background; //< background.
    QEventLoop *   m_eventLoop;  //< Event loop.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent widget.
     */
    SplashWidget(QWidget *parent = nullptr);

    /**
     * @brief		Start work.
     *
     * @param[in]	workFunc	Work to do.
     *
     * @return		Return value of workFunc.
     */
    int exec(::std::function<int()> workFunc);

    /**
     * @brief		Run function in event loop.
     *
     * @param[in]	func		Function to call.
     * @param[in]	args		Arguments to call.
     *
     * @return		Return value of func.
     */
    template<typename R, typename... Args>
    typename ::std::enable_if<! ::std::is_void<R>::value, R>::type
        callFunc(::std::function<R(Args...)> func, Args... args)
    {
        if (QThread::currentThread() == this->thread()) {
            /// Call directlly in the same thread.
            return func(args...);
        }

        /// Send signal.
        R              ret;
        QMutex         mutex;
        QWaitCondition cond;
        mutex.lock();
        emit this->sigCallFunc(::std::function<void()>([&]() -> void {
            ret = func(args...);
            mutex.lock();
            cond.notify_all();
            mutex.unlock();
        }));
        cond.wait(&mutex);
        mutex.unlock();
        return ret;
    }

    /**
     * @brief	Run function in event loop.
     *
     * @param[in]	func		Function to call.
     * @param[in]	args		Arguments to call.
     */
    template<typename... Args>
    void callFunc(::std::function<void(Args...)> func, Args... args)
    {
        if (QThread::currentThread() == this->thread()) {
            /// Call directlly in the same thread.
            func(args...);
            return;
        }

        /// Send signal.
        QMutex         mutex;
        QWaitCondition cond;
        mutex.lock();
        emit this->sigCallFunc(::std::function<void()>([&]() -> void {
            func(args...);
            mutex.lock();
            cond.notify_all();
            mutex.unlock();
        }));
        cond.wait(&mutex);
        mutex.unlock();
        return;
    }

    /**
     * @brief	Destructor.
     */
    virtual ~SplashWidget();

  signals:
    /**
     * @brief	Signal to call function.
     */
    void sigCallFunc(::std::function<void()>);

  private:
    /**
     * @brief		Slot to run function in eventloop.
     *
     * @param[in]	func	Function to call.
     */
    void onCallFunc(::std::function<void()> func);

  private:
    /**
     * @brief		Paint event.
     *
     * @param[in]	event	Event.
     */
    virtual void paintEvent(QPaintEvent *event) override;

    /**
     * @brief		Close event.
     *
     * @param[in]	event	Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    /**
     * @brief		Set text.
     *
     * @param[in]	text	Text to show.
     */
    void setText(QString text);

  private slots:
    /**
     * @brief		Tell the widget the work has benn finished
     *
     */
    void onFinished();
};
