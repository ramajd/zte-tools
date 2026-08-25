#include "ZteService.h"
#include <zte/Codec.h>
#include <QDebug>

ZteService::ZteService(QObject *parent) : QObject(parent) {
    qInfo() << "ZteService created baseUrl=" << m_client.baseUrl();
    connect(&m_client, &zte::ZteClient::errorOccurred, this, [this](const QString &e){
        qWarning() << "ZteClient error:" << e;
        setError(e);
    });
    m_poll.setInterval(3000);
    connect(&m_poll, &QTimer::timeout, this, &ZteService::refresh);
}

void ZteService::setBaseUrl(const QString &u) {
    qInfo() << "setBaseUrl" << u;
    m_client.setBaseUrl(u);
    emit baseUrlChanged();
}
void ZteService::setBusy(bool b){ if(m_busy==b) return; m_busy=b; qDebug() << "busy" << b; emit busyChanged(); }
void ZteService::setError(const QString &e){ m_lastError=e; if(!e.isEmpty()) qWarning() << "lastError:" << e; emit lastErrorChanged(); }

bool ZteService::login(const QString &password) {
    qInfo() << "login attempt baseUrl=" << m_client.baseUrl();
    setBusy(true);
    bool ok = m_client.login(password);
    qInfo() << "login result" << ok;
    if (!ok) setError("login failed");
    else setError("");
    setBusy(false);
    return ok;
}
void ZteService::refresh() {
    qDebug() << "refresh polling baseUrl=" << m_client.baseUrl();
    setBusy(true);
    auto s = m_client.fetchSignal();
    auto w = m_client.fetchWan();
    qDebug() << "refresh done networkType=" << s.networkType << "signalBar=" << s.signalBar << "pppStatus=" << w.pppStatus;
    m_networkType = s.networkType; m_signalBar = s.signalBar; m_rssi = s.rssi;
    m_pppStatus = w.pppStatus; m_provider = s.networkProvider;
    emit statusChanged();
    setBusy(false);
}
void ZteService::setPolling(bool on, int intervalMs) {
    qInfo() << "setPolling" << on << intervalMs;
    m_poll.setInterval(intervalMs);
    if (on) { QTimer::singleShot(500, this, &ZteService::refresh); m_poll.start(); } else m_poll.stop();
}
bool ZteService::connectWan(){ qInfo()<<"connectWan"; setBusy(true); bool ok=m_client.connectWan(); qInfo()<<"connectWan ok"<<ok; setBusy(false); if(!ok) setError("connect failed"); return ok; }
bool ZteService::disconnectWan(){ qInfo()<<"disconnectWan"; setBusy(true); bool ok=m_client.disconnectWan(); qInfo()<<"disconnectWan ok"<<ok; setBusy(false); if(!ok) setError("disconnect failed"); return ok; }
bool ZteService::setBearer(const QString &pref){ qInfo()<<"setBearer"<<pref; setBusy(true); bool ok=m_client.setBearerPreference(pref); qInfo()<<"setBearer ok"<<ok; setBusy(false); return ok; }
bool ZteService::sendSms(const QString &number, const QString &text){ qInfo()<<"sendSms to"<<number; setBusy(true); bool ok=m_client.sendSms(number,text); qInfo()<<"sendSms ok"<<ok; setBusy(false); return ok; }
bool ZteService::deleteSms(const QString &id){ qInfo()<<"deleteSms"<<id; setBusy(true); bool ok=m_client.deleteSms(id); qInfo()<<"deleteSms ok"<<ok; setBusy(false); return ok; }

QVariantList ZteService::apnProfiles(){
    auto v = m_client.fetchApnProfiles();
    QVariantList out;
    for(auto &p: v){
        QVariantMap m{{"index",p.index},{"name",p.name},{"apn",p.apn},{"pdpType",p.pdpType},{"isEmpty",p.isEmpty}};
        out.push_back(m);
    }
    return out;
}
QVariantMap ZteService::currentApn(){
    auto p = m_client.fetchCurrentApn();
    return {{"name",p.name},{"apn",p.apn},{"pdpType",p.pdpType},{"index",p.index},{"dial",p.dial}};
}
QVariantList ZteService::smsMessages(int memStore, int tags){
    auto v = m_client.fetchSms(memStore, tags, 0, 50);
    QVariantList out;
    for(auto &m: v){
        QVariantMap mp{{"id",m.id},{"number",m.number},{"text",m.decodedContent()},{"tag",m.tag},{"date",m.date}};
        out.push_back(mp);
    }
    return out;
}
QVariantMap ZteService::smsCapacity(){
    auto c = m_client.fetchSmsCapacity();
    return {{"nvTotal",c.nvTotal},{"simTotal",c.simTotal},{"nvRev",c.nvRev},{"nvSend",c.nvSend}};
}
