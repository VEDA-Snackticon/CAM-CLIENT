#include "player.h"
#include <gst/gst.h>
#include <QDebug>
#include <QUrl>

Player::Player(QObject *parent)
    : QObject(parent), m_pipeline(nullptr), m_videoSink(nullptr)
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

    // `d3d11va`를 명시적으로 설정
    GstElement *decodebin = gst_bin_get_by_name(GST_BIN(m_pipeline), "decodebin");
    if (decodebin) {
        g_object_set(decodebin, "hw-decoder", "d3d11va", nullptr);
        gst_object_unref(decodebin);
    }
}

void Player::setVideoOutput(QWidget *outputWidget)
{
    if (!outputWidget) {
        qCritical() << "Invalid output widget.";
        return;
    }

    if (!m_pipeline) {
        m_pipeline = gst_element_factory_make("playbin", "player");
        if (!m_pipeline) {
            qCritical() << "Failed to create the pipeline.";
            return;
        }
    }

    // 비디오 싱크 생성
    m_videoSink = gst_element_factory_make("d3dvideosink", "videosink");

    if (!m_videoSink) {
        qCritical() << "Failed to create video sink.";
        return;
    }

    // 비디오 싱크를 파이프라인에 설정
    g_object_set(m_pipeline, "video-sink", m_videoSink, nullptr);

    // QWidget에서 네이티브 윈도우 ID 가져오기
    WId winId = outputWidget->winId();
    HWND hwnd = reinterpret_cast<HWND>(winId); // WId를 HWND로 변환
    GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(m_videoSink);

    if (overlay) {
        gst_video_overlay_set_window_handle(overlay, reinterpret_cast<guintptr>(hwnd));
        gst_video_overlay_handle_events(overlay, true); // 창 이벤트 처리 활성화
    } else {
        qCritical() << "Failed to cast video sink to GstVideoOverlay.";
    }
}

void Player::resizeVideoOutput()
{
    if (m_videoSink) {
        GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(m_videoSink);
        if (overlay) {
            gst_video_overlay_expose(overlay);
        }
    }
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
