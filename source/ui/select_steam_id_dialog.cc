#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QThread>
#include <QtCore/QtGlobal>
#include <QtGui/QResizeEvent>
#include <QtGui/QShowEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <common.h>
#include <config.h>
#include <locale/string_table.h>
#include <ui/select_steam_id_dialog.h>

/**
 * @brief	Constructor.
 */
SelectSteamIDDialog::SelectSteamIDDialog(const QStringList &steam_id3s) :
    QDialog(nullptr)
{
    // Create widgets
    m_layout = new QVBoxLayout();

    // Selections
    // Layout
    m_layoutSelection = new QHBoxLayout();

    // Label
    m_lblSteamID = new QLabel(STR("STR_STEAM_ID"));
    m_layoutSelection->addWidget(m_lblSteamID);

    // Combobox
    m_comboSteamID = new QComboBox();
    m_layoutSelection->addWidget(m_comboSteamID);
    QStringList steam_id64s;
    int         minimumWidth = m_comboSteamID->minimumWidth();
    bool        defaultFound = false;
    QString     defaultID    = Config::instance()->getString("/steamID3", "");
    for (auto &id3 : steam_id3s) {
        QString id64 = QString("7656%1").arg(id3.toULong() + 1197960265728);
        minimumWidth
            = max(minimumWidth,
                  m_comboSteamID->fontMetrics().boundingRect(id64).width());
        m_comboSteamID->addItem(id64);
        if (id3 == defaultID) {
            defaultFound = true;
            m_comboSteamID->setCurrentIndex(m_comboSteamID->count() - 1);
        }
    }
    if (! defaultFound) {
        m_comboSteamID->setCurrentIndex(0);
    }
    m_comboSteamID->setMinimumWidth(minimumWidth);

    m_layout->addLayout(m_layoutSelection);

    // Buttons
    // Layout
    m_layoutButton = new QHBoxLayout();

    // Button OK
    m_layoutButton->addStretch();
    m_btnOK = new QPushButton(STR("STR_BTN_OK"));
    this->connect(m_btnOK, &QPushButton::clicked, this, &QDialog::accept);
    m_layoutButton->addWidget(m_btnOK);

    m_layoutButton->addStretch();
    m_layout->addLayout(m_layoutButton);

    // Set layout
    this->setLayout(m_layout);
    this->setWindowTitle(STR("STR_TITLE_SELECT_STEAM_ID"));

    // Window size and position.
    m_layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    QDesktopWidget *desktop = QApplication::desktop();
    QSize           sz      = m_layout->sizeHint();
    this->setGeometry((desktop->width() - sz.width()) / 2,
                      (desktop->height() - sz.height()) / 2, sz.width(),
                      sz.height());
}

/**
 * @brief       Get selected steam ID.
 */
const QString &SelectSteamIDDialog::selected() const
{
    return m_selected;
}

/**
 * @brief	Destructor.
 */
SelectSteamIDDialog::~SelectSteamIDDialog() {}

/**
 * @brief       Wrapper of exec().
 */
int SelectSteamIDDialog::exec()
{
    int ret = QDialog::exec();

    QString id = m_comboSteamID->currentText();
    id.replace(0, 4, "");
    m_selected = QString("%1").arg(id.toULong() - 1197960265728);

    return ret;
}

/**
 * @brief       Show the dialog and get the result.
 */
QString SelectSteamIDDialog::selectSteamIDs(const QStringList &steam_id3s)
{
    ::std::unique_ptr<SelectSteamIDDialog> dialog(
        new SelectSteamIDDialog(steam_id3s));
    dialog->exec();

    QString ret = dialog->selected();
    return ret;
}

/**
 * @brief       Close event handler.
 */
void SelectSteamIDDialog::closeEvent(QCloseEvent *e)
{
    QString id = m_comboSteamID->currentText();
    id.replace(0, 4, "");
    m_selected = QString("%1").arg(id.toULong() - 1197960265728);
    QDialog::closeEvent(e);
}
