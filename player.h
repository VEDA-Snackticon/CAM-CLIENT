#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <gst/gst.h>

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(QObject *parent = nullptr);
    void setUri(const QString &uri);
    void play();
    void stop();

signals:
    void positionChanged();
    void stateChanged();

private:
    GstElement *m_pipeline;
};

#endif // PLAYER_H

