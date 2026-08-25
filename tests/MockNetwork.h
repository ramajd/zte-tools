#pragma once
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QTimer>

// ponytail: minimal mock — subclass QNetworkReply, return canned JSON
class MockReply : public QNetworkReply {
    // no Q_OBJECT needed — no new signals
public:
    MockReply(const QByteArray &data, QObject *parent=nullptr) : QNetworkReply(parent), m_data(data) {
        open(ReadOnly | Unbuffered);
        setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
        QTimer::singleShot(0, this, [this]{
            m_buf.setData(m_data);
            m_buf.open(QIODevice::ReadOnly);
            setFinished(true);
            emit finished();
            emit metaDataChanged();
            emit readyRead();
        });
    }
    void abort() override {}
    qint64 readData(char *d, qint64 max) override { return m_buf.read(d, max); }
    qint64 bytesAvailable() const override { return m_buf.bytesAvailable() + QNetworkReply::bytesAvailable(); }
    bool isSequential() const override { return true; }
private:
    QByteArray m_data;
    QBuffer m_buf;
};

class MockNam : public QNetworkAccessManager {
    // no Q_OBJECT — avoid moc
public:
    QByteArray nextGetResponse = R"({"signalbar":"5","rssi":"-65","network_type":"LTE"})";
    QByteArray nextSetResponse = R"({"result":"success"})";
    QByteArray lastBody;
    QString lastPath;
protected:
    QNetworkReply *createRequest(Operation, const QNetworkRequest &req, QIODevice *outgoing) override {
        lastPath = req.url().path();
        if (outgoing) lastBody = outgoing->readAll();
        bool isGet = req.url().path().contains("goform_get");
        return new MockReply(isGet ? nextGetResponse : nextSetResponse, this);
    }
};
