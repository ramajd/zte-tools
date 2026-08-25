#include "zte/Codec.h"
#include <QDateTime>

namespace zte {

QString hexUcs2ToString(const QString &hex) {
    if (hex.isEmpty()) return {};
    QString out;
    out.reserve(hex.size()/4);
    for (int i=0; i+3 < hex.size(); i+=4) {
        bool ok=false;
        uint v = hex.mid(i,4).toUInt(&ok,16);
        if (!ok) continue;
        out.append(QChar(v));
    }
    return out;
}

QString stringToHexUcs2(const QString &s) {
    QString out;
    out.reserve(s.size()*4);
    for (QChar c : s) out.append(QString("%1").arg(int(c.unicode()), 4, 16, QLatin1Char('0')).toUpper());
    return out;
}

bool isGsm7(const QString &s) {
    // ponytail: GSM7 subset ≈ printable ASCII + few extras; treat non-ASCII as UNICODE
    for (QChar c : s) {
        ushort u = c.unicode();
        if (u > 127) return false;
        // allow basic GSM7 chars; reject control except \n\r
        if (u < 32 && u != 10 && u != 13) return false;
    }
    return true;
}

QString detectEncodeType(const QString &s) {
    return isGsm7(s) ? "GSM7_default" : "UNICODE";
}

QDateTime parseSmsDate(const QString &s) {
    // "21,05,28,14,31,29,+8"
    auto p = s.split(',');
    if (p.size() < 6) return {};
    int yy = p[0].toInt(), mo=p[1].toInt(), dd=p[2].toInt(), hh=p[3].toInt(), mm=p[4].toInt(), ss=p[5].toInt();
    int year = yy < 70 ? 2000+yy : 1900+yy;
    QDateTime dt(QDate(year, mo, dd), QTime(hh, mm, ss));
    return dt;
}

QString formatSmsDate(const QDateTime &dt) {
    // device expects local time like "21;06;01;11;46;56;+7" — but we send with commas variant? docs use ; and ,
    // use comma form for consistency
    return QString("%1,%2,%3,%4,%5,%6,+8")
        .arg(dt.date().year()%100,2,10,QLatin1Char('0'))
        .arg(dt.date().month(),2,10,QLatin1Char('0'))
        .arg(dt.date().day(),2,10,QLatin1Char('0'))
        .arg(dt.time().hour(),2,10,QLatin1Char('0'))
        .arg(dt.time().minute(),2,10,QLatin1Char('0'))
        .arg(dt.time().second(),2,10,QLatin1Char('0'));
}

QStringList splitApnConfig(const QString &raw) {
    // delimiter "($)"
    if (raw.isEmpty()) return {};
    return raw.split("($)");
}
QString joinApnConfig(const QStringList &parts) {
    return parts.join("($)");
}

} // namespace zte
