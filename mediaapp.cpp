#include "mediaapp.h"
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>

MediaApp::MediaApp(QWidget *parent)
    : QWidget(parent), networkManager(new QNetworkAccessManager(this)), outputFile(nullptr)
{
    QFont font("Consolas");
    QApplication::setFont(font);

    m_player = new Player(this);
    m_baseDir = QLatin1String(".");
    m_fullScreenTimer.setSingleShot(true);

    QVBoxLayout *appLayout = new QVBoxLayout;
    appLayout->setContentsMargins(0, 0, 0, 0);
    createUI(appLayout);
    setLayout(appLayout);

    setWindowTitle(tr("Snack Guard"));
    resize(640, 480);

    // Player Map 초기화
    // for (int i = 0; i < 4; ++i) {
    //     playerMap[i] = nullptr;
    // }

    // 배경 색상 설정
    this->setStyleSheet("background-color: #f0f0f0;");

    // 배경을 이미지로 설정할 경우
    // this->setStyleSheet("background-image: url(:/images/background.png); background-repeat: no-repeat; background-position: center;");
}

// UI
void MediaApp::createUI(QBoxLayout *appLayout)
{
    titleLabel = new QLabel("Snack Guard", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #000000;");
    appLayout->addWidget(titleLabel);

    streamButton = new QPushButton(tr("Live Stream"), this);
    streamButton->setFixedSize(150, 50);
    streamButton->setStyleSheet("background-color: #FAEBD7; color: black; font-size: 14px; padding: 10px;");
    connect(streamButton, &QPushButton::clicked, this, &MediaApp::showChannelGrid);
    appLayout->addWidget(streamButton, 0, Qt::AlignCenter);

    videoButton = new QPushButton(tr("Check Video"), this);
    videoButton->setFixedSize(150, 50);
    videoButton->setStyleSheet("background-color: #FAEBD7; color: black; font-size: 14px; padding: 10px;");
    connect(videoButton, &QPushButton::clicked, this, &MediaApp::fetchVideoList);
    appLayout->addWidget(videoButton, 0, Qt::AlignCenter);

    serverButton = new QPushButton(tr("Connect Server"), this);
    serverButton->setFixedSize(150, 50);
    serverButton->setStyleSheet("background-color: #FAEBD7; color: black; font-size: 14px; padding: 10px;");
    connect(serverButton, &QPushButton::clicked, this, &MediaApp::connectToServer);
    appLayout->addWidget(serverButton, 0, Qt::AlignCenter);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    imageLabel->setPixmap(QPixmap(":/images/BI.png").scaled(100, 100, Qt::KeepAspectRatio));

    appLayout->addWidget(imageLabel, 0, Qt::AlignRight | Qt::AlignBottom);
}

// 4채널 분할 화면 표시
void MediaApp::showChannelGrid()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Live Stream Channels");

    QGridLayout *layout = new QGridLayout;

    QVector<QPushButton *> channelButtons;

    // 4개의 채널 버튼 생성
    for (int i = 0; i < 4; ++i) {
        QPushButton *channelButton = new QPushButton("Channel " + QString::number(i + 1), &dialog);
        channelButton->setFixedSize(200, 150);
        layout->addWidget(channelButton, i / 2, i % 2);
        channelButtons.append(channelButton);

        connect(channelButton, &QPushButton::clicked, this, [this, &dialog, i]() {
            dialog.accept();
            fetchCameraIPsForChannel(i); // 클릭된 채널 번호로 서버에서 IP 가져오기
        });
    }

    dialog.setLayout(layout);
    dialog.exec();
}

void MediaApp::fetchCameraIPsForChannel(int channelIndex)
{
    QString serverUrl = "https://91ac4de2-b37d-499f-9e85-abf4891f1e76.mock.pstmn.io/get_camera_ips"; // 서버 URL
    QUrl url(serverUrl);

    if (!url.isValid()) {
        QMessageBox::critical(this, "Error", "Invalid server URL.");
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, channelIndex]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonResponse.object();

            cameraIPs.clear();
            cameraIPs.append(jsonObject["cam1_ip"].toString());
            cameraIPs.append(jsonObject["cam2_ip"].toString());

            showCameraSelectionDialog(channelIndex); // IP 선택 UI 호출
        } else {
            QMessageBox::critical(this, "Error", "Failed to fetch camera IPs.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void MediaApp::showCameraSelectionDialog(int channelIndex)
{
    if (cameraIPs.isEmpty()) {
        QMessageBox::warning(this, "Error", "No camera IPs available.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Select Camera for Channel " + QString::number(channelIndex + 1));

    QVBoxLayout *layout = new QVBoxLayout;

    QVector<QRadioButton *> radioButtons;
    for (const QString &ip : cameraIPs) {
        QRadioButton *radioButton = new QRadioButton(ip, &dialog);
        layout->addWidget(radioButton);
        radioButtons.append(radioButton);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        QString selectedIp;
        for (QRadioButton *radioButton : radioButtons) {
            if (radioButton->isChecked()) {
                selectedIp = radioButton->text();
                break;
            }
        }

        if (selectedIp.isEmpty()) {
            QMessageBox::warning(this, "No Selection", "Please select a camera IP.");
        } else {
            startStream(channelIndex, selectedIp); // 선택한 IP로 스트리밍 시작
        }

        dialog.accept();
    });

    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setLayout(layout);
    dialog.exec();
}

void MediaApp::startStream(int channelIndex, const QString &cameraIp)
{
    if (cameraIp.isEmpty()) {
        QMessageBox::critical(this, "Error", "Invalid camera IP.");
        return;
    }

    QString rtspUrl = "rtsp://" + cameraIp + ":1935/live/cctv001.stream";

    std::cout << "Starting stream on Channel " << channelIndex + 1
              << " with URL: " << rtspUrl.toStdString() << std::endl;

    // if (!playerMap.contains(channelIndex)) {
    //     playerMap[channelIndex] = new Player(this);
    // }

    // Player *player = playerMap[channelIndex];

    // player->stop();
    // player->setUri(rtspUrl);
    // player->play();

    m_player = new Player(this);

    m_player->stop();
    m_player->setUri(rtspUrl);
    m_player->play();
}

// 서버에 저장된 동영상 불러오기
void MediaApp::fetchVideoList()
{
    QString serverUrl = "https://cefd612e-c1b7-4ce0-a3fe-a111698bd5c4.mock.pstmn.io/get_videos"; // 서버 URL
    QUrl url(serverUrl);

    // URL 유효성 검사
    if (!url.isValid()) {
        QMessageBox::critical(this, "Error", "Invalid server URL.");
        return;
    }

    // 서버에 요청
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // JSON 응답 처리
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonResponse.object();
            QJsonArray videoArray = jsonObject["videos"].toArray();

            // 비디오 URL 목록 초기화
            videoUrls.clear();

            QDialog dialog(this);
            dialog.setWindowTitle("Select Videos");

            QVBoxLayout *layout = new QVBoxLayout;

            // 재생할 동영상 선택하는 버튼 생성
            QVector<QRadioButton *> radioButtons;
            for (const QJsonValue &videoValue : videoArray) {
                QJsonObject videoObject = videoValue.toObject();
                QString videoName = videoObject["name"].toString();
                QString videoUrl = videoObject["url"].toString();

                videoUrls.append(videoUrl); // URL 저장

                QRadioButton *radioButton = new QRadioButton(videoName, &dialog);
                layout->addWidget(radioButton);
                radioButtons.append(radioButton);
            }

            // 다운로드 여부 선택 체크박스 추가
            QCheckBox *downloadCheckBox = new QCheckBox("Download", &dialog);
            layout->addWidget(downloadCheckBox);

            // OK 및 Cancel 버튼
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            layout->addWidget(buttonBox);

            connect(buttonBox, &QDialogButtonBox::accepted, &dialog, [&]() {
                // 선택된 동영상 확인
                QString selectedUrl;
                for (int i = 0; i < radioButtons.size(); ++i) {
                    if (radioButtons[i]->isChecked()) {
                        selectedUrl = videoUrls[i];
                        break;
                    }
                }

                if (downloadCheckBox->isChecked()) {
                    // 저장할 경로 선택
                    QString savePath = QFileDialog::getSaveFileName(this, "Save Video", "", "MP4 Files (*.mp4);;All Files (*)");
                    if (savePath.isEmpty()) {
                        QMessageBox::warning(this, "Save Cancelled", "저장 경로를 선택하지 않았습니다.");
                        return;
                    }

                    // 동영상 다운로드
                    downloadVideo(selectedUrl, savePath);
                }

                if (selectedUrl.isEmpty()) {
                    QMessageBox::information(this, "No Selection", "No video selected.");
                    dialog.accept();
                    return;
                } else {
                    // 동영상 재생
                    std::cout << "Playing video: " << selectedUrl.toStdString() << std::endl;
                    m_player->stop();
                    m_player->setUri(selectedUrl);  // 비디오 URL 설정
                    m_player->play();               // 재생
                }
                dialog.accept();
            });

            connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            dialog.setLayout(layout);
            dialog.exec();
        } else {
            QMessageBox::critical(this, "Error", "Failed to fetch video list.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

// 서버에 저장된 동영상 다운로드
void MediaApp::downloadVideo(const QString &url, const QString &savePath)
{
    QUrl videoUrl(url);
    if (!videoUrl.isValid()) {
        QMessageBox::critical(this, "Invalid URL", "유효하지 않은 URL입니다.");
        return;
    }

    QNetworkRequest request(videoUrl);
    outputFile = new QFile(savePath, this);

    if (!outputFile->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "파일 저장 실패: " + outputFile->errorString());
        delete outputFile;
        outputFile = nullptr;
        return;
    }

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        if (outputFile) {
            outputFile->write(reply->readAll());
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (outputFile) {
            outputFile->close();
            delete outputFile;
            outputFile = nullptr;
        }
        reply->deleteLater();
        QMessageBox::information(this, "Message", "Download Success");
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError code) {
        QMessageBox::critical(this, "Message", "Error: " + reply->errorString());
        if (outputFile) {
            outputFile->close();
            delete outputFile;
            outputFile = nullptr;
        }
        reply->deleteLater();
    });
}

// http 서버 접속
void MediaApp::connectToServer()
{
    // 서버 주소
    QString serverUrl = "http://snackticon.iptime.org:5555/helloWorld";
    QUrl url(serverUrl);

    // URL 유효성 검사
    if (!url.isValid() || (url.scheme() != "http" && url.scheme() != "https")) {
        QMessageBox::critical(this, "Error", "유효하지 않은 서버 주소입니다.");
        return;
    }

    // 웹 브라우저에서 URL 열기
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox::critical(this, "Error", "Web Page 오픈 실패");
    }
}

void MediaApp::handleServerReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // 서버 응답 읽기
        QString response = reply->readAll();
        QMessageBox::information(this, "Success", "서버 연결 성공.\nResponse: " + response);
    } else {
        QMessageBox::critical(this, "Error", "서버 연결 실패.\nError: " + reply->errorString());
    }
    // 메모리 해제
    reply->deleteLater();
}

void MediaApp::handleDownloadReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString response = reply->readAll();
        QMessageBox::information(this, "Success", "다운로드 성공.\nResponse: " + response);
    } else {
        QMessageBox::critical(this, "Error", "다운로드 실패.\nError: " + reply->errorString());
    }
    reply->deleteLater();
}

// void MediaApp::showChannelGrid()
// {
//     channelLayout->setContentsMargins(0, 0, 0, 0);
//     channelLayout->setSpacing(5);

//     for (int i = 0; i < 4; ++i) {
//         QLabel *channelLabel = new QLabel("Channel " + QString::number(i + 1), this);
//         channelLabel->setStyleSheet("background-color: #d3d3d3; border: 1px solid black;");
//         channelLabel->setFixedSize(200, 150);
//         channelLabel->setAlignment(Qt::AlignCenter);

//         int row = i / 2;
//         int col = i % 2;
//         channelLayout->addWidget(channelLabel, row, col);

//         // 이벤트 필터 추가
//         channelLabel->installEventFilter(this);

//         // 채널 번호와 QLabel 매핑
//         frameToChannelMap[channelLabel] = i;
//     }
// }

// bool MediaApp::eventFilter(QObject *watched, QEvent *event)
// {
//     if (event->type() == QEvent::MouseButtonPress) {
//         QLabel *clickedLabel = qobject_cast<QLabel *>(watched);
//         if (clickedLabel && frameToChannelMap.contains(clickedLabel)) {
//             int channelIndex = frameToChannelMap[clickedLabel];
//             fetchCameraIPsForChannel(channelIndex); // 해당 채널에 IP 요청
//         }
//     }
//     return QWidget::eventFilter(watched, event); // 기본 이벤트 처리
// }

// // 4채널 분할 화면 표시
// void MediaApp::showChannelGrid()
// {
//     QDialog dialog(this);
//     dialog.setWindowTitle("Live Stream Channels");

//     QGridLayout *layout = new QGridLayout;

//     QVector<QPushButton *> channelButtons;

//     // 4개의 채널 버튼 생성
//     for (int i = 0; i < 4; ++i) {
//         QPushButton *channelButton = new QPushButton("Channel " + QString::number(i + 1), &dialog);
//         channelButton->setFixedSize(200, 150);
//         layout->addWidget(channelButton, i / 2, i % 2);
//         channelButtons.append(channelButton);

//         connect(channelButton, &QPushButton::clicked, this, [this, &dialog, i]() {
//             dialog.accept();
//             fetchCameraIPsForChannel(i); // 클릭된 채널 번호로 서버에서 IP 가져오기
//         });
//     }

//     dialog.setLayout(layout);
//     dialog.exec();
// }

// // 특정 채널에서 사용할 카메라 IP 가져오기
// void MediaApp::fetchCameraIPsForChannel(int channelIndex)
// {
//     QString serverUrl = "https://91ac4de2-b37d-499f-9e85-abf4891f1e76.mock.pstmn.io/get_camera_ips"; // 서버 URL
//     QUrl url(serverUrl);

//     // URL 유효성 검사
//     if (!url.isValid()) {
//         QMessageBox::critical(this, "Error", "Invalid server URL.");
//         return;
//     }

//     QNetworkRequest request(url);
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // JSON 요청

//     QNetworkReply *reply = networkManager->get(request);

//     connect(reply, &QNetworkReply::finished, this, [this, reply, channelIndex]() {
//         if (reply->error() == QNetworkReply::NoError) {
//             QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
//             QJsonObject jsonObject = jsonResponse.object();

//             cameraIPs.clear();
//             cameraIPs.append(jsonObject["cam1_ip"].toString());
//             cameraIPs.append(jsonObject["cam2_ip"].toString());

//             showCameraSelectionDialog(channelIndex); // IP 선택 UI 호출
//         } else {
//             QMessageBox::critical(this, "Error", "Failed to fetch camera IPs.\nError: " + reply->errorString());
//         }
//         reply->deleteLater();
//     });
// }

// // 카메라 선택 UI
// void MediaApp::showCameraSelectionDialog(int channelIndex)
// {
//     if (cameraIPs.isEmpty()) {
//         QMessageBox::warning(this, "Error", "Camera IPs are not available.");
//         return;
//     }

//     QDialog dialog(this);
//     dialog.setWindowTitle("Select Camera for Channel " + QString::number(channelIndex + 1));

//     QVBoxLayout *layout = new QVBoxLayout;

//     QVector<QRadioButton *> radioButtons;

//     // 라디오 버튼으로 카메라 IP 표시
//     for (const QString &ip : cameraIPs) {
//         QRadioButton *radioButton = new QRadioButton(ip, &dialog);
//         layout->addWidget(radioButton);
//         radioButtons.append(radioButton);
//     }

//     QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
//     layout->addWidget(buttonBox);

//     connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
//         QString selectedIp;
//         for (QRadioButton *radioButton : radioButtons) {
//             if (radioButton->isChecked()) {
//                 selectedIp = radioButton->text();
//                 break;
//             }
//         }

//         if (selectedIp.isEmpty()) {
//             QMessageBox::warning(this, "No Selection", "Please select a camera IP.");
//         } else {
//             startStream(channelIndex, selectedIp); // 선택한 IP로 스트리밍 시작
//         }

//         dialog.accept();
//     });

//     connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

//     dialog.setLayout(layout);
//     dialog.exec();
// }

// // 특정 채널에서 RTSP 스트리밍 시작
// void MediaApp::startStream(int channelIndex, const QString &cameraIp)
// {
//     if (cameraIp.isEmpty()) {
//         QMessageBox::critical(this, "Error", "Invalid camera IP.");
//         return;
//     }

//     QString rtspUrl = "rtsp://" + cameraIp + ":1935/live/cctv001.stream";

//     std::cout << "Starting stream on Channel " << channelIndex + 1
//               << " with URL: " << rtspUrl.toStdString() << std::endl;

//     if (!playerMap.contains(channelIndex)) {
//         playerMap[channelIndex] = new Player(this);
//     }

//     Player *player = playerMap[channelIndex];
//     player->stop();
//     player->setUri(rtspUrl);
//     player->play();
// }

// 저장된 비디오 재생 -> 기기에 저장된 파일 재생
// void MediaApp::openFile()
// {
//     bool ok;
//     QString addr = QInputDialog::getText(this, "Video Address", "Address: ", QLineEdit::Normal, "Address 입력", &ok);
//     m_baseDir = QFileInfo(addr).path();

//     m_player->stop();
//     m_player->setUri(addr);
//     m_player->play();
// }

// 서버에 저장된 동영상 재생 및 다운로드 -> http 주소 통해
// void MediaApp::playVideo()
// {
//     QDialog dialog(this);
//     dialog.setWindowTitle("Check Video");

//     QVBoxLayout *layout = new QVBoxLayout;

//     // URL 입력창
//     QLineEdit *urlInput = new QLineEdit(&dialog);
//     urlInput->setPlaceholderText("Enter Video Address");
//     layout->addWidget(urlInput);

//     // 다운로드 선택 체크박스
//     QCheckBox *downloadCheckBox = new QCheckBox("Download", &dialog);
//     layout->addWidget(downloadCheckBox);

//     // 버튼 추가 (OK, Cancel)
//     QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
//     layout->addWidget(buttonBox);

//     dialog.setLayout(layout);

//     // OK 버튼 동작
//     connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
//         QString videoUrl = urlInput->text();

//         if (videoUrl.isEmpty()) {
//             QMessageBox::warning(this, "Invalid URL", "URL을 입력하세요.");
//             return;
//         }

//         // 체크 여부 확인
//         if (downloadCheckBox->isChecked()) {
//             // 경로 선택
//             QString savePath = QFileDialog::getSaveFileName(this, "Save Video", "", "MP4 Files (*.mp4);;All Files (*)");

//             if (savePath.isEmpty()) {
//                 QMessageBox::warning(this, "Save Cancelled", "저장 경로를 입력하세요.");
//                 return;
//             }

//             // 다운로드
//             downloadVideo(videoUrl, savePath);
//         }

//         // URL로 비디오 재생
//         std::cout << "HTTP Video Address: " << videoUrl.toStdString() << std::endl;

//         // 플레이어를 중지하고 새로운 주소로 설정
//         m_player->stop();
//         m_player->setUri(videoUrl);  // HTTP URL 설정
//         m_player->play();            // 재생

//         dialog.accept();
//     });

//     // Cancel 버튼 동작
//     connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

//     dialog.exec();
// }

// void MediaApp::triggerDownload()
// {
//     bool ok;
//     QString videoUrl = QInputDialog::getText(this, "Download Video", "다운로드 할 동영상 URL 입력:", QLineEdit::Normal, "http://<video-address>/video.mp4", &ok);

//     if (ok && !videoUrl.isEmpty()) {
//         QString savePath = QFileDialog::getSaveFileName(this, "Save Video", "", "MP4 Files (*.mp4);;All Files (*)");

//         if (savePath.isEmpty()) {
//             QMessageBox::warning(this, "저장 실패", "저장 경로를 설정하세요.");
//             return;
//         }

//         downloadVideo(videoUrl, savePath);
//     } else {
//         QMessageBox::warning(this, "Invalid URL", "유효하지 않은 URL입니다.");
//     }
// }

// // 서버로부터 카메라 IP 받아오기
// void MediaApp::fetchCameraIPs()
// {
//     QString serverUrl = "https://91ac4de2-b37d-499f-9e85-abf4891f1e76.mock.pstmn.io/get_camera_ips"; // 서버 URL
//     QUrl url(serverUrl);

//     // URL 유효성 검사 -> http 또는 https
//     if (!url.isValid()) {
//         QMessageBox::critical(this, "Error", "Invalid server URL.");
//         return;
//     }

//     // QNetworkRequest 객체 생성
//     QNetworkRequest request(url);
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // JSON 형식 요청

//     // GET 요청 보내기
//     QNetworkReply *reply = networkManager->get(request);

//     // 서버 응답 처리
//     connect(reply, &QNetworkReply::finished, this, [this, reply]() {
//         if (reply->error() == QNetworkReply::NoError) {
//             // 서버 응답 처리
//             QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
//             QJsonObject jsonObject = jsonResponse.object();

//             // 기존 cameraIPs 리스트 초기화 후 값 추가
//             cameraIPs.clear();
//             cameraIPs.append(jsonObject["cam1_ip"].toString());
//             cameraIPs.append(jsonObject["cam2_ip"].toString());

//             // 서버에서 받은 IP 확인 메시지 (테스트용)
//             QString ipList;
//             for (const QString &ip : cameraIPs) {
//                 ipList += ip + "\n";
//             }
//             QMessageBox::information(this, "Camera IPs", "Camera IPs received:\n" + ipList);

//             // 카메라 선택 UI 호출
//             showCameraSelectionDialog();
//         } else {
//             // 요청 실패 처리
//             QMessageBox::critical(this, "Error", "Failed to fetch camera IPs.\nError: " + reply->errorString());
//         }
//         reply->deleteLater();
//     });
// }

// // 카메라 선택 UI
// void MediaApp::showCameraSelectionDialog()
// {
//     if (cameraIPs.isEmpty()) {
//         QMessageBox::warning(this, "Error", "Camera IPs are not available.");
//         return;
//     }

//     QDialog dialog(this);
//     dialog.setWindowTitle("Select Camera");
//     QVBoxLayout *layout = new QVBoxLayout;

//     QVector<QRadioButton *> radioButtons;

//     // 라디오 버튼으로 카메라 IP 표시
//     for (const QString &ip : cameraIPs) {
//         QRadioButton *radioButton = new QRadioButton(ip, &dialog);
//         layout->addWidget(radioButton);
//         radioButtons.append(radioButton);
//     }

//     QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
//     layout->addWidget(buttonBox);

//     connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
//         QString selectedIp;
//         for (QRadioButton *radioButton : radioButtons) {
//             if (radioButton->isChecked()) {
//                 selectedIp = radioButton->text();
//                 break;
//             }
//         }

//         if (selectedIp.isEmpty()) {
//             QMessageBox::warning(this, "No Selection", "Please select a camera IP.");
//         } else {
//             connectRTSP(selectedIp); // 선택한 IP로 스트리밍 시작
//         }

//         dialog.accept();
//     });

//     connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

//     dialog.setLayout(layout);
//     dialog.exec();
// }

// // RTSP 통한 실시간 스트리밍
// void MediaApp::connectRTSP(const QString &cameraIp)
// {
//     if (cameraIp.isEmpty()) {
//         QMessageBox::critical(this, "Error", "Invalid camera IP.");
//         return;
//     }

//     // IP 주소로부터 RTSP URL 생성 -> 라즈베리파이일 경우: rtsp://<라즈베리파이_IP>:<포트번호>/<스트림 경로>
//     // QString rtspUrl = "rtsp://" + cameraIp + ":" + port + "/stream1";
//     QString rtspUrl = "rtsp://" + cameraIp + ":1935/live/cctv001.stream";

//     std::cout << "Connecting to RTSP URL: " << rtspUrl.toStdString() << std::endl;

//     m_player->stop();
//     m_player->setUri(rtspUrl);
//     m_player->play();
// }
