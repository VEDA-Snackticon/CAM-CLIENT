#ifndef MEDIAAPP_H
#define MEDIAAPP_H

#include <QWidget>
#include <iostream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QRadioButton>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QEvent>
#include <QFrame>
#include <QVideoWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <functional>
#include <QListWidget>
#include <QMouseEvent>
#include <QGroupBox>
#include <QHttpPart>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QStyleFactory>
#include <QDateEdit>
#include <QScrollArea>
#include "player.h"
#include "subwindow.h"
#include "DraggableDialog.h"

class MediaApp : public QWidget
{
    Q_OBJECT

public:
    explicit MediaApp(QWidget *parent = nullptr);

private slots:
    // Live Stream
    void showStreamingArea();
    void addStreamWindow();
    void fetchCameraIPsForChannel(int channelIndex, QMdiSubWindow *targetWindow);
    void startStream(int channelIndex, const QString &cameraIp, QMdiSubWindow *targetWindow);

    // Check Video
    void showVideoArea();
    void addVideoWindow();
    void fetchVideoList(int channelIndex, QMdiSubWindow *targetWindow);
    void playVideoInMdiWindow(int channelIndex, const QString &videoUrl, QMdiSubWindow *targetWindow);
    void downloadVideo(const QString &url, const QString &savePath);

    void fetchEventLog();
    void sortEventLogByTime(QJsonArray &eventList);
    void displayEventLog(const QJsonArray &eventList);

    // Camera Info
    void fetchCameraInfo();
    void displayCameraInfo(const QJsonArray &camerasArray);
    void patchCameraInfo(const QJsonArray &updatedCameras);

    // 창 크기 및 소멸
    void maximizeChannel(int channelNumber);
    void returnSize(int channelNumber);
    void closeChannel(int channelNumber);
private:
     // UI 생성
    void createUI(QBoxLayout *appLayout);
    void createCustomTitleBar(QBoxLayout *appLayout);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void uploadCameraProgram();
    void sendProgramToServer(const QString &fps, const QString &name, const QString &description);

    void showCustomMessage(const QString &title, const QString &message);

    // 프레임리스 창 이동에 필요한 변수 선언
    QPoint m_dragPosition; // 드래그 시작 위치
    bool m_dragging = false; // 드래그 상태 확인

    // First UI
    Player *m_player;
    QString m_baseDir;
    QTimer m_fullScreenTimer;
    QLabel *titleLabel;
    QPushButton *streamButton;
    QPushButton *videoButton;
    QPushButton *eventButton;
    QPushButton *serverButton;
    QPushButton *sendRequestButton;
    QPushButton *fetchInfoButton;
    QLabel *imageLabel;

    // Server
    QNetworkAccessManager *networkManager;

    // Download
    QFile *outputFile;
    QAction *downloadAction;
    QCheckBox *downloadCheckBox;

    // Camera or Video List
    QStringList videoUrls;                                              // 서버에서 가져온 비디오 URL 목록 저장
    QVector<QString> cameraIPs;                                         // 카메라 IP 목록

    // Stream or Play
    QVector<Player *> players;                                          // 각 채널의 Player
    QVector<QWidget *> videoWidgets;                                    // 각 채널의 비디오 위젯
    QDialog *gridDialog;                                                // 격자 창
    QMdiArea *mdiArea;
    Player *player;                                                     // Player 객체를 멤버로 선언

protected:
};

#endif // MEDIAAPP_H
