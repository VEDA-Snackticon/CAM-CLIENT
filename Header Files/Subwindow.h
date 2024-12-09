#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>

class SubWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit SubWindow(QWidget *parent = nullptr) : QMdiSubWindow(parent) {}

protected:
    void closeEvent(QCloseEvent *event) override {
        resetToEmptyChannel();
        event->ignore(); // 창을 완전히 닫지 않고 상태만 초기화
    }

private:
    void resetToEmptyChannel() {
        // Placeholder 생성
        QWidget *placeholder = new QWidget(this);
        placeholder->setStyleSheet("background-color: black;");

        QLabel *emptyLabel = new QLabel("Empty Channel", placeholder);
        emptyLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
        emptyLabel->setAlignment(Qt::AlignCenter);

        QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
        placeholderLayout->addWidget(emptyLabel);
        placeholder->setLayout(placeholderLayout);

        // 서브윈도우를 초기 상태로 재설정
        this->setWidget(placeholder);
        this->setWindowTitle("Empty Channel");
    }
};

#endif // SUBWINDOW_H
