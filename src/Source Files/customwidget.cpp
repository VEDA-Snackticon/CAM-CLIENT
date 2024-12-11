#include "customwidget.h"

// CustomWidget::CustomWidget(int index, Player *player, QWidget *parent)

CustomWidget::CustomWidget(int index, QWidget *parent)
    : QWidget(parent), m_index(index)
{
    setAttribute(Qt::WA_Hover);  // 마우스 이벤트를 활성화
}

Player *CustomWidget::getPlayer() const
{
    return m_player;
}

void CustomWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_index);  // 왼쪽 클릭 시 신호 발생
    }
}

