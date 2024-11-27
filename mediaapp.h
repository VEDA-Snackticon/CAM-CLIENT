#ifndef MEDIAAPP_H
#define MEDIAAPP_H

#include <QWidget>
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
#include <QGridLayout>
#include <QEvent>
#include <QFrame>
#include "player.h"
// #include "channelwidget.h"

class MediaApp : public QWidget
{
    Q_OBJECT

public:
    explicit MediaApp(QWidget *parent = nullptr);

private slots:
    void startStream(int channelIndex, const QString &cameraIp);
    void showCameraSelectionDialog(int channelIndex);                   // 카메라 선택
    void fetchCameraIPsForChannel(int channelIndex);                    // 서버에서 Cam1, Cam2 IP 가져오기
    void connectToServer();                                             // 서버 접속
    void fetchVideoList();                                              // 서버에 저장된 동영상 목록 가져오기
    void downloadVideo(const QString &url, const QString &savePath);    // 동영상 다운로드
    void handleServerReply(QNetworkReply *reply);                       // 서버 응답 처리
    void handleDownloadReply(QNetworkReply *reply);
    void showChannelGrid();
    // void eventFilter(QObject *watched, QEvent *event);
    // void showMultiStreamDialog();
    // void selectCameraIP(QWidget *pane);                              // 클릭한 창에서 카메라 IP 선택
    // void connectRTSP(const QString &cameraIp);                       // 실시간 스트리밍

private:
    void createUI(QBoxLayout *appLayout);                               // UI 생성

    Player *m_player;
    QString m_baseDir;
    QTimer m_fullScreenTimer;
    QLabel *titleLabel;
    QPushButton *streamButton;
    QPushButton *videoButton;
    QPushButton *serverButton;
    QLabel *imageLabel;
    // QPushButton *cam1Button;
    // QPushButton *cam2Button;
    QNetworkAccessManager *networkManager;

    QFile *outputFile;
    QAction *downloadAction;
    QCheckBox *downloadCheckBox;

    QStringList videoUrls;                                              // 서버에서 가져온 비디오 URL 목록 저장
    QVector<QString> cameraIPs;                                         // 카메라 IP 목록

    // QVector<QWidget *> streamPanes;                                     // 각 스트림 창
    // QVector<Player *> streamPlayers;                                    // 각 스트림에 연결된 Player 객체
    // QMap<int, ChannelWidget *> gridWidgets; // 채널 번호 -> ChannelWidget
    // QMap<int, Player *> playerMap;                                      // 채널 번호 -> Player 객체 맵
    // QMap<QLabel *, int> frameToChannelMap;
    QGridLayout *channelLayout;                                         // 4분할 레이아웃

protected:
    // bool eventFilter(QObject *watched, QEvent *event) override; // 이벤트 필터 재정의
};

#endif // MEDIAAPP_H
