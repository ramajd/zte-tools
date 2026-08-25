#include <QtTest/QtTest>
#include "zte/Codec.h"
using namespace zte;

class TstCodec : public QObject { Q_OBJECT
private slots:
    void hexRoundtrip() {
        QString s = "Halo T-SEL";
        auto hex = stringToHexUcs2(s);
        QCOMPARE(hexUcs2ToString(hex), s);
        // known from docs: spn 00540045004C... = "TELKOMSEL"
        QCOMPARE(hexUcs2ToString("00540045004C004B004F004D00530045004C"), QString("TELKOMSEL"));
    }
    void gsmDetect() {
        QVERIFY(isGsm7("hello 123"));
        QVERIFY(!isGsm7(QString::fromUtf8("halo €")));
        QCOMPARE(detectEncodeType("hi"), QString("GSM7_default"));
        QCOMPARE(detectEncodeType(QString::fromUtf8("€")), QString("UNICODE"));
    }
    void smsDate() {
        auto dt = parseSmsDate("21,05,28,14,31,29,+8");
        QCOMPARE(dt.date().year(), 2021); QCOMPARE(dt.date().month(), 5); QCOMPARE(dt.time().hour(), 14);
        auto fmt = formatSmsDate(dt);
        QVERIFY(fmt.contains("21,05,28"));
    }
    void apnSplit() {
        QString raw = "testv4($)test_apn($)manual($)*99#($)pap($)u($)p($)IP($)auto($)auto($)8.8.8.8($)8.8.8.8";
        auto parts = splitApnConfig(raw);
        QCOMPARE(parts.size(), 12); QCOMPARE(parts[0], QString("testv4"));
        QCOMPARE(joinApnConfig(parts), raw);
        // empty field case: "auto($)($)auto" -> 12 inc empty
        QString raw2 = "a($)b($)manual($)*99#($)pap($)u($)p($)IP($)auto($)($)auto($)1.1.1.1";
        auto p2 = splitApnConfig(raw2);
        QCOMPARE(p2.size(), 12);
    }
};

QTEST_MAIN(TstCodec)
#include "tst_codec.moc"
