#include "DraggableDialog.h"

DraggableDialog::DraggableDialog(QWidget *parent)
    : QDialog(parent), m_dragging(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 프레임 없는 창
}

void DraggableDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void DraggableDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragStartPosition);
        event->accept();
    }
}

void DraggableDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}
