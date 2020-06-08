#include <ui/main_window/editor_widget/editor_widget.h>

/**
 * @brief		Constructor.
 */
EditorWidget::EditorWidget(::std::shared_ptr<Save>  save,
                           MainWindow::EditActions *editActions)
{}

/**
 * @brief	Destructors.
 */
EditorWidget::~EditorWidget() {}

/**
 * @brief	Close save file.
 */
bool EditorWidget::closeSave() {}

/**
 * @brief		Close event.
 */
void EditorWidget::closeEvent(QCloseEvent *event) {}

/**
 * @brief		Save.
 */
void EditorWidget::save() {}

/**
 * @brief		Save as.
 */
void EditorWidget::saveAs() {}

/**
 * @brief	Clear all warning informations.
 */
void EditorWidget::clearInfo() {}

/**
 * @brief		Show error information.
 */
void EditorWidget::addWarning(QString id) {}
