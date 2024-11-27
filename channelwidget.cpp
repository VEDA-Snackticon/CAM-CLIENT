#include "channelwidget.h"
#include <QVBoxLayout>
#include <QLabel>

ChannelWidget::ChannelWidget(int channelIndex, QWidget *parent)
    : QWidget(parent), m_channelIndex(channelIndex)
{
    // 격자무늬 및 기본 스타일 설정
    setStyleSheet("background-color: #e0e0e0; border: 2px solid black;");

    // 레이아웃 생성
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 채널 번호 표시 라벨 추가
    QLabel *channelLabel = new QLabel(QString("Channel %1").arg(m_channelIndex + 1), this);
    channelLabel->setAlignment(Qt::AlignCenter);
    channelLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333333;");

    layout->addWidget(channelLabel);
    setLayout(layout);

    setFixedSize(300, 200); // 고정 크기 설정
}

void ChannelWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit channelClicked(m_channelIndex); // 클릭된 채널 인덱스 전달
    }
    QWidget::mousePressEvent(event);
}
