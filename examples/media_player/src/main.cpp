#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QDebug>
#include "backend.h"

int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);

  qmlRegisterUncreatableType<MediaProvider::Provider>("MediaProvider", 1, 0, "Provider", "Interface");
  qmlRegisterUncreatableType<MediaProvider::Resource>("MediaProvider", 1, 0, "Resource", "Interface");
  qmlRegisterUncreatableType<MediaProvider::Stream>("MediaProvider", 1, 0, "Stream", "Interface");

  Backend backend;

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("backend", &backend);
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
      &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
