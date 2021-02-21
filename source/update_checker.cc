#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QRegularExpression>
#include <QtCore/QVersionNumber>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QMessageBox>

#include <common/auto_release.h>
#include <locale/string_table.h>
#include <update_checker.h>
#include <version.h>

/// Check update URL.
const QString UpdateChecker::_checkUpdateURL
    = "https://api.github.com/repos/darknightghost/x4-station-calc/releases/"
      "latest";

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
            QMessageBox::critical(m_parent, STR("STR_ERROR"),
                                  STR("STR_CHECKUPDATE_FAILED"));
        }
        return;
    }

    // Parse json.
    // Parse data.
    QJsonParseError jsonError;
    QJsonDocument   doc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse version info : "
                 << jsonError.errorString();
        return;
    }
    QJsonObject root = doc.object();

    // Make version.
    auto iter = root.find("tag_name");
    if (iter == root.end()) {
        qDebug() << "Failed to parse version info : Missing key \"/tag_name\".";
        return;
    }
    if (! iter->isString()) {
        qDebug() << "Failed to parse version info : Key \"/tag_name\" should "
                    "be string.";
        return;
    }

    QString s = iter->toString();
    s.replace(QRegularExpression("[vV\\s\\r\\n]"), "");
    QVersionNumber newestVersion = QVersionNumber::fromString(s);
    qDebug() << "Current version :" << VERSION
             << ", Newest version :" << newestVersion;

    // Compare.
    if (newestVersion > VERSION) {
        if (QMessageBox::question(m_parent, STR("STR_TITLE_CHECKUPDATE"),
                                  STR("STR_NEW_VERSION_FOUND"))
            == QMessageBox::StandardButton::Yes) {
            iter = root.find("html_url");
            if (iter == root.end()) {
                qDebug() << "Failed to parse download url info : Missing key "
                            "\"/html_url\".";
                return;
            }
            if (! iter->isString()) {
                qDebug() << "Failed to parse download url : Key \"/html_url\" "
                            "should be string.";
                return;
            }

            QString downloadURL = iter->toString();
            qDebug() << "Download url : " << downloadURL;
            QDesktopServices::openUrl(QUrl(downloadURL));
        }

        return;
    } else {
        if (! m_quiet) {
            QMessageBox::information(m_parent, STR("STR_INFO"),
                                     STR("STR_NEW_VERSION_NOT_FOUND"));
        }
        return;
    }
}
