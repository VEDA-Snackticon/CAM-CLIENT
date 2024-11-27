#include "player.h"
#include <gst/gst.h>
#include <QDebug>
#include <QUrl>

Player::Player(QObject *parent)
    : QObject(parent), m_pipeline(nullptr)
{
    // GStreamer 초기화
    gst_init(nullptr, nullptr);
}

void Player::setUri(const QString &uri)
{
    QString realUri = uri;

    if (realUri.indexOf("://") < 0) {
        realUri = QUrl::fromLocalFile(realUri).toEncoded();
    }

    if (!m_pipeline) {
        m_pipeline = gst_element_factory_make("playbin", "player");
        if (!m_pipeline) {
            qCritical() << "Failed to create the pipeline";
            return;
        }
    }

    g_object_set(m_pipeline, "uri", realUri.toStdString().c_str(), nullptr);
}

void Player::play()
{
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    }
}

void Player::stop()
{
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
    }
}
