#pragma once

#include <memory>

#include <QtCore/QVector>

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>
#include <ui/main_window/editor_widget/x4sc_group_clipboard_mime_data_builder.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::PasteGroupOperation :
    virtual public EditorWidget::OperationBase<
        EditorWidget::PasteGroupOperation,
        GroupItem *,
        const X4SCGroupClipboardMimeDataBuilder &,
        EditorWidget *> {
    CREATE_FUNC(EditorWidget::PasteGroupOperation,
                GroupItem *,
                const X4SCGroupClipboardMimeDataBuilder &,
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
    int m_firstGroupIndex; ///< Index of first group to paste.
    QVector<::std::shared_ptr<GroupInfo>> m_groups; ///< Groups to paste.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groupAfter      Group to paste groups after.
     * @param[in]	builder         Mime data builder
     * @param[in]	editorWidget    Editor widget.
     */
    PasteGroupOperation(GroupItem *                              groupAfter,
                        const X4SCGroupClipboardMimeDataBuilder &builder,
                        EditorWidget *                           editorWidget);

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
    virtual ~PasteGroupOperation();
};
