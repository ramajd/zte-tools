#pragma once
#include <QObject>
#include <QUrl>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include "Models.h"

namespace zte {

class ZteClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(bool loggedIn READ isLoggedIn NOTIFY loggedInChanged)
public:
    explicit ZteClient(QObject *parent = nullptr, QNetworkAccessManager *nam = nullptr);
    ~ZteClient() override = default;

    QString baseUrl() const { return m_baseUrl.toString(); }
    void setBaseUrl(const QString &url);
    void setBaseUrl(const QUrl &url);

    bool isLoggedIn() const { return m_loggedIn; }

    // Low-level
    QJsonObject postGet(const QStringList &cmds); // sync via event loop — ponytail: blocking for simplicity
    QJsonObject postSet(const QVariantMap &params);

    // Auth: password is plain text, will be base64-encoded
    bool login(const QString &password);
    // Helpers
    SignalInfo fetchSignal();
    WanStatus fetchWan();
    QVector<ApnProfile> fetchApnProfiles(); // 0..19 + current
    ApnProfile fetchCurrentApn();
    bool saveApn(const ApnProfile &p);
    bool deleteApn(int index);
    bool setDefaultApn(int index, const QString &pdpType = "IP");
    bool setBearerPreference(const QString &pref); // NETWORK_auto, Only_LTE etc
    bool setConnectionMode(const QString &mode, const QString &roam = "off");
    bool connectWan();
    bool disconnectWan();

    SmsCapacity fetchSmsCapacity();
    QVector<SmsMessage> fetchSms(int memStore=1, int tags=10, int page=0, int perPage=20, const QString &orderBy="order+by+id+desc");
    bool sendSms(const QString &number, const QString &text);
    bool deleteSms(const QString &msgId);

    void setReferer(const QString &r) { m_referer = r; }
    void setTimeoutMs(int ms) { m_timeoutMs = ms; }

signals:
    void baseUrlChanged();
    void loggedInChanged();
    void errorOccurred(const QString &msg);

private:
    QJsonObject doPost(const QString &path, const QByteArray &body);
    QByteArray buildGetBody(const QStringList &cmds) const;
    QByteArray buildSetBody(const QVariantMap &params) const;

    QUrl m_baseUrl{"http://192.168.0.1"};
    QString m_referer{"http://192.168.0.1/index.html"};
    QNetworkAccessManager *m_nam = nullptr;
    bool m_ownsNam = false;
    bool m_loggedIn = false;
    int m_timeoutMs = 8000;
};

} // namespace zte
