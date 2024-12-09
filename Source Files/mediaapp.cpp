#include "mediaapp.h"
#include <iostream>

MediaApp::MediaApp(QWidget *parent)
    : QWidget(parent), networkManager(new QNetworkAccessManager(this)), outputFile(nullptr), player(new Player(this))
{
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 스타일시트 로드 및 적용
    QFile file(":/darkstyle.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    m_player = new Player(this);
    m_baseDir = QLatin1String(".");
    m_fullScreenTimer.setSingleShot(true);

    QVBoxLayout *appLayout = new QVBoxLayout;
    appLayout->setContentsMargins(0, 0, 0, 0);
    createCustomTitleBar(appLayout);
    createUI(appLayout);
    setLayout(appLayout);

    resize(640, 480);
}

// 초기화면 CUSTOM
void MediaApp::createCustomTitleBar(QBoxLayout *appLayout)
{
    // 타이틀 바 위젯 생성
    QWidget *titleBar = new QWidget(this);
    titleBar->setStyleSheet("background-color: #FAEBD7;");
    QHBoxLayout *titleBarLayout = new QHBoxLayout(titleBar);
    titleBarLayout->setContentsMargins(5, 0, 5, 0);

    QWidget *leftSpacer = new QWidget(titleBar);
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 제목
    QLabel *titleLabel = new QLabel("       Snack Guard", titleBar);
    titleLabel->setStyleSheet("color: black; font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QWidget *rightSpacer = new QWidget(titleBar);
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 닫기 버튼
    QPushButton *closeButton = new QPushButton("X", titleBar);
    closeButton->setStyleSheet(R"(
        QPushButton {
            color: black;
            background-color: transparent;
            font-size: 16px;
            font-weight: bold;
            border: none;
        }
        QPushButton:hover {
            color: red;
            background-color: #FAEBD7;
        }
        QPushButton:pressed {
            color: darkred;
        }
    )");
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        // 커스텀 다이얼로그 생성
        QDialog customDialog(this, Qt::FramelessWindowHint | Qt::Dialog);
        customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&customDialog);

        QLabel *messageLabel = new QLabel("Do you want to close this application?", &customDialog);
        messageLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *yesButton = new QPushButton("Yes", &customDialog);
        yesButton->setStyleSheet(R"(
            QPushButton {
                background-color: #455364;
                color: #ffffff;
                border: none;
                border-radius: 5px;
                padding: 10px 20px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #54687A;
            }
            QPushButton:pressed {
                background-color: #60798B;
            }
        )");
        QPushButton *noButton = new QPushButton("No", &customDialog);
        noButton->setStyleSheet(R"(
            QPushButton {
                background-color: #455364;
                color: #ffffff;
                border: none;
                border-radius: 5px;
                padding: 10px 20px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #54687A;
            }
            QPushButton:pressed {
                background-color: #60798B;
            }
        )");

        // 버튼 클릭 이벤트 연결
        connect(yesButton, &QPushButton::clicked, &customDialog, [this, &customDialog]() {
            customDialog.accept();
            close();                            // 어플리케이션 종료
        });
        connect(noButton, &QPushButton::clicked, &customDialog, &QDialog::reject);

        buttonLayout->addWidget(yesButton);
        buttonLayout->addWidget(noButton);

        layout->addLayout(buttonLayout);

        customDialog.setLayout(layout);

        // 다이얼로그 실행
        customDialog.exec();
    });

    // 레이아웃에 위젯 추가
    titleBarLayout->addWidget(leftSpacer);                  // 왼쪽 공간
    titleBarLayout->addWidget(titleLabel);                  // 중앙 제목
    titleBarLayout->addWidget(rightSpacer);                 // 오른쪽 공간
    titleBarLayout->addWidget(closeButton);                 // 닫기 버튼

    // 레이아웃에 타이틀 바 추가
    appLayout->addWidget(titleBar);
}

// UI
void MediaApp::createUI(QBoxLayout *appLayout)
{
    streamButton = new QPushButton(tr("Live Stream"), this);
    streamButton->setFixedSize(150, 50);
    streamButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FAEBD7;"
        "   color: #000000;"
        "   border: 2px solid #8B4513;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #CD853F;"
        "}"
        );
    connect(streamButton, &QPushButton::clicked, this, &MediaApp::showStreamingArea);
    appLayout->addWidget(streamButton, 0, Qt::AlignCenter);

    videoButton = new QPushButton(tr("Saved Video"), this);
    videoButton->setFixedSize(150, 50);
    videoButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FAEBD7;"
        "   color: #000000;"
        "   border: 2px solid #8B4513;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #CD853F;"
        "}"
        );
    connect(videoButton, &QPushButton::clicked, this, &MediaApp::showVideoArea);
    appLayout->addWidget(videoButton, 0, Qt::AlignCenter);

    fetchInfoButton = new QPushButton(tr("Camera Info"), this);
    fetchInfoButton->setFixedSize(150, 50);
    fetchInfoButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FAEBD7;"
        "   color: #000000;"
        "   border: 2px solid #8B4513;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #CD853F;"
        "}"
        );
    connect(fetchInfoButton, &QPushButton::clicked, this, &MediaApp::fetchCameraInfo);
    appLayout->addWidget(fetchInfoButton, 0, Qt::AlignCenter);

    QPushButton *uploadButton = new QPushButton(tr("Upload Program"), this);
    uploadButton->setFixedSize(150, 50);
    uploadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FAEBD7;"
        "   color: #000000;"
        "   border: 2px solid #8B4513;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #CD853F;"
        "}"
        );
    connect(uploadButton, &QPushButton::clicked, this, &MediaApp::uploadCameraProgram);
    appLayout->addWidget(uploadButton, 0, Qt::AlignCenter);
}

/*
 * [ Live Stream 버튼 클릭 시 동작 ]
 *
 * 1. Add Camera 버튼 통해 스트리밍할 RTSP 주소 선택
 * 2. Maximize Channel, Return Size, Close Channel 버튼 통해 창 사이즈 조절 및 닫기
 *
 */

// 4채널 MDI Area 표시
void MediaApp::showStreamingArea()
{
    mdiArea = new QMdiArea;
    mdiArea->setViewMode(QMdiArea::SubWindowView);

    QDialog *mdiDialog = new QDialog(this);
    mdiDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    mdiDialog->resize(1302, 1018);

    QVBoxLayout *mdiLayout = new QVBoxLayout(mdiDialog);

    // "Add Camera" 버튼
    QPushButton *addStreamButton = new QPushButton("+ Add Camera", mdiDialog);
    addStreamButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(addStreamButton, &QPushButton::clicked, this, &MediaApp::addStreamWindow);

    // 채널 번호 입력
    QLineEdit *channelInput = new QLineEdit(mdiDialog);
    channelInput->setPlaceholderText("Channel Number");
    channelInput->setFixedWidth(150);

    // 확대
    QPushButton *maximizeButton = new QPushButton("Maximize", mdiDialog);
    maximizeButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(maximizeButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();                       // 채널 번호 가져오기
        if (channelNumber >= 1 && channelNumber <= 4) {
            maximizeChannel(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 원래 크기로
    QPushButton *restoreButton = new QPushButton("Return Size", mdiDialog);
    restoreButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(restoreButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();
        if (channelNumber >= 1 && channelNumber <= 4) {
            returnSize(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 채널 닫기
    QPushButton *closeButton = new QPushButton("Close", mdiDialog);
    closeButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(closeButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();
        if (channelNumber >= 1 && channelNumber <= 4) {
            closeChannel(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 창 닫기 버튼 추가
    QPushButton *exitButton = new QPushButton("Exit", mdiDialog);
    exitButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(exitButton, &QPushButton::clicked, mdiDialog, [mdiDialog]() {
        // 커스텀 다이얼로그 생성
        QDialog customDialog(mdiDialog, Qt::FramelessWindowHint | Qt::Dialog);
        customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&customDialog);

        // 메시지 라벨 추가
        QLabel *messageLabel = new QLabel("Do you want to close this window?", &customDialog);
        messageLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);

        // 버튼 추가
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *yesButton = new QPushButton("Yes", &customDialog);
        yesButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
        QPushButton *noButton = new QPushButton("No", &customDialog);
        noButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");

        // 버튼 클릭 이벤트
        connect(yesButton, &QPushButton::clicked, &customDialog, [&customDialog, mdiDialog]() {
            customDialog.accept();
            mdiDialog->close();
        });
        connect(noButton, &QPushButton::clicked, &customDialog, &QDialog::reject);

        buttonLayout->addWidget(yesButton);
        buttonLayout->addWidget(noButton);

        layout->addLayout(buttonLayout);

        customDialog.setLayout(layout);

        // 다이얼로그 실행
        customDialog.exec();
    });

    // 버튼 및 입력 필드 추가
    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(addStreamButton);
    controlsLayout->addWidget(channelInput);
    controlsLayout->addWidget(maximizeButton);
    controlsLayout->addWidget(restoreButton);
    controlsLayout->addWidget(closeButton);

    // 세로 구분선 추가
    QFrame *verticalLine = new QFrame(mdiDialog);
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    verticalLine->setStyleSheet("color: #757575;");
    controlsLayout->addWidget(verticalLine);
    QFrame *verticalLine2 = new QFrame(mdiDialog);
    verticalLine2->setFrameShape(QFrame::VLine);
    verticalLine2->setFrameShadow(QFrame::Sunken);
    verticalLine2->setStyleSheet("color: #757575;");
    controlsLayout->addWidget(verticalLine2);

    controlsLayout->addWidget(exitButton);

    mdiLayout->addLayout(controlsLayout);
    mdiLayout->addWidget(mdiArea);

    mdiDialog->setLayout(mdiLayout);

    // 미리 4개의 서브윈도우 생성
    for (int i = 0; i < 4; ++i) {
        QMdiSubWindow *subWindow = new QMdiSubWindow;

        // 서브윈도우의 제목 표시줄 제거
        subWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

        // Placeholder 위젯 추가
        QWidget *placeholder = new QWidget(subWindow);
        placeholder->setStyleSheet("background-color: black;");

        QLabel *emptyLabel = new QLabel("Empty Channel", placeholder);
        emptyLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
        emptyLabel->setAlignment(Qt::AlignCenter);

        QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
        placeholderLayout->addWidget(emptyLabel);
        placeholder->setLayout(placeholderLayout);

        subWindow->setWidget(placeholder);
        subWindow->setWindowTitle("Channel " + QString::number(i + 1));
        subWindow->resize(640, 480);
        mdiArea->addSubWindow(subWindow);

        subWindow->show();
    }

    mdiDialog->exec();
}

// 새로운 스트리밍 창 추가
void MediaApp::addStreamWindow()
{
    // 미리 생성된 서브윈도우 리스트 가져오기
    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

    // 빈 서브윈도우를 찾기
    QMdiSubWindow *targetWindow = nullptr;
    for (QMdiSubWindow *subWindow : subWindows) {
        // 빈 채널인지 확인 (Empty Channel 텍스트를 확인)
        QLabel *emptyLabel = subWindow->findChild<QLabel *>();
        if (emptyLabel && emptyLabel->text() == "Empty Channel") {
            targetWindow = subWindow;
            break;
        }
    }

    if (!targetWindow) {
        showCustomMessage("Error", "No empty slots available for new streams.");
        return;
    }

    // 비어 있는 서브윈도우에 영상을 추가
    int channelIndex = subWindows.indexOf(targetWindow);                // 서브윈도우 인덱스 가져오기
    fetchCameraIPsForChannel(channelIndex, targetWindow);
}

// 서버로부터 카메라 IP 받아오기
void MediaApp::fetchCameraIPsForChannel(int channelIndex, QMdiSubWindow *targetWindow)
{
    QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_camera_ips"; // 서버 URL
    QUrl url(serverUrl);

    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, channelIndex, targetWindow]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonResponse.object();

            cameraIPs.clear();
            cameraIPs.append(jsonObject["cam1_ip"].toString());
            cameraIPs.append(jsonObject["cam2_ip"].toString());
            cameraIPs.append(jsonObject["cam3_ip"].toString());
            cameraIPs.append(jsonObject["cam4_ip"].toString());

            // 카메라 선택 다이얼로그를 보여줌
            QDialog dialog(this, Qt::FramelessWindowHint | Qt::Dialog);
            dialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");
            dialog.setWindowTitle("");

            QVBoxLayout *layout = new QVBoxLayout;

            QVector<QRadioButton *> radioButtons;

            // 라디오 버튼의 스타일시트 설정
            QString radioStyle = R"(
                QRadioButton {
                    spacing: 5px;
                    color: #ffffff;
                    font-size: 18px;
                }
                QRadioButton::indicator {
                    width: 16px;
                    height: 16px;
                    border-radius: 8px;
                    border: 2px solid #757575;
                    background-color: #2d2d2d;
                }
                QRadioButton::indicator:hover {
                    border: 2px solid #54687A;
                }
                QRadioButton::indicator:checked {
                    background-color: #60798B;
                    border: 2px solid #ffffff;
                }
                QRadioButton::indicator:unchecked {
                    background-color: #19232D;
                    border: 2px solid #ffffff;
                }
            )";

            for (const QString &ip : cameraIPs) {
                QRadioButton *radioButton = new QRadioButton(ip, &dialog);
                radioButton->setStyleSheet(radioStyle);
                layout->addWidget(radioButton);
                radioButtons.append(radioButton);
            }

            // 확인 버튼
            QPushButton *okButton = new QPushButton("OK", &dialog);
            okButton->setStyleSheet(R"(
                QPushButton {
                    background-color: #455364;
                    color: #ffffff;
                    border: none;
                    border-radius: 5px;
                    padding: 10px 20px;
                    font-size: 14px;
                }
                QPushButton:hover {
                    background-color: #54687A;
                }
                QPushButton:pressed {
                    background-color: #60798B;
                }
            )");
            connect(okButton, &QPushButton::clicked, [&]() {
                QString selectedIp;
                for (QRadioButton *radioButton : radioButtons) {
                    if (radioButton->isChecked()) {
                        selectedIp = radioButton->text();
                        break;
                    }
                }

                if (selectedIp.isEmpty()) {
                    showCustomMessage("Error", "Please select a camera.");
                    return;
                }

                startStream(channelIndex, selectedIp, targetWindow); // 선택된 IP로 스트리밍 시작
                dialog.accept();
            });

            // 취소 버튼
            QPushButton *cancelButton = new QPushButton("Cancel", &dialog);
            cancelButton->setStyleSheet(R"(
                QPushButton {
                    background-color: #455364;
                    color: #ffffff;
                    border: none;
                    border-radius: 5px;
                    padding: 10px 20px;
                    font-size: 14px;
                }
                QPushButton:hover {
                    background-color: #54687A;
                }
                QPushButton:pressed {
                    background-color: #60798B;
                }
            )");
            connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

            // 버튼 레이아웃
            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);

            // 다이얼로그 레이아웃 구성
            layout->addLayout(buttonLayout);
            dialog.setLayout(layout);
            dialog.exec();
        } else {
            showCustomMessage("Error", "Failed to fetch camera IPs.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

// 실시간 RTSP 스트리밍
void MediaApp::startStream(int channelIndex, const QString &cameraIp, QMdiSubWindow *targetWindow)
{
    if (cameraIp.isEmpty()) {
        showCustomMessage("Error", "Invalid camera IP.");
        return;
    }

    QWidget *videoWidget = new QWidget(targetWindow);
    videoWidget->setStyleSheet("background-color: black;");

    // Player와 VideoWidget 연결
    Player *player = new Player(this);
    player->setVideoOutput(videoWidget);
    QString channelStr = QString::number(channelIndex + 1);
    player->setUri("rtsp://" + cameraIp + ":1935/live/cctv00" + channelStr + ".stream");
    player->play();

    // 서브윈도우 레이아웃 설정
    QVBoxLayout *layout = new QVBoxLayout(videoWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    videoWidget->setLayout(layout);

    targetWindow->setWidget(videoWidget);
    targetWindow->setWindowTitle("Channel " + QString::number(channelIndex + 1));
    targetWindow->resize(640, 480);
    targetWindow->show();
}
/* ************************************************************************************************************ */
/* ************************************************************************************************************ */

/*
 * [ Saved Video 버튼 클릭 시 동작 ]
 *
 * 1. Video Lists 버튼 통해 서버에 저장된 동영상 중 재생할 영상의 주소 및 다운로드 여부 선택
 * 2. Maximize Video, Return Size, Close Video 버튼 통해 창 사이즈 조절 및 닫기
 *
 */

// 4채널 MDI Area 표시
void MediaApp::showVideoArea()
{
    mdiArea = new QMdiArea;
    mdiArea->setViewMode(QMdiArea::SubWindowView);

    QDialog *mdiDialog = new QDialog(this);
    mdiDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    mdiDialog->resize(1302, 1023);

    QVBoxLayout *mdiLayout = new QVBoxLayout(mdiDialog);

    // 비디오 리스트 확인
    QPushButton *addVideoButton = new QPushButton("Video Lists", mdiDialog);
    addVideoButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(addVideoButton, &QPushButton::clicked, this, &MediaApp::addVideoWindow);

    // 채널 번호 입력
    QLineEdit *channelInput = new QLineEdit(mdiDialog);
    channelInput->setPlaceholderText("Slot Number");
    channelInput->setFixedWidth(150);

    // 확대
    QPushButton *maximizeButton = new QPushButton("Maximize Video", mdiDialog);
    maximizeButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(maximizeButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();                           // 채널 번호 가져오기
        if (channelNumber >= 1 && channelNumber <= 4) {
            maximizeChannel(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 원래 크기로
    QPushButton *restoreButton = new QPushButton("Return Size", mdiDialog);
    restoreButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(restoreButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();
        if (channelNumber >= 1 && channelNumber <= 4) {
            returnSize(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 채널 닫기
    QPushButton *closeButton = new QPushButton("Close Video", mdiDialog);
    closeButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(closeButton, &QPushButton::clicked, this, [this, channelInput]() {
        int channelNumber = channelInput->text().toInt();
        if (channelNumber >= 1 && channelNumber <= 4) {
            closeChannel(channelNumber);
        } else {
            showCustomMessage("Invalid Channel", "Please enter a valid channel number (1-4).");
        }
    });

    // 창 닫기 버튼 추가
    QPushButton *exitButton = new QPushButton("Exit", mdiDialog);
    exitButton->setStyleSheet("font-size: 14px; padding: 10px;");
    connect(exitButton, &QPushButton::clicked, mdiDialog, [mdiDialog]() {
        // 커스텀 다이얼로그 생성
        QDialog customDialog(mdiDialog, Qt::FramelessWindowHint | Qt::Dialog);
        customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&customDialog);

        // 메시지 라벨 추가
        QLabel *messageLabel = new QLabel("Do you want to close the window?", &customDialog);
        messageLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);

        // 버튼 추가
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *yesButton = new QPushButton("Yes", &customDialog);
        yesButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
        QPushButton *noButton = new QPushButton("No", &customDialog);
        noButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");

        // 버튼 클릭 이벤트
        connect(yesButton, &QPushButton::clicked, &customDialog, [&customDialog, mdiDialog]() {
            customDialog.accept();
            mdiDialog->close();
        });
        connect(noButton, &QPushButton::clicked, &customDialog, &QDialog::reject);

        buttonLayout->addWidget(yesButton);
        buttonLayout->addWidget(noButton);

        layout->addLayout(buttonLayout);

        customDialog.setLayout(layout);

        // 다이얼로그 실행
        customDialog.exec();
    });

    // 버튼 및 입력 필드 추가
    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(addVideoButton);
    controlsLayout->addWidget(channelInput);
    controlsLayout->addWidget(maximizeButton);
    controlsLayout->addWidget(restoreButton);
    controlsLayout->addWidget(closeButton);

    // 세로 구분선 추가
    QFrame *verticalLine = new QFrame(mdiDialog);
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    verticalLine->setStyleSheet("color: #757575;");
    controlsLayout->addWidget(verticalLine);
    QFrame *verticalLine2 = new QFrame(mdiDialog);
    verticalLine2->setFrameShape(QFrame::VLine);
    verticalLine2->setFrameShadow(QFrame::Sunken);
    verticalLine2->setStyleSheet("color: #757575;");
    controlsLayout->addWidget(verticalLine2);

    controlsLayout->addWidget(exitButton);

    mdiLayout->addLayout(controlsLayout);
    mdiLayout->addWidget(mdiArea);

    mdiDialog->setLayout(mdiLayout);

    // 미리 4개의 서브윈도우 생성
    for (int i = 0; i < 4; ++i) {
        SubWindow *subWindow = new SubWindow;

        // 서브윈도우의 제목 표시줄 제거
        subWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

        // Placeholder 위젯 추가
        QWidget *placeholder = new QWidget(subWindow);
        placeholder->setStyleSheet("background-color: black;");

        QLabel *emptyLabel = new QLabel("Empty Slot", placeholder);
        emptyLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
        emptyLabel->setAlignment(Qt::AlignCenter);

        QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
        placeholderLayout->addWidget(emptyLabel);
        placeholder->setLayout(placeholderLayout);

        subWindow->setWidget(placeholder);
        subWindow->setWindowTitle("Channel " + QString::number(i + 1));
        subWindow->resize(640, 480);
        mdiArea->addSubWindow(subWindow);

        subWindow->show();
    }

    mdiDialog->exec();
}

// 동영상 재생 창 추가
void MediaApp::addVideoWindow()
{
    //미리 생성된 서브윈도우 리스트 가져오기
    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

    // 빈 서브윈도우를 찾기
    QMdiSubWindow *targetWindow = nullptr;
    for (QMdiSubWindow *subWindow : subWindows) {
        // 빈 채널인지 확인 (Empty Channel 텍스트를 확인)
        QLabel *emptyLabel = subWindow->findChild<QLabel *>();
        if (emptyLabel && emptyLabel->text() == "Empty Slot") {
            targetWindow = subWindow;
            break;
        }
    }

    if (!targetWindow) {
        showCustomMessage("Error", "No available slots for new videos.");
        return;
    }

    int channelIndex = subWindows.indexOf(targetWindow);                // 서브윈도우 인덱스 가져오기
    fetchVideoList(channelIndex, targetWindow);
}

// 서버에 저장된 동영상 목록 불러오기
void MediaApp::fetchVideoList(int channelIndex, QMdiSubWindow *targetWindow)
{
    QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_videos"; // 서버 URL
    QUrl url(serverUrl);

    // URL 유효성 검사
    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    // 서버에 요청
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, channelIndex, targetWindow]() {
        if (reply->error() == QNetworkReply::NoError) {
            // JSON 응답 처리
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonResponse.object();
            QJsonArray videoArray = jsonObject["videos"].toArray();

            // 비디오 URL 목록 초기화
            videoUrls.clear();

            // 타이틀 바 없는 다이얼로그 생성
            QDialog dialog(this, Qt::FramelessWindowHint | Qt::Dialog);
            dialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

            QVBoxLayout *layout = new QVBoxLayout;

            // 재생할 동영상 선택하는 버튼 생성
            QVector<QRadioButton *> radioButtons;

            // 라디오 버튼 스타일 정의
            QString radioStyle = R"(
                QRadioButton {
                    spacing: 5px;
                    color: #ffffff;
                    font-size: 18px;
                }
                QRadioButton::indicator {
                    width: 16px;
                    height: 16px;
                    border-radius: 8px;
                    border: 2px solid #757575;
                    background-color: #2d2d2d;
                }
                QRadioButton::indicator:hover {
                    border: 2px solid #54687A;
                }
                QRadioButton::indicator:checked {
                    background-color: #60798B;
                    border: 2px solid #ffffff;
                }
                QRadioButton::indicator:unchecked {
                    background-color: #19232D;
                    border: 2px solid #ffffff;
                }
            )";

            for (const QJsonValue &videoValue : videoArray) {
                QJsonObject videoObject = videoValue.toObject();
                QString videoName = videoObject["name"].toString();
                QString videoUrl = videoObject["url"].toString();

                videoUrls.append(videoUrl); // URL 저장

                QRadioButton *radioButton = new QRadioButton(videoName, &dialog);
                radioButton->setStyleSheet(radioStyle);
                layout->addWidget(radioButton);
                radioButtons.append(radioButton);
            }

            // 다운로드 여부 체크박스 스타일 정의
            QString checkboxStyle = R"(
                QCheckBox {
                    spacing: 5px;
                    color: #DFE1E2;
                    font-size: 14px;
                }
                QCheckBox::indicator {
                    width: 16px;
                    height: 16px;
                    border-radius: 3px;
                    border: 2px solid #757575;
                    background-color: #2d2d2d;
                }
                QCheckBox::indicator:hover {
                    border: 2px solid #54687A;
                }
                QCheckBox::indicator:checked {
                    background-color: #60798B;
                    border: 2px solid #ffffff;
                }
                QCheckBox::indicator:unchecked {
                    background-color: #19232D;
                    border: 2px solid #ffffff;
                }
            )";

            // 다운로드 여부 선택 체크박스 추가
            QCheckBox *downloadCheckBox = new QCheckBox("Download", &dialog);
            downloadCheckBox->setStyleSheet(checkboxStyle);
            layout->addWidget(downloadCheckBox);

            // OK 버튼
            QPushButton *okButton = new QPushButton("OK", &dialog);
            okButton->setStyleSheet(R"(
                QPushButton {
                    background-color: #455364;
                    color: #ffffff;
                    border: none;
                    border-radius: 5px;
                    padding: 10px 20px;
                    font-size: 14px;
                }
                QPushButton:hover {
                    background-color: #54687A;
                }
                QPushButton:pressed {
                    background-color: #60798B;
                }
            )");

            // Cancel 버튼
            QPushButton *cancelButton = new QPushButton("Cancel", &dialog);
            cancelButton->setStyleSheet(R"(
                QPushButton {
                    background-color: #455364;
                    color: #ffffff;
                    border: none;
                    border-radius: 5px;
                    padding: 10px 20px;
                    font-size: 14px;
                }
                QPushButton:hover {
                    background-color: #54687A;
                }
                QPushButton:pressed {
                    background-color: #60798B;
                }
            )");

            // 버튼 레이아웃
            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);

            // OK 버튼 클릭 이벤트
            connect(okButton, &QPushButton::clicked, [&]() {
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
                        showCustomMessage("Save Cancelled", "No storage path has been selected");
                        return;
                    }

                    // 동영상 다운로드
                    downloadVideo(selectedUrl, savePath);
                }

                if (selectedUrl.isEmpty()) {
                    showCustomMessage("Error", "Please select video.");
                    return;
                } else {
                    // 동영상 재생
                    playVideoInMdiWindow(channelIndex, selectedUrl, targetWindow);
                }

                dialog.accept();
            });

            // Cancel 버튼 클릭 이벤트
            connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

            layout->addLayout(buttonLayout);
            dialog.setLayout(layout);
            dialog.exec();
        } else {
            showCustomMessage("Error", "Failed to fetch camera IPs.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

// Mdi Window에서 선택한 동영상 재생
void MediaApp::playVideoInMdiWindow(int channelIndex, const QString &videoUrl, QMdiSubWindow *targetWindow)
{
    if (videoUrl.isEmpty()) {
        showCustomMessage("Error", "Invalid video URL.");
        return;
    }

    QWidget *videoWidget = new QWidget(targetWindow);
    videoWidget->setStyleSheet("background-color: black;");

    // Player와 VideoWidget 연결
    Player *player = new Player(this);
    player->setVideoOutput(videoWidget);
    player->setUri(videoUrl);
    player->play();

    // 서브윈도우 레이아웃 설정
    QVBoxLayout *layout = new QVBoxLayout(videoWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    videoWidget->setLayout(layout);

    targetWindow->setWidget(videoWidget);
    targetWindow->setWindowTitle("Channel " + QString::number(channelIndex + 1));
    targetWindow->resize(640, 480);
    targetWindow->show();
}

// 서버에 저장된 동영상 다운로드
void MediaApp::downloadVideo(const QString &url, const QString &savePath)
{
    QUrl videoUrl(url);
    if (!videoUrl.isValid()) {
        showCustomMessage("Invalid URL", "Please enter a valid URL");
        return;
    }

    QNetworkRequest request(videoUrl);
    outputFile = new QFile(savePath, this);

    if (!outputFile->open(QIODevice::WriteOnly)) {
        showCustomMessage("Error", "Failed to save file: " + outputFile->errorString());
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
        showCustomMessage("Message", "Download Success");
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError code) {
        showCustomMessage("Error", "Download Error: " + reply->errorString());
        if (outputFile) {
            outputFile->close();
            delete outputFile;
            outputFile = nullptr;
        }
        reply->deleteLater();
    });
}
/* ************************************************************************************************************ */
/* ************************************************************************************************************ */

/*
 * [ CameraInfo 버튼 클릭 시 동작 ]
 *
 * 1. 서버로부터 Camera Info 수신
 * 2. Camera Info 확인 후 원하는 정보 수정 후 서버로 PATCH 수행
 *
 */

// 서버로부터 카메라 정보 받아오기
void MediaApp::fetchCameraInfo()
{
    QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_camera_list";
    QUrl url(serverUrl);

    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonArray cameraList = jsonResponse.object()["cameras"].toArray();

            // 카메라 정보를 UI에 표시
            displayCameraInfo(cameraList);

        } else {
            showCustomMessage("Error", "Failed to fetch camera information.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

// 받아온 정보 표시
void MediaApp::displayCameraInfo(const QJsonArray &cameraList)
{
    QDialog dialog(this, Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVector<QLineEdit *> descriptionEdits;
    QVector<QLineEdit *> groupNumberEdits;
    QVector<QLineEdit *> ipAddressEdits;
    QVector<QCheckBox *> isMasterEdits;

    for (const QJsonValue &value : cameraList) {
        QJsonObject camera = value.toObject();

        // GroupBox 생성
        QGroupBox *cameraBox = new QGroupBox("Camera Info", &dialog);
        cameraBox->setStyleSheet(R"(
            QGroupBox {
                border: 2px solid #455364;
                border-radius: 5px;
                margin-top: 10px;
                padding: 10px;
                font-weight: bold;
                color: #ffffff;
                font-size: 14px;
            }
        )");

        QVBoxLayout *boxLayout = new QVBoxLayout(cameraBox);

        // Description
        QLabel *descriptionLabel = new QLabel("Description:", cameraBox);
        descriptionLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(descriptionLabel);

        QLabel *descriptionEdit = new QLabel(camera["description"].toString(), cameraBox);
        boxLayout->addWidget(descriptionEdit);

        // Group Number
        QLabel *groupNumberLabel = new QLabel("Group Number:", cameraBox);
        groupNumberLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(groupNumberLabel);

        QLineEdit *groupNumberEdit = new QLineEdit(QString::number(camera["groupNumber"].toInt()), cameraBox);
        boxLayout->addWidget(groupNumberEdit);
        groupNumberEdits.append(groupNumberEdit);

        // IP Address
        QLabel *ipAddrLabel = new QLabel("IP Address:", cameraBox);
        ipAddrLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(ipAddrLabel);

        QLabel *ipAddrEdit = new QLabel(camera["ipAddr"].toString(), cameraBox);
        boxLayout->addWidget(ipAddrEdit);

        // Is Master
        QLabel *isMasterLabel = new QLabel("Is Master:", cameraBox);
        isMasterLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(isMasterLabel);

        QCheckBox *isMasterEdit = new QCheckBox(cameraBox);
        isMasterEdit->setChecked(camera["isMaster"].toBool());
        isMasterEdit->setStyleSheet(R"(
            QCheckBox {
                spacing: 5px;
                color: #ffffff;
                font-size: 14px;
            }
            QCheckBox::indicator {
                width: 16px;
                height: 16px;
                border-radius: 3px;
                border: 2px solid #757575;
                background-color: #54687A;
            }
            QCheckBox::indicator:hover {
                border: 2px solid #54687A;
            }
            QCheckBox::indicator:checked {
                background-color: #60798B;
                border: 2px solid #ffffff;
            }
            QCheckBox::indicator:unchecked {
                background-color: #19232D;
                border: 2px solid #ffffff;
            }
        )");
        boxLayout->addWidget(isMasterEdit);
        isMasterEdits.append(isMasterEdit);

        // GroupBox 추가
        mainLayout->addWidget(cameraBox);
    }

    // Save Button
    QPushButton *saveButton = new QPushButton("Save", &dialog);
    saveButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, [&]() {
        QJsonArray updatedCameras;

        for (int i = 0; i < descriptionEdits.size(); ++i) {
            QJsonObject updatedCamera;
            updatedCamera["description"] = descriptionEdits[i]->text();
            updatedCamera["groupNumber"] = groupNumberEdits[i]->text().toInt();
            updatedCamera["ipAddr"] = ipAddressEdits[i]->text();
            updatedCamera["isMaster"] = isMasterEdits[i]->isChecked();

            updatedCameras.append(updatedCamera);
        }

        // 서버로 Patch 요청
        patchCameraInfo(updatedCameras);
        dialog.accept();
    });

    // Close Button
    QPushButton *closeButton = new QPushButton("Close", &dialog);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
    mainLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.setLayout(mainLayout);
    dialog.exec();
}

// 수정한 정보 서버로 PATCH 요청
void MediaApp::patchCameraInfo(const QJsonArray &updatedCameras)
{
    QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_camera_list";
    QUrl url(serverUrl);

    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    QJsonObject requestBody;
    requestBody["cameras"] = updatedCameras;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->sendCustomRequest(request, "PATCH", QJsonDocument(requestBody).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            showCustomMessage("Success", "Camera information updated successfully.");
        } else {
            showCustomMessage("Error", "Failed to update camera information.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}
/* ************************************************************************************************************ */
/* ************************************************************************************************************ */

/*
 * [ Upload Program 버튼 클릭 시 동작 ]
 *
 * 1. Upload에 필요한 fps, file name, description, file 입력 및 선택
 * 2. 입력한 정보를 바탕으로 서버로 POST 수행
 *
 */

// 파일 정보 입력
void MediaApp::uploadCameraProgram()
{
    // Input dialog for user to provide FPS, name, and description
    QDialog dialog(this, Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // FPS 입력
    QLabel *fpsLabel = new QLabel("FPS:", &dialog);
    fpsLabel->setStyleSheet("font-size: 14px; color: white;");
    QLineEdit *fpsInput = new QLineEdit(&dialog);
    fpsInput->setPlaceholderText("Enter FPS");

    // 구분선 추가
    QFrame *line1 = new QFrame(&dialog);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line1->setStyleSheet("color: #ffffff;");

    // 파일 이름 입력
    QLabel *nameLabel = new QLabel("File Name:", &dialog);
    nameLabel->setStyleSheet("font-size: 14px; color: white;");
    QLineEdit *nameInput = new QLineEdit(&dialog);
    nameInput->setPlaceholderText("Enter File Name");

    QFrame *line2 = new QFrame(&dialog);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line2->setStyleSheet("color: #ffffff;");

    // 설명 입력
    QLabel *descLabel = new QLabel("Description:", &dialog);
    descLabel->setStyleSheet("font-size: 14px; color: white;");
    QLineEdit *descInput = new QLineEdit(&dialog);
    descInput->setPlaceholderText("Enter Description");

    QFrame *line3 = new QFrame(&dialog);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    line3->setStyleSheet("color: #ffffff;");

    // 확인 버튼
    QPushButton *okButton = new QPushButton("OK", &dialog);
    okButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
    connect(okButton, &QPushButton::clicked, &dialog, [&]() {
        if (fpsInput->text().isEmpty() || nameInput->text().isEmpty() || descInput->text().isEmpty()) {
            showCustomMessage("Error", "All fields are required.");
            return;
        }

        dialog.accept();
        sendProgramToServer(fpsInput->text(), nameInput->text(), descInput->text());
    });

    // 취소 버튼
    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);
    cancelButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // 레이아웃 구성
    layout->addWidget(fpsLabel);
    layout->addWidget(fpsInput);
    layout->addWidget(line1);

    layout->addWidget(nameLabel);
    layout->addWidget(nameInput);
    layout->addWidget(line2);

    layout->addWidget(descLabel);
    layout->addWidget(descInput);
    layout->addWidget(line3);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout);

    dialog.setLayout(layout);
    dialog.exec();
}

// 서버로 POST 요청
void MediaApp::sendProgramToServer(const QString &fps, const QString &name, const QString &description)
{
    QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/post_program";
    QUrl url(serverUrl);

    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    QNetworkRequest request(url);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart fpsPart;
    fpsPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"fps\""));
    fpsPart.setBody(fps.toUtf8());

    QHttpPart namePart;
    namePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    namePart.setBody(name.toUtf8());

    QHttpPart descPart;
    descPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"description\""));
    descPart.setBody(description.toUtf8());

    multiPart->append(fpsPart);
    multiPart->append(namePart);
    multiPart->append(descPart);

    QNetworkReply *reply = networkManager->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            showCustomMessage("Success", "Program uploaded successfully.");
        } else {
            showCustomMessage("Error", "Failed to upload program.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}
/* ************************************************************************************************************ */
/* ************************************************************************************************************ */

/*
 * 마우스 동작 처리
 */
void MediaApp::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        m_dragging = true;
    }
}

void MediaApp::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        move(event->globalPosition().toPoint() - m_dragPosition);
    }
}

void MediaApp::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
}

/*
 * 창 크기 조절 및 채널 닫기
 */
void MediaApp::maximizeChannel(int channelNumber)
{
    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

    // 채널 번호에 해당하는 서브윈도우 찾기
    if (channelNumber < 1 || channelNumber > subWindows.size()) {
        showCustomMessage("Error", "Invalid channel number.");
        return;
    }

    QMdiSubWindow *targetWindow = subWindows.at(channelNumber - 1);

    // Empty 상태인지 확인
    QLabel *emptyLabel = targetWindow->findChild<QLabel *>();
    if (emptyLabel && emptyLabel->text() == "Empty Channel") {
        showCustomMessage("Empty Channel", QString("Channel %1 is empty.").arg(channelNumber));
        return;
    }

    // 서브윈도우 확대
    mdiArea->setActiveSubWindow(targetWindow);
    targetWindow->showMaximized();
}

void MediaApp::returnSize(int channelNumber)
{
    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

    // 채널 번호에 해당하는 서브윈도우 찾기
    if (channelNumber < 1 || channelNumber > subWindows.size()) {
        showCustomMessage("Error", "Invalid channel number.");
        return;
    }

    QMdiSubWindow *targetWindow = subWindows.at(channelNumber - 1);

    // Empty 상태인지 확인
    QLabel *emptyLabel = targetWindow->findChild<QLabel *>();
    if (emptyLabel && emptyLabel->text() == "Empty Channel") {
        showCustomMessage("Empty Channel", QString("Channel %1 is empty.").arg(channelNumber));
        return;
    }

    mdiArea->setActiveSubWindow(targetWindow);
    targetWindow->showNormal();
    targetWindow->resize(640, 480);
}

void MediaApp::closeChannel(int channelNumber)
{
    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();

    // 채널 번호에 해당하는 서브윈도우 찾기
    if (channelNumber < 1 || channelNumber > subWindows.size()) {
        showCustomMessage("Error", "Invalid channel number.");
        return;
    }

    QMdiSubWindow *targetWindow = subWindows.at(channelNumber - 1);

    // Empty 상태인지 확인
    QLabel *emptyLabel = targetWindow->findChild<QLabel *>();
    if (emptyLabel && emptyLabel->text() == "Empty Channel") {
        showCustomMessage("Empty Channel", QString("Channel %1 is already empty.").arg(channelNumber));
        return;
    }

    // "Empty Channel" 상태로 복구
    QWidget *placeholder = new QWidget(targetWindow);
    placeholder->setStyleSheet("background-color: black;");

    QLabel *newEmptyLabel = new QLabel("Empty Channel", placeholder);
    newEmptyLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    newEmptyLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->addWidget(newEmptyLabel);
    placeholder->setLayout(placeholderLayout);

    targetWindow->setWidget(placeholder);
    targetWindow->setWindowTitle("Empty Channel");
    targetWindow->resize(640, 480);
    targetWindow->showNormal();
}

// 오류 메세지 출력 다이얼로그
void MediaApp::showCustomMessage(const QString &title, const QString &message)
{
    QDialog customDialog(this, Qt::FramelessWindowHint | Qt::Dialog);
    customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(&customDialog);

    QLabel *titleLabel = new QLabel(title, &customDialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *messageLabel = new QLabel(message, &customDialog);
    messageLabel->setStyleSheet("font-size: 14px; color: white;");
    messageLabel->setAlignment(Qt::AlignCenter);

    QPushButton *okButton = new QPushButton("OK", &customDialog);
    okButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: none;
            border-radius: 5px;
            padding: 8px 8px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");

    connect(okButton, &QPushButton::clicked, &customDialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addWidget(okButton);

    customDialog.setLayout(layout);
    customDialog.exec();
}
