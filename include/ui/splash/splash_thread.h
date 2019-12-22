#pragma once

#include <functional>

#include <QtCore/QThread>

class SplashWidget;
class SplashThread : public QThread {
    Q_OBJECT
  private:
    ::std::function<int()> m_workFunc; //< Work to do.
    int                    m_exitCode; //< Exit code.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	workFunc		Work.
     * @param[in]	parent			Parent.
     */
    SplashThread(::std::function<int()> workFunc, SplashWidget *parent);

    /**
     * @breif		Get exit code.
     *
     * @return		Exit code.
     */
    int exitCode();

    /**
     * @brief		Destructor.
     */
    virtual ~SplashThread();

  protected:
    /**
     * @brief		Thread function..
     */
    virtual void run();

  signals:
    /**
     * @brief	Work finished.
     */
    void finished();
};

#include <ui/splash/splash_widget.h>
