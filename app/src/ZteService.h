#pragma once
#include <QObject>
#include <QTimer>
#include <zte/ZteClient.h>

class ZteService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString networkType READ networkType NOTIFY statusChanged)
    Q_PROPERTY(QString signalBar READ signalBar NOTIFY statusChanged)
    Q_PROPERTY(QString rssi READ rssi NOTIFY statusChanged)
    Q_PROPERTY(QString pppStatus READ pppStatus NOTIFY statusChanged)
    Q_PROPERTY(QString provider READ provider NOTIFY statusChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
    explicit ZteService(QObject *parent=nullptr);

    QString baseUrl() const { return m_client.baseUrl(); }
    void setBaseUrl(const QString &u);

    QString networkType() const { return m_networkType; }
    QString signalBar() const { return m_signalBar; }
    QString rssi() const { return m_rssi; }
    QString pppStatus() const { return m_pppStatus; }
    QString provider() const { return m_provider; }
    bool busy() const { return m_busy; }
    QString lastError() const { return m_lastError; }

    Q_INVOKABLE bool login(const QString &password);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setPolling(bool on, int intervalMs=3000);
    Q_INVOKABLE bool connectWan();
    Q_INVOKABLE bool disconnectWan();
    Q_INVOKABLE bool setBearer(const QString &pref);
    Q_INVOKABLE bool sendSms(const QString &number, const QString &text);
    Q_INVOKABLE bool deleteSms(const QString &id);

    // expose types to QML via QVariant
    Q_INVOKABLE QVariantList apnProfiles();
    Q_INVOKABLE QVariantMap currentApn();
    Q_INVOKABLE QVariantList smsMessages(int memStore=1, int tags=10);
    Q_INVOKABLE QVariantMap smsCapacity();

signals:
    void baseUrlChanged();
    void statusChanged();
    void busyChanged();
    void lastErrorChanged();

private:
    void setBusy(bool b);
    void setError(const QString &e);

    zte::ZteClient m_client;
    QTimer m_poll;
    QString m_networkType, m_signalBar, m_rssi, m_pppStatus, m_provider, m_lastError;
    bool m_busy=false;
};
