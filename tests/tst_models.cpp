#include <QtTest/QtTest>
#include "zte/Models.h"
using namespace zte;

class TstModels : public QObject { Q_OBJECT
private slots:
    void apnFromRaw() {
        QString v4="myapn($)internet($)manual($)*99#($)pap($)user($)pass($)IP($)auto($)($)auto($)1.1.1.1($)1.0.0.1";
        QString v6="myapn($)internet6($)manual($)*99#($)chap($)u6($)p6($)IPv6($)auto($)($)manual($)2001::1($)2001::2";
        auto p = ApnProfile::fromRaw(3, v4, v6);
        QCOMPARE(p.index, 3); QCOMPARE(p.name, QString("myapn")); QCOMPARE(p.apn, QString("internet"));
        QCOMPARE(p.authMode, QString("pap")); QCOMPARE(p.ipv6AuthMode, QString("chap"));
        QVERIFY(!p.isEmpty);
        QCOMPARE(p.toV4Raw().split("($)").size(), 12);
    }
    void apnEmpty() {
        auto p = ApnProfile::fromRaw(0, "", "");
        QVERIFY(p.isEmpty);
    }
    void smsDecode() {
        SmsMessage m; m.contentHex="00480069002D"; // "Hi"
        // 0048 0069 002D -> "Hi-"
        QCOMPARE(m.decodedContent(), QString("Hi-"));
    }
};

QTEST_MAIN(TstModels)
#include "tst_models.moc"
