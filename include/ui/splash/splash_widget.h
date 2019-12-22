#pragma once

#include <functional>

#include <QtCore/QEventLoop>
#include <QtCore/QReadWriteLock>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include <ui/splash/splash_thread.h>

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
     * @brief	Constructor.
     */
    SplashWidget();

    /**
     * @brief		Start work.
     *
     * @param[in]	workFunc	Work to do.
     *
     * @return		Return value of workFunc.
     */
    int exec(::std::function<int()> workFunc);

    /**
     * @brief	Destructor.
     */
    virtual ~SplashWidget();

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
