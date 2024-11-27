#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(int channelIndex, QWidget *parent = nullptr);

signals:
    void channelClicked(int channelIndex);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_channelIndex;
};

#endif // CHANNELWIDGET_H
