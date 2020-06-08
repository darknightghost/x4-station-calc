#pragma once

#include <memory>

#include <QtGui/QCloseEvent>
#include <QtWidgets/QMdiSubWindow>

#include <common/generic_string.h>
#include <save/save.h>
#include <ui/main_window/main_window.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QMdiSubWindow {
    Q_OBJECT;

  private:
    ::std::shared_ptr<Save>  m_save;        ///< Save file.
    bool                     m_dirty;       ///< Dirty flag.
    MainWindow::EditActions *m_editActions; ///< Edit actions.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	save			Save file.
     * @param[in]	editActions		Edit actions.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
                 MainWindow::EditActions *editActions);

    /**
     * @brief	Destructors.
     */
    virtual ~EditorWidget();

  public:
    /**
     * @brief	Close save file.
     *
     * @return	\c true if the file closed, otherwise returns false.
     */
    bool closeSave();

  private:
    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    /**
     * @brief		Save.
     */
    void save();

    /**
     * @brief		Save as.
     */
    void saveAs();

  private slots:
    /**
     * @brief	Clear all warning informations.
     */
    void clearInfo();

    /**
     * @brief		Show error information.
     *
     * @param[in]	id		String id.
     */
    void addWarning(QString id);
};
