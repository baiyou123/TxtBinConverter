#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2>
#include "file_convert.h"
#include <QVariant>

QObject *gObject;

void showMessage(QVariant value)
{
    gObject->setProperty("text", value);
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Universal");
    QQmlApplicationEngine engine;
    FileConvert fileConvert;

    engine.rootContext()->setContextProperty("FILECONVERT",&fileConvert);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    QObject *root = engine.rootObjects().first();
    gObject = root->findChild<QObject *>("tipLabelObj");
    if (!gObject) { return -1;}

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
