#include <QtTest/QtTest>
#include "zte/ZteClient.h"
#include "MockNetwork.h"

using namespace zte;

class TstClient : public QObject { Q_OBJECT
private slots:
    void loginSuccess() {
        MockNam nam;
        nam.nextSetResponse = R"({"result":"0"})";
        ZteClient c(nullptr, &nam);
        QVERIFY(c.login("admin"));
        QVERIFY(nam.lastBody.contains("goformId=LOGIN"));
        QVERIFY(nam.lastBody.contains("password="));
    }
    void loginBase64() {
        MockNam nam;
        ZteClient c(nullptr, &nam);
        c.login("hello");
        // hello base64 = aGVsbG8=
        QVERIFY(nam.lastBody.contains("aGVsbG8%3D") || nam.lastBody.contains("aGVsbG8="));
    }
    void fetchSignalParses() {
        MockNam nam;
        nam.nextGetResponse = R"({"sim_imsi":"51010","rssi":"-70","signalbar":"4","network_type":"HSPA+","network_provider":"T-SEL","spn_name_data":"00540045"})";
        ZteClient c(nullptr, &nam);
        auto s = c.fetchSignal();
        QCOMPARE(s.rssi, QString("-70")); QCOMPARE(s.signalBar, QString("4"));
    }
};

QTEST_MAIN(TstClient)
#include "tst_client.moc"
