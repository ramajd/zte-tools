#include "zte/ZteClient.h"
#include "zte/Codec.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QUrlQuery>

namespace zte {

ZteClient::ZteClient(QObject *parent, QNetworkAccessManager *nam) : QObject(parent) {
    if (nam) { m_nam = nam; m_ownsNam = false; }
    else { m_nam = new QNetworkAccessManager(this); m_ownsNam = true; }
}

void ZteClient::setBaseUrl(const QString &url) { setBaseUrl(QUrl(url)); }
void ZteClient::setBaseUrl(const QUrl &url) {
    if (m_baseUrl == url) return;
    m_baseUrl = url;
    m_referer = url.toString(QUrl::RemovePath) + "/index.html";
    emit baseUrlChanged();
}

QByteArray ZteClient::buildGetBody(const QStringList &cmds) const {
    QUrlQuery q;
    q.addQueryItem("isTest","false");
    q.addQueryItem("cmd", cmds.join(','));
    // device also accepts multi_data=1 in URL, but body only needs cmd
    return q.query(QUrl::FullyEncoded).toUtf8();
}
QByteArray ZteClient::buildSetBody(const QVariantMap &params) const {
    QUrlQuery q;
    for (auto it=params.begin(); it!=params.end(); ++it) q.addQueryItem(it.key(), it.value().toString());
    return q.query(QUrl::FullyEncoded).toUtf8();
}

QJsonObject ZteClient::doPost(const QString &path, const QByteArray &body) {
    QUrl url = m_baseUrl;
    url.setPath(path);
    // for get, append multi_data query as docs show
    if (path.contains("goform_get")) {
        QUrlQuery uq(url.query());
        uq.addQueryItem("multi_data","1");
        uq.addQueryItem("isTest","false");
        url.setQuery(uq);
    }
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    req.setRawHeader("Referer", m_referer.toUtf8());
    req.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");

    auto *reply = m_nam->post(req, body);
    QEventLoop loop;
    QTimer t; t.setSingleShot(true);
    QObject::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    t.start(m_timeoutMs);
    loop.exec();
    QJsonObject out;
    if (reply->isFinished()) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) out = doc.object();
        else if (!data.isEmpty()) {
            // device returns text/plain but JSON; fallback
            doc = QJsonDocument::fromJson(data.trimmed());
            if (doc.isObject()) out = doc.object();
        }
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
        }
    } else {
        reply->abort();
        emit errorOccurred("timeout");
    }
    reply->deleteLater();
    return out;
}

QJsonObject ZteClient::postGet(const QStringList &cmds) {
    if (cmds.isEmpty()) return {};
    return doPost("/goform/goform_get_cmd_process", buildGetBody(cmds));
}
QJsonObject ZteClient::postSet(const QVariantMap &params) {
    return doPost("/goform/goform_set_cmd_process", buildSetBody(params));
}

bool ZteClient::login(const QString &password) {
    auto b64 = QString::fromUtf8(password.toUtf8().toBase64());
    auto o = postSet({{"goformId","LOGIN"},{"password", b64}});
    bool ok = o.value("result").toString() == "0" || o.value("result").toString() == "success";
    m_loggedIn = ok;
    if (ok) emit loggedInChanged();
    else emit errorOccurred("login failed");
    return ok;
}

SignalInfo ZteClient::fetchSignal() {
    auto o = postGet({"sim_imsi","hmcc","hmnc","hplmn","rssi","rscp","lte_rsrp","network_type","signalbar","network_provider","spn_name_data","simcard_roam"});
    SignalInfo s;
    s.simImsi=o["sim_imsi"].toString(); s.hmcc=o["hmcc"].toString(); s.hmnc=o["hmnc"].toString(); s.hplmn=o["hplmn"].toString();
    s.rssi=o["rssi"].toString(); s.rscp=o["rscp"].toString(); s.lteRsrp=o["lte_rsrp"].toString();
    s.networkType=o["network_type"].toString(); s.signalBar=o["signalbar"].toString();
    s.networkProvider=o["network_provider"].toString(); s.spnNameData=o["spn_name_data"].toString(); s.simcardRoam=o["simcard_roam"].toString();
    return s;
}

WanStatus ZteClient::fetchWan() {
    auto o = postGet({"connectionMode","autoConnectWhenRoaming","ppp_status","realtime_rx_bytes","realtime_tx_bytes","realtime_rx_thrpt","realtime_tx_thrpt","realtime_time","monthly_rx_bytes","monthly_tx_bytes","monthly_time"});
    WanStatus w;
    w.connectionMode=o["connectionMode"].toString(); w.autoRoaming=o["autoConnectWhenRoaming"].toString(); w.pppStatus=o["ppp_status"].toString();
    w.realtimeRxBytes=o["realtime_rx_bytes"].toString().toLongLong();
    w.realtimeTxBytes=o["realtime_tx_bytes"].toString().toLongLong();
    w.realtimeRxThrpt=o["realtime_rx_thrpt"].toString().toLongLong();
    w.realtimeTxThrpt=o["realtime_tx_thrpt"].toString().toLongLong();
    w.realtimeTime=o["realtime_time"].toString().toLongLong();
    w.monthlyRxBytes=o["monthly_rx_bytes"].toString().toLongLong();
    w.monthlyTxBytes=o["monthly_tx_bytes"].toString().toLongLong();
    w.monthlyTime=o["monthly_time"].toString().toLongLong();
    return w;
}

QVector<ApnProfile> ZteClient::fetchApnProfiles() {
    QStringList cmds;
    for(int i=0;i<20;i++) cmds << QString("APN_config%1").arg(i) << QString("ipv6_APN_config%1").arg(i);
    cmds << "m_profile_name" << "wan_apn" << "Current_index";
    auto o = postGet(cmds);
    QVector<ApnProfile> out;
    for(int i=0;i<20;i++) {
        auto v4 = o[QString("APN_config%1").arg(i)].toString();
        auto v6 = o[QString("ipv6_APN_config%1").arg(i)].toString();
        out.push_back(ApnProfile::fromRaw(i, v4, v6));
    }
    return out;
}
ApnProfile ZteClient::fetchCurrentApn() {
    auto o = postGet({"m_profile_name","wan_dial","pdp_type","Current_index","wan_apn","ppp_auth_mode","ppp_username","ppp_passwd","dns_mode","prefer_dns_manual","standby_dns_manual","ipv6_wan_apn","ipv6_ppp_auth_mode","ipv6_ppp_username","ipv6_ppp_passwd","ipv6_dns_mode","ipv6_prefer_dns_manual","ipv6_standby_dns_manual"});
    ApnProfile p;
    p.name=o["m_profile_name"].toString(); p.dial=o["wan_dial"].toString(); p.pdpType=o["pdp_type"].toString();
    p.index=o["Current_index"].toString().toInt(); p.apn=o["wan_apn"].toString(); p.authMode=o["ppp_auth_mode"].toString();
    p.username=o["ppp_username"].toString(); p.password=o["ppp_passwd"].toString(); p.dnsMode=o["dns_mode"].toString();
    p.preferDns=o["prefer_dns_manual"].toString(); p.standbyDns=o["standby_dns_manual"].toString();
    p.ipv6Apn=o["ipv6_wan_apn"].toString(); p.ipv6AuthMode=o["ipv6_ppp_auth_mode"].toString(); p.ipv6Username=o["ipv6_ppp_username"].toString();
    p.ipv6Password=o["ipv6_ppp_passwd"].toString(); p.ipv6DnsMode=o["ipv6_dns_mode"].toString(); p.ipv6PreferDns=o["ipv6_prefer_dns_manual"].toString(); p.ipv6StandbyDns=o["ipv6_standby_dns_manual"].toString();
    p.isEmpty = p.name.isEmpty() && p.apn.isEmpty();
    return p;
}

bool ZteClient::saveApn(const ApnProfile &p) {
    QVariantMap m{
        {"goformId","APN_PROC_EX"},{"apn_action","save"},{"apn_mode","manual"},{"pdp_select","auto"},
        {"profile_name",p.name},{"wan_dial",p.dial},{"pdp_type",p.pdpType},{"index",QString::number(p.index)},
        {"wan_apn",p.apn},{"ppp_auth_mode",p.authMode},{"ppp_username",p.username},{"ppp_passwd",p.password},
        {"dns_mode",p.dnsMode},{"prefer_dns_manual",p.preferDns},{"standby_dns_manual",p.standbyDns},
        {"ipv6_wan_apn",p.ipv6Apn},{"ipv6_ppp_auth_mode",p.ipv6AuthMode},{"ipv6_ppp_username",p.ipv6Username},{"ipv6_ppp_passwd",p.ipv6Password},
        {"ipv6_dns_mode",p.ipv6DnsMode},{"ipv6_prefer_dns_manual",p.ipv6PreferDns},{"ipv6_standby_dns_manual",p.ipv6StandbyDns},
    };
    auto o = postSet(m);
    return o["result"].toString()=="success";
}
bool ZteClient::deleteApn(int index) {
    auto o = postSet({{"goformId","APN_PROC_EX"},{"apn_action","delete"},{"index",QString::number(index)}});
    return o["result"].toString()=="success";
}
bool ZteClient::setDefaultApn(int index, const QString &pdpType) {
    auto o = postSet({{"goformId","APN_PROC_EX"},{"apn_action","set_default"},{"apn_mode","manual"},{"set_default_flag","1"},{"pdp_type",pdpType},{"index",QString::number(index)}});
    return o["result"].toString()=="success";
}
bool ZteClient::setBearerPreference(const QString &pref) {
    auto o = postSet({{"goformId","SET_BEARER_PREFERENCE"},{"BearerPreference",pref}});
    return o["result"].toString()=="success";
}
bool ZteClient::setConnectionMode(const QString &mode, const QString &roam) {
    auto o = postSet({{"goformId","SET_CONNECTION_MODE"},{"ConnectionMode",mode},{"roam_setting_option",roam}});
    return o["result"].toString()=="success";
}
bool ZteClient::connectWan() {
    auto o = postSet({{"goformId","CONNECT_NETWORK"}});
    return o["result"].toString()=="success";
}
bool ZteClient::disconnectWan() {
    auto o = postSet({{"goformId","DISCONNECT_NETWORK"}});
    return o["result"].toString()=="success";
}

SmsCapacity ZteClient::fetchSmsCapacity() {
    auto o = postGet({"sms_capacity_info"}); // returns multiple keys
    // fallback: try explicit keys
    if (!o.contains("sms_nv_total")) o = postGet({"sms_nv_total","sms_sim_total","sms_nv_rev_total","sms_nv_send_total","sms_nv_draftbox_total","sms_sim_rev_total","sms_sim_send_total","sms_sim_draftbox_total"});
    SmsCapacity c;
    c.nvTotal=o["sms_nv_total"].toString().toInt(); c.simTotal=o["sms_sim_total"].toString().toInt();
    c.nvRev=o["sms_nv_rev_total"].toString().toInt(); c.nvSend=o["sms_nv_send_total"].toString().toInt(); c.nvDraft=o["sms_nv_draftbox_total"].toString().toInt();
    c.simRev=o["sms_sim_rev_total"].toString().toInt(); c.simSend=o["sms_sim_send_total"].toString().toInt(); c.simDraft=o["sms_sim_draftbox_total"].toString().toInt();
    return c;
}

QVector<SmsMessage> ZteClient::fetchSms(int memStore, int tags, int page, int perPage, const QString &orderBy) {
    // docs: cmd=sms_data_total&page=0&data_per_page=500&mem_store=1&tags=10&order_by=...
    // we use postGet with cmd + extra query items in body
    QUrlQuery q;
    q.addQueryItem("cmd","sms_data_total");
    q.addQueryItem("page",QString::number(page));
    q.addQueryItem("data_per_page",QString::number(perPage));
    q.addQueryItem("mem_store",QString::number(memStore));
    q.addQueryItem("tags",QString::number(tags));
    q.addQueryItem("order_by",orderBy);
    // doPost with custom body that includes cmd plus params
    auto o = doPost("/goform/goform_get_cmd_process", q.query(QUrl::FullyEncoded).toUtf8());
    QVector<SmsMessage> out;
    auto arr = o["messages"].toArray();
    // some firmware returns sms_data_total wrapper; try fallback
    if (arr.isEmpty() && o.contains("messages")) arr = o["messages"].toArray();
    for(auto v: arr) {
        auto obj=v.toObject();
        SmsMessage m;
        m.id=obj["id"].toString(); m.number=obj["number"].toString(); m.contentHex=obj["content"].toString();
        m.tag=obj["tag"].toString(); m.date=obj["date"].toString(); m.draftGroupId=obj["draft_group_id"].toString();
        m.receivedAllConcat=obj["received_all_concat_sms"].toString(); m.concatTotal=obj["concat_sms_total"].toString(); m.concatReceived=obj["concat_sms_received"].toString();
        out.push_back(m);
    }
    return out;
}

bool ZteClient::sendSms(const QString &number, const QString &text) {
    auto enc = detectEncodeType(text);
    auto hex = enc=="UNICODE" ? stringToHexUcs2(text) : stringToHexUcs2(text); // ponytail: use UCS2 for both; GSM7 also hex UCS2 works on MF910
    // device expects MessageBody as hex UCS2 regardless; encode_type tells how to display
    QString bodyHex = stringToHexUcs2(text);
    if (enc=="GSM7_default") {
        // for GSM7, still hex of GSM7 chars is same as UCS2 for ASCII, so reuse
        bodyHex = stringToHexUcs2(text);
    }
    auto now = QDateTime::currentDateTime();
    QString smsTime = formatSmsDate(now).replace(',', ';'); // SEND_SMS expects ; separator
    QVariantMap m{{"goformId","SEND_SMS"},{"notCallback","true"},{"Number",number},{"sms_time",smsTime},{"MessageBody",bodyHex},{"ID","-1"},{"encode_type",enc}};
    auto o = postSet(m);
    return o["result"].toString()=="success";
}
bool ZteClient::deleteSms(const QString &msgId) {
    auto o = postSet({{"goformId","DELETE_SMS"},{"notCallback","true"},{"msg_id",msgId}});
    return o["result"].toString()=="success";
}

} // namespace zte
