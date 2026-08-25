#include "zte/Models.h"
#include "zte/Codec.h"

namespace zte {

QString SignalInfo::spnDecoded() const { return spnDecode(spnNameData); }

QString SmsMessage::decodedContent() const { return hexUcs2ToString(contentHex); }

ApnProfile ApnProfile::fromRaw(int idx, const QString &v4Raw, const QString &v6Raw) {
    ApnProfile p;
    p.index = idx;
    if (v4Raw.isEmpty() && v6Raw.isEmpty()) { p.isEmpty = true; return p; }
    auto a = splitApnConfig(v4Raw);
    auto b = splitApnConfig(v6Raw);
    auto at = [&](const QStringList &l, int i){ return i < l.size() ? l[i] : QString(); };
    p.name = at(a,0); p.apn = at(a,1); p.dial = at(a,3).isEmpty()? "*99#" : at(a,3);
    p.authMode = at(a,4); p.username = at(a,5); p.password = at(a,6);
    p.pdpType = at(a,7); p.dnsMode = at(a,9); p.preferDns = at(a,10); p.standbyDns = at(a,11);
    if (!b.isEmpty()) {
        p.ipv6Apn = at(b,1); p.ipv6AuthMode = at(b,4); p.ipv6Username = at(b,5); p.ipv6Password = at(b,6);
        p.ipv6DnsMode = at(b,9); p.ipv6PreferDns = at(b,10); p.ipv6StandbyDns = at(b,11);
        if (p.apn.isEmpty()) p.apn = p.ipv6Apn;
    }
    p.isEmpty = p.name.isEmpty() && p.apn.isEmpty();
    return p;
}

QString ApnProfile::toV4Raw() const {
    QStringList a(12);
    a[0]=name; a[1]=apn; a[2]="manual"; a[3]=dial; a[4]=authMode; a[5]=username; a[6]=password;
    a[7]=pdpType; a[8]="auto"; a[9]=dnsMode; a[10]=preferDns; a[11]=standbyDns;
    return joinApnConfig(a);
}
QString ApnProfile::toV6Raw() const {
    QStringList b(12);
    b[0]=name; b[1]=ipv6Apn.isEmpty()? apn : ipv6Apn; b[2]="manual"; b[3]=dial; b[4]=ipv6AuthMode; b[5]=ipv6Username; b[6]=ipv6Password;
    b[7]="IPv6"; b[8]="auto"; b[9]=ipv6DnsMode; b[10]=ipv6PreferDns; b[11]=ipv6StandbyDns;
    return joinApnConfig(b);
}

} // namespace zte
