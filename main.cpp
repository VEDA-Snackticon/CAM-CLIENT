#include "mainwindow.h"

#include <QApplication>
#include "mediaapp.h"
#include <gst/gst.h>

int main(int argc, char *argv[])
{
    // Qt 애플리케이션 초기화
    QApplication app(argc, argv);

    // GStreamer 초기화
    gst_init(&argc, &argv);

    // MediaApp 생성 및 실행
    MediaApp media;
    media.show();

    // Qt 애플리케이션 실행
    return app.exec();
}
