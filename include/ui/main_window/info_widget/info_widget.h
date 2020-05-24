#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include <ui/main_window/action_control_dock_widget.h>
#include <ui/main_window/info_widget/square_button.h>

/**
 * @brief	Information widget.
 */
class InfoWidget : public ActionControlDockWidget {
    Q_OBJECT

    /**
     * @brief	History type.
     */
    enum HistoryType {
        Ware,         ///< Ware.
        StationModule ///< Station module.
    };

    /**
     * @brief	History.
     */
    struct History {
        HistoryType type; ///< History type.
        QString     id;   ///< ID/macro of the object.
    };

  protected:
    QWidget *     m_widget;     ///< Widget.
    QGridLayout * m_layout;     ///< Layout.
    SquareButton *m_btnBack;    ///< Button back.
    SquareButton *m_btnForward; ///< Button forward.
    QTreeWidget * m_treeInfo;   ///< Information tree widget.

    QVector<History> m_history;      ///< History.
    int              m_historyIndex; ///< Index.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	statusAction	Action to control the visibility of the
     *								widget.
     * @param[in]	parent			Parent object.
     * @param[in]	flags			Window flags.
     */
    InfoWidget(QAction *       statusAction,
               QWidget *       parent = nullptr,
               Qt::WindowFlags flags  = Qt::WindowFlags());

    /**
     * @brief		Destructor.
     */
    virtual ~InfoWidget();

  public slots:
    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief		Show information of station module.
     *
     * @param[in]	macro			Macro of the module.
     * @param[in]	keepPrevious	Keep previous history.
     */
    void showStationModuleInfo(QString macro, bool keepPrevious = false);

    /**
     * @brief		Show information of ware.
     *
     * @param[in]	id				Ware ID.
     * @param[in]	keepPrevious	Keep previous history.
     */
    void showWareInfo(QString id, bool keepPrevious = false);

  private:
    /**
     * @brief		Show information.
     */
    void showInfo();
};
