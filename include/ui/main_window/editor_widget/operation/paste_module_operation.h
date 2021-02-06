#pragma once

#include <memory>

#include <QtCore/QVector>

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>
#include <ui/main_window/editor_widget/x4sc_module_clipboard_mime_data_builder.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::PasteModuleOperation :
    virtual public EditorWidget::OperationBase<
        EditorWidget::PasteModuleOperation,
        GroupItem *,
        ModuleItem *,
        const X4SCModuleClipboardMimeDataBuilder &,
        EditorWidget *>
{
    CREATE_FUNC(EditorWidget::PasteModuleOperation,
                GroupItem *,
                ModuleItem *,
                const X4SCModuleClipboardMimeDataBuilder &,
                EditorWidget *);

  private:
    /**
     * @brief       Module info.
     */
    struct ModuleInfo {
        QString macro;  ///< Macro of the module.
        quint64 amount; ///< Amount of the module.
    };

    /**
     * @brief       Group info.
     */
    struct GroupInfo {
        QString name; ///< Name of the group.
        QVector<::std::shared_ptr<ModuleInfo>>
            modules; ///< Modules in the group.
    };

  private:
    int m_groupIndex;       ///< Index of the group to paste modules.
    int m_firstModuleIndex; ///< Index of first module to paste.
    QVector<::std::shared_ptr<ModuleInfo>> m_modules; ///< Modules to paste.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groupToPaste    Group to paste data.
     * @param[in]	moduleAfter     Moudle to paste modules after.
     * @param[in]	builder         Mime data builder
     * @param[in]	editorWidget    Editor widget.
     */
    PasteModuleOperation(GroupItem *                               groupToPaste,
                         ModuleItem *                              moduleAfter,
                         const X4SCModuleClipboardMimeDataBuilder &builder,
                         EditorWidget *editorWidget);

  public:
    /**
     * @brief	Do operation.
     *
     * @return	On success, the method will return \c true, otherwise returns
     *			\c false.
     */
    virtual bool doOperation() override;

    /**
     * @brief	Undo operation.
     */
    virtual void undoOperation() override;

    /**
     * @brief	Destructor.
     */
    virtual ~PasteModuleOperation();
};
