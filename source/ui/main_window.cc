#include <QtCore/QDebug>

#include <config.h>
#include <locale/string_table.h>
#include <open_file_listener.h>
#include <ui/main_window.h>

/**
 * @brief		Constructor of main window.
 */
MainWindow::MainWindow() : QMainWindow(nullptr)
{
    // Title
    this->setWindowTitle(STR("STR_TITLE"));
    this->connect(OpenFileListener::instance().get(),
                  &OpenFileListener::openFile, this, &MainWindow::open,
                  Qt::ConnectionType::QueuedConnection);
    this->connect(OpenFileListener::instance().get(), &OpenFileListener::active,
                  this, &MainWindow::active,
                  Qt::ConnectionType::QueuedConnection);
    OpenFileListener::instance()->unblock();
}

/**
 * @brief		Destructor of main window.
 */
MainWindow::~MainWindow() {}

/**
 * @brief		Open file.
 *
 * @param[in]	path		Path of file.
 */
void MainWindow::open(QString path)
{
    qDebug() << path;
}

/**
 * @brief		Active window.
 */
void MainWindow::active()
{
    this->activateWindow();
}
