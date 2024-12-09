#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QMdiSubWindow>
#include "player.h"

class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(int index, QWidget *parent = nullptr);
    Player *getPlayer() const;  // 연결된 Player를 반환

signals:
    void clicked(int index);  // 클릭 신호

protected:
    void mousePressEvent(QMouseEvent *event) override;  // 마우스 클릭 이벤트

private:
    int m_index;  // 이 위젯의 인덱스
    Player *m_player;  // 연결된 Player 객체
};

#endif // CUSTOMWIDGET_H
