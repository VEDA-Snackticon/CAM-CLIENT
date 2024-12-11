#ifndef DRAGGABLEDIALOG_H
#define DRAGGABLEDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QPoint>

class DraggableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DraggableDialog(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_dragging;
    QPoint m_dragStartPosition;
};

#endif // DRAGGABLEDIALOG_H
