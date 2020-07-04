#pragma once

#include <memory>

#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <common/generic_string.h>
#include <common/multi_threading.h>
#include <save/save.h>
#include <ui/main_window/editor_widget/operation.h>
#include <ui/main_window/editor_widget/warning_widget.h>
#include <ui/main_window/main_window.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QWidget {
    Q_OBJECT;

  private:
    ::std::shared_ptr<Save>  m_save;            ///< Save file.
    bool                     m_dirty;           ///< Dirty flag.
    MainWindow::EditActions *m_editActions;     ///< Edit actions.
    BackgroundTask *         m_backgroundTasks; ///< Background tasks.

    QVBoxLayout *m_layout; ///< Layout.

    QVector<WarningWidget *> m_widgetsWarningInfos; ///< Warning informations.
    QTreeWidget *            m_treeEditor;          ///< Editor.

    // Operation stack.
    QVector<::std::shared_ptr<Operation>>
        m_operationStack; ///< Operation stack.

    // Items
    QTreeWidgetItem *m_itemModules; ///< Station modules.
    QTreeWidgetItem *m_itemSummary; ///< Summary.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	save			Save file.
     * @param[in]	editActions		Edit actions.
     * @param[in]	parent			Parent.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
                 MainWindow::EditActions *editActions,
                 QMdiSubWindow *          parent);

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

    /**
     * @brief	Check save file.
     */
    void checkSave();

  private:
    /**
     * @brief		Do operation.
     *
     * @param[in]	operation		Operation.
     */
    void doOperation(::std::shared_ptr<Operation> operation);

  private:
    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    /**
     * @brief		Create new group.
     */
    void newGroup();

    /**
     * @brief		Undo.
     */
    void undo();

    /**
     * @brief		Redo.
     */
    void redo();

    /**
     * @brief		Cut.
     */
    void cut();

    /**
     * @brief		Copy.
     */
    void copy();

    /**
     * @brief		Paste.
     */
    void paste();

    /**
     * @brief		Remove.
     */
    void remove();

    /**
     * @brief		Save.
     */
    void save();

    /**
     * @brief		Save as.
     */
    void saveAs();

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

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
