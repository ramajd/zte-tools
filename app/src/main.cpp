#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ZteService.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    qmlRegisterType<ZteService>("ZteTools", 1, 0, "ZteService");
    QQmlApplicationEngine engine;
    engine.loadFromModule("ZteTools", "Main");
    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}
