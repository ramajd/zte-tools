#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QLoggingCategory>
#include <QTimer>
#include "ZteService.h"

static QFile g_logFile;
static QtMessageHandler g_oldHandler = nullptr;

static void logToFile(QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
    QString level;
    switch (type) {
        case QtDebugMsg: level = "DEBUG"; break;
        case QtInfoMsg: level = "INFO"; break;
        case QtWarningMsg: level = "WARN"; break;
        case QtCriticalMsg: level = "CRIT"; break;
        case QtFatalMsg: level = "FATAL"; break;
    }
    QString line = QString("%1 [%2] %3 (%4:%5)\n")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
        .arg(level, msg,
             QString(ctx.file ? ctx.file : ""), QString::number(ctx.line));
    // always to stderr so `zte-gui` in Terminal shows logs
    fprintf(stderr, "%s", line.toUtf8().constData());
    fflush(stderr);
    if (g_logFile.isOpen()) {
        g_logFile.write(line.toUtf8());
        g_logFile.flush();
    }
    if (g_oldHandler) g_oldHandler(type, ctx, msg);
    if (type == QtFatalMsg) abort();
}

int main(int argc, char *argv[]) {
    // ponytail: log to /tmp so `cat /tmp/zte-gui.log` works after Finder double-click
    // (QDir::tempPath() on macOS is /var/folders/... which users never check)
    QString logPath = "/tmp/zte-gui.log";
    g_logFile.setFileName(logPath);
    if (g_logFile.open(QIODevice::Append | QIODevice::Text)) {
        g_logFile.write(QString("\n=== zte-gui start %1 ===\n").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).toUtf8());
        g_logFile.flush();
    }
    g_oldHandler = qInstallMessageHandler(logToFile);

    // log before QApplication (argv only)
    qInfo() << "zte-gui starting" << "args:" << argc << "Qt" << qVersion() << "log:" << logPath << "cwd:" << QDir::currentPath();
    for (int i = 0; i < argc; ++i) qInfo() << "argv[" << i << "]=" << argv[i];

    QGuiApplication app(argc, argv);
    app.setApplicationName("zte-gui");
    app.setOrganizationName("zte-tools");

    qInfo() << "appDir:" << QCoreApplication::applicationDirPath() << "cwd:" << QDir::currentPath();
    qInfo() << "QML import paths:" << app.libraryPaths();

    qmlRegisterType<ZteService>("ZteTools", 1, 0, "ZteService");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        [](const QUrl &url){ qCritical() << "QML objectCreationFailed:" << url; });
    // log every warning/error from QML
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
        [](const QList<QQmlError> &warns){ for(auto &e: warns) qWarning() << "QML warning:" << e.toString() << e.url(); });

    qInfo() << "loading QML module ZteTools Main...";
    engine.loadFromModule("ZteTools", "Main");

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "FATAL: QML load failed - no root objects. Check log:" << logPath;
        // already logged via warnings signal
        fprintf(stderr, "zte-gui: QML load failed, see %s\n", qPrintable(logPath));
        return -1;
    }
    qInfo() << "QML loaded, rootObjects:" << engine.rootObjects().size() << "window visible, entering event loop";
    int rc = app.exec();
    qInfo() << "event loop exited rc=" << rc;
    return rc;
}
