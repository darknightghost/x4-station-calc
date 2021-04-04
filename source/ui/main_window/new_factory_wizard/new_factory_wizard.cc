#include <QtCore/QEventLoop>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizard::NewFactoryWizard(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::Dialog);
    this->setWindowTitle(STR("STR_TITLE_NEW_FACTORY_WIZARD"));

    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);

    // Client area.
    m_clientArea = new QScrollArea(this);
    m_layout->addWidget(m_clientArea);

    // Buttons.
    m_layoutButton = new QHBoxLayout();
    m_layout->addLayout(m_layoutButton);

    m_layoutButton->addStretch();

    m_btnBack = new QPushButton(STR("STR_BTN_BACK"));
    m_layoutButton->addWidget(m_btnBack);
    m_btnBack->setEnabled(false);
    this->connect(m_btnBack, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnBackClicked);

    m_btnNext = new QPushButton(STR("STR_BTN_NEXT"));
    m_layoutButton->addWidget(m_btnNext);
    this->connect(m_btnNext, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnNextClicked);

    m_btnFinish = new QPushButton(STR("STR_BTN_FINISH"));
    m_layoutButton->addWidget(m_btnFinish);
    m_btnFinish->setVisible(false);
    this->connect(m_btnFinish, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnFinishClicked);

    m_btnCancel = new QPushButton(STR("STR_BTN_CANCEL"));
    m_layoutButton->addWidget(m_btnCancel);
    this->connect(m_btnCancel, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnCancelClicked);

    QRect windowRect = QApplication::desktop()->geometry();

    windowRect.setWidth(windowRect.width() / 4 * 3);
    windowRect.setHeight(windowRect.height() / 4 * 3);
    windowRect.setX(windowRect.width() / 8);
    windowRect.setY(windowRect.height() / 8);

    this->setFixedSize(windowRect.size());
}

/**
 * @brief       Run wizard.
 */
::std::shared_ptr<Save> NewFactoryWizard::exec()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->show();
    QEventLoop *eventLoop = new QEventLoop(this);
    eventLoop->exec();

    return m_result;
}

/**
 * @brief       Show wizard.
 */
::std::shared_ptr<Save> NewFactoryWizard::showWizard(QWidget *parent)
{
    NewFactoryWizard *wizard = new NewFactoryWizard(parent);
    auto              ret    = wizard->exec();
    delete wizard;
    return ret;
}

/**
 * @brief       Destructor.
 */
NewFactoryWizard::~NewFactoryWizard() {}

/**
 * @brief   On button "Back" clicked.
 */
void NewFactoryWizard::onBtnBackClicked() {}

/**
 * @brief   On button "Next" clicked.
 */
void NewFactoryWizard::onBtnNextClicked() {}

/**
 * @brief   On button "Finish" clicked.
 */
void NewFactoryWizard::onBtnFinishClicked() {}

/**
 * @brief   On button "Cancel" clicked.
 */
void NewFactoryWizard::onBtnCancelClicked()
{
    this->close();
}
