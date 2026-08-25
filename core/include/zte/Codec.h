#pragma once
#include <QString>
#include <QDateTime>

namespace zte {

// UCS2 hex <-> QString. Device sends content as hex of UTF-16BE (e.g. 004D...).
QString hexUcs2ToString(const QString &hex);
QString stringToHexUcs2(const QString &s);

// Decide encode_type for SEND_SMS: GSM7_default if all chars in GSM7, else UNICODE.
// ponytail: naive GSM7 check — covers ASCII subset, good enough for v1; upgrade if needed.
QString detectEncodeType(const QString &s);
bool isGsm7(const QString &s);

// SMS date: "21,05,28,14,31,29,+8" -> QDateTime (local)
QDateTime parseSmsDate(const QString &s);
QString formatSmsDate(const QDateTime &dt);

// SPN hex decode (same as UCS2)
inline QString spnDecode(const QString &hex) { return hexUcs2ToString(hex); }

// APN ($)-split helpers
QStringList splitApnConfig(const QString &raw);
QString joinApnConfig(const QStringList &parts);

} // namespace zte
