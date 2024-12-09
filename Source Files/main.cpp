#include "mainwindow.h"

#include <QApplication>
#include <QLoggingCategory>
#include <QStyleFactory>
#include "mediaapp.h"
#include <gst/gst.h>

int main(int argc, char *argv[])
{
    // Qt 애플리케이션 초기화
    QApplication app(argc, argv);

    // GStreamer 초기화
    gst_init(&argc, &argv);

    QLoggingCategory::setFilterRules("*.debug=false");

    // QDarkStyle 적용
    QFile file(":/darkstyle.qss"); // qdarkstyle.qss 파일 경로
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyle(QStyleFactory::create("Fusion"));
        app.setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning("Unable to open qdarkstyle.qss file");
    }

    // MediaApp 생성 및 실행
    MediaApp media;
    media.show();

    // Qt 애플리케이션 실행
    return app.exec();
}
