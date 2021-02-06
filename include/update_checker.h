#pragma once

#include <atomic>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslConfiguration>
#include <QtWidgets/QWidget>

/**
 * @brief       Update checker.
 */
class UpdateChecker : public QObject {
    Q_OBJECT;

  private:
    static const QString _checkUpdateURL; ///< Check update URL.
    static const QString _downloadURL;    ///< Download URL.

  private:
    QWidget *              m_parent;              ///< Parent;
    QNetworkAccessManager *m_netwokAccessManager; ///< Network access manager.
    QNetworkRequest        m_request;             ///< Request.
    ::std::atomic<bool>    m_checkingFlag;        ///< Checking flag.
    bool                   m_quiet;               ///< Quiet mode.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent object.
     */
    UpdateChecker(QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~UpdateChecker();

  public slots:
    /**
     * @brief       Check update.
     *
     * @param[in]   quiet       Quiet mode.
     */
    void checkUpdate(bool quiet = false);

  private slots:
    /**
     * @brief       Called when check update request finished.
     *
     * @param[in]   reply       Reply.
     */
    void onRequestFinished(QNetworkReply *reply);
};
