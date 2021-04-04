#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <save/save.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizard : public QWidget {
    Q_OBJECT

  private:
    QVBoxLayout *m_layout;     ///< Widget layout.
    QScrollArea *m_clientArea; ///< Client area.

    QHBoxLayout *m_layoutButton; ///< Button layout.
    QPushButton *m_btnBack;      ///< Button "Back".
    QPushButton *m_btnNext;      ///< Button "Next".
    QPushButton *m_btnFinish;    ///< Button "Finish".
    QPushButton *m_btnCancel;    ///< Button "Cancel".

    ::std::shared_ptr<Save> m_result; ///< Result.

  private:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    NewFactoryWizard(QWidget *parent);

    /**
     * @brief       Run wizard.
     *
     * @return      If a new save is generated, the method returns the new save.
     *              If not, \c nullptr is returned.
     */
    ::std::shared_ptr<Save> exec();

  public:
    /**
     * @brief       Show wizard.
     *
     * @param[in]   parent      Parent widget.
     *
     * @return      If a new save is generated, the method returns the new save.
     *              If not, \c nullptr is returned.
     */
    static ::std::shared_ptr<Save> showWizard(QWidget *parent = nullptr);

  public:
    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizard();

  private slots:
    /**
     * @brief   On button "Back" clicked.
     */
    void onBtnBackClicked();

    /**
     * @brief   On button "Next" clicked.
     */
    void onBtnNextClicked();

    /**
     * @brief   On button "Finish" clicked.
     */
    void onBtnFinishClicked();

    /**
     * @brief   On button "Cancel" clicked.
     */
    void onBtnCancelClicked();
};
