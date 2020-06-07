#pragma once

#include <functional>

#include <QtCore/QMap>
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
        Race,         ///< Race.
        Ware,         ///< Ware.
        StationModule ///< Station module.
    };

    /**
     * @brief	History.
     */
    struct History {
        HistoryType type;  ///< History type.
        QString     macro; ///< Macro of the object.
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
     * @param[in]	macro			Ware ID.
     * @param[in]	keepPrevious	Keep previous history.
     */
    void showWareInfo(QString macro, bool keepPrevious = false);

    /**
     * @brief		Show information of race.
     *
     * @param[in]	macro			race ID.
     * @param[in]	keepPrevious	Keep previous history.
     */
    void showRaceInfo(QString macro, bool keepPrevious = false);

  private:
    /**
     * @brief		Update.
     */
    void update();

    /**
     * @brief		Update information of station module.
     *
     * @param[in]	macro		Macro of station module.
     */
    void updateModule(const QString &macro);

    /**
     * @brief		Update information of ware.
     *
     * @param[in]	macro		Macro of ware.
     */
    void updateWare(const QString &macro);

    /**
     * @brief		Update information of race.
     *
     * @param[in]	macro		Macro of race.
     */
    void updateRace(const QString &macro);

  private slots:
    /**
     * @brief		On item double clicked.
     *
     * @param[in]	item	Item clicked.
     * @param[in]	column	Column clicked.
     */
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

    /**
     * @brief	On button back clicked.
     */
    void onBtnBackClicked();

    /**
     * @brief	On button forward clicked.
     */
    void onBtnForwardClicked();
};
