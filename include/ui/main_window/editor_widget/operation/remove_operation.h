#pragma once

#include <memory>

#include <QtCore/QVector>

#include <common/generic_reference.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/operation.h>

/**
 * @brief		Operation to rename modules group.
 */
class EditorWidget::RemoveOperation :
    virtual public EditorWidget::OperationBase<EditorWidget::RemoveOperation,
                                               const QVector<GroupItem *> &,
                                               const QVector<ModuleItem *> &,
                                               EditorWidget *>
{
    CREATE_FUNC(EditorWidget::RemoveOperation,
                const QVector<GroupItem *> &,
                const QVector<ModuleItem *> &,
                EditorWidget *);

  private:
    /**
     * @brief   Module to remove.
     */
    struct ModuleToRemove {
        int     groupIndex;  ///< Index of the group.
        int     moduleIndex; ///< Index of the module.
        QString macro;       ///< Macro of the module.
        int     amount;      ///< Amount.
    };

    /**
     * @brief   Module in group.
     */
    struct ModuleInGroup {
        QString macro;  ///< Macro of the module.
        int     amount; ///< Amount.
    };

    /**
     * @brief   Group to remove.
     */
    struct GroupToRemove {
        int     groupIndex; ///< Index of the group.
        QString name;       ///< Group name.
        bool    expanded;   ///< Expand status.
        QVector<::std::shared_ptr<ModuleInGroup>>
            modules; ///< Modules ion the group.
    };

  private:
    QVector<::std::shared_ptr<GroupToRemove>> m_groups; ///< Groups to remove.
    QVector<::std::shared_ptr<ModuleToRemove>>
        m_modules; ///< Modules to remove.

  private:
    /**
     * @brief		Constructor.
     *
     * @param[in]	groups          Groups to remove.
     * @param[in]	modules         Modules to remove.
     * @param[in]	editorWidget    Editor widget.
     */
    RemoveOperation(const QVector<GroupItem *> & groups,
                    const QVector<ModuleItem *> &modules,
                    EditorWidget *               editorWidget);

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
    virtual ~RemoveOperation();
};
