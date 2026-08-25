#pragma once
#include <QString>
#include <QStringList>
#include <QVector>

namespace zte {

struct SignalInfo {
    QString simImsi, hmcc, hmnc, hplmn;
    QString rssi, rscp, lteRsrp;
    QString networkType, signalBar, networkProvider, spnNameData;
    QString simcardRoam;
    QString spnDecoded() const;
};

struct WanStatus {
    QString connectionMode; // auto_dial / manual_dial
    QString autoRoaming;
    QString pppStatus; // ppp_connected etc
    qint64 realtimeRxBytes = 0, realtimeTxBytes = 0;
    qint64 realtimeRxThrpt = 0, realtimeTxThrpt = 0;
    qint64 realtimeTime = 0;
    qint64 monthlyRxBytes = 0, monthlyTxBytes = 0, monthlyTime = 0;
};

struct ApnProfile {
    QString name, apn, dial = "*99#", pdpType = "IP", authMode = "auto";
    QString username, password, dnsMode = "auto", preferDns, standbyDns;
    // ipv6
    QString ipv6Apn, ipv6AuthMode = "auto", ipv6Username, ipv6Password;
    QString ipv6DnsMode = "auto", ipv6PreferDns, ipv6StandbyDns;
    int index = -1;
    bool isEmpty = true;

    // Parse APN_configX / ipv6_APN_configX raw "a($)b($)..." (12 fields)
    static ApnProfile fromRaw(int idx, const QString &v4Raw, const QString &v6Raw);
    QString toV4Raw() const;
    QString toV6Raw() const;
};

struct SmsMessage {
    QString id, number, contentHex, tag, date, draftGroupId;
    QString receivedAllConcat, concatTotal, concatReceived;
    QString decodedContent() const;
    int tagInt() const { return tag.toInt(); }
    bool isUnread() const { return tag == "1"; }
};

struct SmsCapacity {
    int nvTotal=0, simTotal=0, nvRev=0, nvSend=0, nvDraft=0, simRev=0, simSend=0, simDraft=0;
};

} // namespace zte
