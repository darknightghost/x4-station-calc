#pragma once

#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    /**
     * @brief		Constructor of main window.
     */
    MainWindow();

    /**
     * @brief		Destructor of main window.
     */
    virtual ~MainWindow();

  protected slots:
    /**
     * @brief		Open file.
     *
     * @param[in]	path		Path of file.
     */
    void open(QString path);
};
