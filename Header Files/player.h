#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <gst/gst.h>
#include <QWidget>
#include <gst/video/videooverlay.h>
#include <Windows.h>

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(QObject *parent = nullptr);
    void setUri(const QString &uri);
    void play();
    void stop();
    void setVideoOutput(QWidget *outputWidget);
    void resizeVideoOutput();

signals:
    void positionChanged();
    void stateChanged();

private:
    GstElement *m_pipeline;
    GstElement *m_videoSink;
};

#endif // PLAYER_H

