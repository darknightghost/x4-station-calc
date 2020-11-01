#include <QtCore/QRegularExpression>
#include <QtCore/QVersionNumber>
#include <QtGui/QDesktopServices>

#include <common/auto_release.h>
#include <locale/string_table.h>
#include <ui/customized_widgets/customized_message_box.h>
#include <update_checker.h>
#include <version.h>

/// Check update URL.
const QString UpdateChecker::_checkUpdateURL
    = "https://raw.githubusercontent.com/darknightghost/"
      "darknightghost.github.io/master/x4-station-calc/version";

/// Download URL.
#if defined(OS_WINDOWS)
const QString UpdateChecker::_downloadURL
    = "https://github.com/darknightghost/darknightghost.github.io/raw/master/"
      "x4-station-calc/v%1.7z";
#else
const QString UpdateChecker::_downloadURL
    = "https://github.com/darknightghost/x4-station-calc/releases/tag/v%1";

#endif

/**
 * @brief       Constructor.
 */
UpdateChecker::UpdateChecker(QWidget *parent) :
    QObject(parent), m_parent(parent), m_request(QUrl(_checkUpdateURL)),
    m_checkingFlag(false)
{
    // Create manager.
    m_netwokAccessManager = new QNetworkAccessManager(this);

    // Enable TLS.
    QSslConfiguration sslConfig = m_request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1SslV3);
    m_request.setSslConfiguration(sslConfig);

    this->connect(m_netwokAccessManager, &QNetworkAccessManager::finished, this,
                  &UpdateChecker::onRequestFinished);
}

/**
 * @brief       Check update.
 */
void UpdateChecker::checkUpdate(bool quiet)
{
    // Check if previous request has been finished.
    bool checkingFlag = true;
    checkingFlag      = m_checkingFlag.exchange(checkingFlag);
    if (checkingFlag) {
        return;
    }

    // Make request.
    m_quiet = quiet;
    m_netwokAccessManager->get(m_request);
}

/**
 * @brief       Destructor.
 */
UpdateChecker::~UpdateChecker() {}

/**
 * @brief       Called when check update request finished.
 *
 * @param[in]   reply       Reply.
 */
void UpdateChecker::onRequestFinished(QNetworkReply *reply)
{
    AutoRelease<::std::atomic<bool> &> unlocker(
        m_checkingFlag, [](::std::atomic<bool> &flag) -> void {
            flag = false;
        });

    // Check result.
    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NetworkError::NoError) {
        qDebug() << err;
        if (! m_quiet) {
            CustomizedMessageBox::critical(m_parent, STR("STR_ERROR"),
                                           STR("STR_CHECKUPDATE_FAILED"));
        }
        return;
    }

    // Make version.
    QString s = QString::fromUtf8(reply->readAll());
    s.replace(QRegularExpression("[\\s\\r\\n]"), "");
    QVersionNumber newestVersion = QVersionNumber::fromString(s);
    qDebug() << "Current version :" << VERSION
             << ", Newest version :" << newestVersion;

    // Compare.
    if (newestVersion > VERSION) {
        if (CustomizedMessageBox::question(m_parent,
                                           STR("STR_TITLE_CHECKUPDATE"),
                                           STR("STR_NEW_VERSION_FOUND"))
            == QMessageBox::StandardButton::Yes) {
            QString downloadURL = _downloadURL.arg(s);
            qDebug() << "Download url : " << downloadURL;
            QDesktopServices::openUrl(QUrl(downloadURL));
        }

        return;
    } else {
        if (! m_quiet) {
            CustomizedMessageBox::information(m_parent, STR("STR_INFO"),
                                              STR("STR_NEW_VERSION_NOT_FOUND"));
        }
        return;
    }
}
