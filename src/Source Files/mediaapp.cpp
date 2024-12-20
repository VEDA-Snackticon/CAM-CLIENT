#include "mediaapp.h"

MediaApp::MediaApp(QWidget *parent)
    : QWidget(parent), networkManager(new QNetworkAccessManager(this)), outputFile(nullptr), player(new Player(this))
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    QFont font("Arial", 14);
    QApplication::setFont(font);

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

    // 제목
    QLabel *titleLabel = new QLabel("     P R O X I - S E N S E", titleBar);
    titleLabel->setStyleSheet("font-family: Arial; color: #19232D; font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 닫기 버튼
    QPushButton *closeButton = new QPushButton("X", titleBar);
    closeButton->setStyleSheet(R"(
        QPushButton {
            color: #19232D;
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

        QLabel *messageLabel = new QLabel("Do you want to quit \"SNACK GUARD\"?", &customDialog);
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
    titleBarLayout->addWidget(titleLabel);
    titleBarLayout->addWidget(closeButton);

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
        "   font-family: Arial;"
        "   background-color: #FAEBD7;"
        "   color: #19232D;"
        "   border: 2px solid #ea632a;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ea632a;"
        "}"
        );
    connect(streamButton, &QPushButton::clicked, this, &MediaApp::showStreamingArea);
    appLayout->addWidget(streamButton, 0, Qt::AlignCenter);

    videoButton = new QPushButton(tr("Saved Video"), this);
    videoButton->setFixedSize(150, 50);
    videoButton->setStyleSheet(
        "QPushButton {"
        "   font-family: Arial;"
        "   background-color: #FAEBD7;"
        "   color: #19232D;"
        "   border: 2px solid #ea632a;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ea632a;"
        "}"
        );
    connect(videoButton, &QPushButton::clicked, this, &MediaApp::showVideoArea);
    appLayout->addWidget(videoButton, 0, Qt::AlignCenter);

    eventButton = new QPushButton(tr("Event Log"), this);
    eventButton->setFixedSize(150, 50);
    eventButton->setStyleSheet(
        "QPushButton {"
        "   font-family: Arial;"
        "   background-color: #FAEBD7;"
        "   color: #19232D;"
        "   border: 2px solid #ea632a;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ea632a;"
        "}"
        );
    connect(eventButton, &QPushButton::clicked, this, &MediaApp::fetchEventLog);
    appLayout->addWidget(eventButton, 0, Qt::AlignCenter);

    fetchInfoButton = new QPushButton(tr("Camera Info"), this);
    fetchInfoButton->setFixedSize(150, 50);
    fetchInfoButton->setStyleSheet(
        "QPushButton {"
        "   font-family: Arial;"
        "   background-color: #FAEBD7;"
        "   color: #19232D;"
        "   border: 2px solid #ea632a;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ea632a;"
        "}"
        );
    connect(fetchInfoButton, &QPushButton::clicked, this, &MediaApp::fetchCameraInfo);
    appLayout->addWidget(fetchInfoButton, 0, Qt::AlignCenter);

    QPushButton *uploadButton = new QPushButton(tr("Upload Program"), this);
    uploadButton->setFixedSize(150, 50);
    uploadButton->setStyleSheet(
        "QPushButton {"
        "   font-family: Arial;"
        "   background-color: #FAEBD7;"
        "   color: #19232D;"
        "   border: 2px solid #ea632a;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F5DEB3;"
        "   color: #8B0000;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #ea632a;"
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

    DraggableDialog *mdiDialog = new DraggableDialog(this);
    mdiDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    mdiDialog->resize(1302, 1018);

    QVBoxLayout *mdiLayout = new QVBoxLayout(mdiDialog);

    // "Add Camera" 버튼
    QPushButton *addStreamButton = new QPushButton("+ Add Camera", mdiDialog);
    addStreamButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    connect(addStreamButton, &QPushButton::clicked, this, &MediaApp::addStreamWindow);

    // 채널 번호 입력
    QLineEdit *channelInput = new QLineEdit(mdiDialog);
    channelInput->setPlaceholderText("Channel Number");
    channelInput->setFixedWidth(150);

    // 확대
    QPushButton *maximizeButton = new QPushButton("Maximize", mdiDialog);
    maximizeButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    restoreButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    closeButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    exitButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    connect(exitButton, &QPushButton::clicked, mdiDialog, [mdiDialog]() {
        // 커스텀 다이얼로그 생성
        QDialog customDialog(mdiDialog, Qt::FramelessWindowHint | Qt::Dialog);
        customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&customDialog);

        // 메시지 라벨 추가
        QLabel *messageLabel = new QLabel("Do you want to close this STREAMING channel?", &customDialog);
        messageLabel->setStyleSheet("font-family: Arial; font-size: 16px; font-weight: bold; color: white;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);

        // 버튼 추가
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *yesButton = new QPushButton("Yes", &customDialog);
        yesButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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
            font-family: Arial;
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
        emptyLabel->setStyleSheet("font-family: Arial; color: white; font-size: 18px; font-weight: bold;");
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
    // QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_camera_list";
    QString serverUrl = "http://snackticon.iptime.org/cameraList";
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
            QJsonArray cameraArray = jsonResponse.object()["cameras"].toArray();

            cameraIPs.clear();
            for (const QJsonValue &value : cameraArray) {
                QJsonObject cameraObject = value.toObject();
                QString ip = cameraObject["ipAddr"].toString();
                if (!ip.isEmpty()) {
                    cameraIPs.append(ip);
                }
            }

            if (cameraIPs.isEmpty()) {
                showCustomMessage("Error", "No camera IPs available.");
                return;
            }

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
                    font-family: Arial;
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
                    font-family: Arial;
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
            showCustomMessage("Error", "Failed to fetch camera information.\nError: " + reply->errorString());
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
    QString portNum = "8550";
    QString rtspUrl = "rtsp://" + cameraIp + ":" + portNum + "/snackticon";
    player->setUri(rtspUrl);
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

    DraggableDialog *mdiDialog = new DraggableDialog(this);
    mdiDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    mdiDialog->resize(1302, 1023);

    QVBoxLayout *mdiLayout = new QVBoxLayout(mdiDialog);

    // 비디오 리스트 확인
    QPushButton *addVideoButton = new QPushButton("Video Lists", mdiDialog);
    addVideoButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    connect(addVideoButton, &QPushButton::clicked, this, &MediaApp::addVideoWindow);

    // 채널 번호 입력
    QLineEdit *channelInput = new QLineEdit(mdiDialog);
    channelInput->setPlaceholderText("Slot Number");
    channelInput->setFixedWidth(150);

    // 확대
    QPushButton *maximizeButton = new QPushButton("Maximize Video", mdiDialog);
    maximizeButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    restoreButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    closeButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
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
    exitButton->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    connect(exitButton, &QPushButton::clicked, mdiDialog, [mdiDialog]() {
        // 커스텀 다이얼로그 생성
        QDialog customDialog(mdiDialog, Qt::FramelessWindowHint | Qt::Dialog);
        customDialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

        QVBoxLayout *layout = new QVBoxLayout(&customDialog);

        // 메시지 라벨 추가
        QLabel *messageLabel = new QLabel("Do you want to close this VIDEO channel?", &customDialog);
        messageLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: white;");
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);

        // 버튼 추가
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *yesButton = new QPushButton("Yes", &customDialog);
        yesButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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
            font-family: Arial;
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

        QLabel *emptyLabel = new QLabel("Empty Channel", placeholder);
        emptyLabel->setStyleSheet("font-family: Arial; color: white; font-size: 18px; font-weight: bold;");
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
        if (emptyLabel && emptyLabel->text() == "Empty Channel") {
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
    QString serverUrl = "http://snackticon.iptime.org/file";
    QUrl url(serverUrl);

    if (!url.isValid()) {
        showCustomMessage("Error", "Invalid server URL.");
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, channelIndex, targetWindow]() {
        if (reply->error() == QNetworkReply::NoError) {
            // JSON 응답 처리
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonArray videoArray = jsonResponse.object()["files"].toArray();

            videoUrls.clear();

            QDialog dialog(this, Qt::FramelessWindowHint | Qt::Dialog);
            dialog.setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

            QVBoxLayout *layout = new QVBoxLayout;

            QVector<QRadioButton *> radioButtons;

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
                QString fileName = videoObject["fileName"].toString();
                QString time = videoObject["time"].toString();

                if (!fileName.isEmpty()) {
                    QString fullUrl = QString("http://snackticon.iptime.org/files?filename=%1").arg(fileName);
                    videoUrls.append(fullUrl);

                    QRadioButton *radioButton = new QRadioButton(fileName, &dialog);
                    radioButton->setStyleSheet(radioStyle);
                    layout->addWidget(radioButton);
                    radioButtons.append(radioButton);
                }
            }

            QCheckBox *downloadCheckBox = new QCheckBox("Download", &dialog);
            downloadCheckBox->setStyleSheet(R"(
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
            )");
            layout->addWidget(downloadCheckBox);

            QPushButton *okButton = new QPushButton("OK", &dialog);
            okButton->setStyleSheet(R"(
                QPushButton {
                    font-family: Arial;
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

            QPushButton *cancelButton = new QPushButton("Cancel", &dialog);
            cancelButton->setStyleSheet(R"(
                QPushButton {
                    font-family: Arial;
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

            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);

            connect(okButton, &QPushButton::clicked, [&]() {
                QString selectedUrl;
                for (int i = 0; i < radioButtons.size(); ++i) {
                    if (radioButtons[i]->isChecked()) {
                        selectedUrl = videoUrls[i];
                        break;
                    }
                }

                if (downloadCheckBox->isChecked()) {
                    QString savePath = QFileDialog::getSaveFileName(this, "Save Video", "", "MP4 Files (*.mp4);;All Files (*)");
                    if (savePath.isEmpty()) {
                        showCustomMessage("Save Cancelled", "No storage path has been selected");
                        return;
                    }
                    downloadVideo(selectedUrl, savePath);
                }

                if (selectedUrl.isEmpty()) {
                    showCustomMessage("Error", "Please select video.");
                    return;
                }

                playVideoInMdiWindow(channelIndex, selectedUrl, targetWindow);
                dialog.accept();
            });

            connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

            layout->addLayout(buttonLayout);
            dialog.setLayout(layout);
            dialog.exec();
        } else {
            showCustomMessage("Error", "Failed to fetch video list.\nError: " + reply->errorString());
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
 * [ Event Log 버튼 클릭 시 동작 ]
 *
 * 1. {서버 URL}/event 에서 지금까지 발생한 이벤트의 time stamp를 받아와 표시
 *
 */
void MediaApp::fetchEventLog()
{
    QString serverUrl = "http://snackticon.iptime.org/event";
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
            QJsonArray eventList = jsonResponse.object()["events"].toArray();

            // 시간 순으로 정렬
            sortEventLogByTime(eventList);

            // 카메라 정보를 UI에 표시
            displayEventLog(eventList);

        } else {
            showCustomMessage("Error", "Failed to fetch event log.\nError: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

// event list 시간순 정렬
void MediaApp::sortEventLogByTime(QJsonArray &eventList)
{
    // QVector로 변환 (정렬을 위해)
    QVector<QJsonObject> events;
    for (const QJsonValue &value : eventList) {
        events.append(value.toObject());
    }

    // 정렬: time 필드를 기준으로
    std::sort(events.begin(), events.end(), [](const QJsonObject &a, const QJsonObject &b) {
        QString timeA = a["time"].toString();
        QString timeB = b["time"].toString();

        // QDateTime으로 변환
        QDateTime dateTimeA = QDateTime::fromString(timeA, "yyyy-MM-dd HH:mm:ss");
        QDateTime dateTimeB = QDateTime::fromString(timeB, "yyyy-MM-dd HH:mm:ss");

        return dateTimeA < dateTimeB; // 오름차순 정렬
    });

    // QVector를 다시 QJsonArray로 변환
    eventList = QJsonArray();
    for (const QJsonObject &event : events) {
        eventList.append(event);
    }
}


// 받아온 정보 표시
void MediaApp::displayEventLog(const QJsonArray &eventList)
{
    DraggableDialog *dialog = new DraggableDialog(this);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Event display container
    QVBoxLayout *eventLayout = new QVBoxLayout;
    QWidget *eventContainer = new QWidget;
    eventContainer->setLayout(eventLayout);

    for (const QJsonValue &value : eventList) {
        QJsonObject event = value.toObject();

        // Event time display
        QLabel *timeLabel = new QLabel(event["time"].toString());
        timeLabel->setStyleSheet("font-family: Arial; font-size: 14px; color: #ffffff;");
        eventLayout->addWidget(timeLabel);

        // Add separator
        QFrame *separator = new QFrame;
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet("color: #ffffff;");
        eventLayout->addWidget(separator);
    }

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(eventContainer);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Filter Button
    QPushButton *filterButton = new QPushButton("Filter", dialog);
    filterButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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

    connect(filterButton, &QPushButton::clicked, this, [this, &eventList, eventLayout]() {
        QDialog customDialog(this, Qt::FramelessWindowHint | Qt::Dialog);

        QVBoxLayout *filterLayout = new QVBoxLayout(&customDialog);

        QLabel *label = new QLabel("Enter time range (HH:MM):", &customDialog);
        filterLayout->addWidget(label);

        QLineEdit *startTimeInput = new QLineEdit(&customDialog);
        startTimeInput->setPlaceholderText("From");
        filterLayout->addWidget(startTimeInput);

        QLineEdit *endTimeInput = new QLineEdit(&customDialog);
        endTimeInput->setPlaceholderText("To");
        filterLayout->addWidget(endTimeInput);

        QPushButton *applyFilterButton = new QPushButton("Apply", &customDialog);
        filterLayout->addWidget(applyFilterButton);

        connect(applyFilterButton, &QPushButton::clicked, &customDialog, [this, &eventList, eventLayout, startTimeInput, endTimeInput, &customDialog]() {
            QString startTime = startTimeInput->text();
            QString endTime = endTimeInput->text();

            if (startTime.isEmpty() || endTime.isEmpty()) {
                showCustomMessage("Input Error", "Please enter both start and end times.");
                return;
            }

            QTime start = QTime::fromString(startTime, "hh:mm");
            QTime end = QTime::fromString(endTime, "hh:mm");

            if (!start.isValid() || !end.isValid()) {
                showCustomMessage("Input Error", "Please enter valid times in the format HH:MM.");
                return;
            }

            if (start > end) {
                showCustomMessage("Input Error", "Start time must be before or equal to end time.");
                return;
            }

            // Clear previous event display
            QLayoutItem *item;
            while ((item = eventLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }

            // Display filtered events
            bool eventFound = false;
            for (const QJsonValue &value : eventList) {
                QJsonObject event = value.toObject();

                // Parse event time as QDateTime
                QDateTime eventDateTime = QDateTime::fromString(event["time"].toString(), "yyyy-MM-dd HH:mm:ss");

                if (eventDateTime.isValid()) {
                    QTime eventTime = eventDateTime.time();

                    // Check if event time falls within the given range
                    if (eventTime >= start && eventTime <= end) {
                        QLabel *timeLabel = new QLabel(event["time"].toString());
                        timeLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
                        eventLayout->addWidget(timeLabel);

                        QFrame *separator = new QFrame;
                        separator->setFrameShape(QFrame::HLine);
                        separator->setStyleSheet("color: #ffffff;");
                        eventLayout->addWidget(separator);

                        eventFound = true;
                    }
                }
            }

            if (!eventFound) {
                QLabel *noEventLabel = new QLabel("No events found for the selected time range.");
                noEventLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
                eventLayout->addWidget(noEventLabel);
            }


            customDialog.accept();
        });

        customDialog.exec();
    });

    // Close Button
    QPushButton *closeButton = new QPushButton("Close", dialog);
    closeButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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

    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(filterButton);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    dialog->setLayout(mainLayout);
    dialog->exec();
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
    QString serverUrl = "http://snackticon.iptime.org/cameraList";
    // QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/get_camera_list";
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
    DraggableDialog *dialog = new DraggableDialog(this);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVector<QLineEdit *> groupNumberEdits;
    QVector<QCheckBox *> isMasterEdits;

    for (const QJsonValue &value : cameraList) {
        QJsonObject camera = value.toObject();

        // GroupBox 생성
        QGroupBox *cameraBox = new QGroupBox("Camera Info", dialog);
        cameraBox->setStyleSheet(R"(
            QGroupBox {
                font-family: Arial;
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
        descriptionLabel->setStyleSheet("font-family: Arial; font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(descriptionLabel);

        QLabel *descriptionEdit = new QLabel(camera["description"].toString(), cameraBox);
        boxLayout->addWidget(descriptionEdit);

        // Group Number
        QLabel *groupNumberLabel = new QLabel("Group Number:", cameraBox);
        groupNumberLabel->setStyleSheet("font-family: Arial; font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(groupNumberLabel);

        QLineEdit *groupNumberEdit = new QLineEdit(QString::number(camera["groupNumber"].toInt()), cameraBox);
        boxLayout->addWidget(groupNumberEdit);
        groupNumberEdits.append(groupNumberEdit);

        // IP Address
        QLabel *ipAddrLabel = new QLabel("IP Address:", cameraBox);
        ipAddrLabel->setStyleSheet("font-family: Arial; font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(ipAddrLabel);

        QLabel *ipAddrEdit = new QLabel(camera["ipAddr"].toString(), cameraBox);
        boxLayout->addWidget(ipAddrEdit);

        // Is Master
        QLabel *isMasterLabel = new QLabel("Is Master:", cameraBox);
        isMasterLabel->setStyleSheet("font-family: Arial; font-weight: bold; font-size: 14px; color: #ffffff;");
        boxLayout->addWidget(isMasterLabel);

        QCheckBox *isMasterEdit = new QCheckBox(cameraBox);
        bool isMaster;
        if (camera["isMaster"].toInt() == 1) {
            isMaster = true;
        }
        else
            isMaster = false;
        qInfo() << "Is Master for camera: " << camera["description"].toString() << " = " << isMaster;

        isMasterEdit->setChecked(isMaster);
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
    QPushButton *saveButton = new QPushButton("Save", dialog);
    saveButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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

    connect(saveButton, &QPushButton::clicked, [&, cameraList]() {
        QJsonArray updatedCameras;

        for (int i = 0; i < groupNumberEdits.size(); ++i) {
            QJsonObject originalCamera = cameraList[i].toObject();
            QJsonObject updatedCamera;

            bool isModified = false;

            // Group Number 비교
            int originalGroupNumber = originalCamera["groupNumber"].toInt();
            int newGroupNumber = groupNumberEdits[i]->text().toInt();
            if (originalGroupNumber != newGroupNumber) {
                updatedCamera["groupNumber"] = newGroupNumber;
                isModified = true;
            }

            // Is Master 비교
            int originalIsMaster = originalCamera["isMaster"].toInt();
            bool originalIsMaster_bool;
            if (originalIsMaster == 1)
                originalIsMaster_bool = true;
            else
                originalIsMaster_bool = false;
            bool newIsMaster = isMasterEdits[i]->isChecked();
            if (originalIsMaster_bool != newIsMaster) {
                if (newIsMaster == true)
                    updatedCamera["isMaster"] = 1;
                else
                    updatedCamera["isMaster"] = 0;

                isModified = true;
            }

            // Description 및 IP Address는 원본 데이터를 유지
            updatedCamera["description"] = originalCamera["description"].toString();
            updatedCamera["ipAddr"] = originalCamera["ipAddr"].toString();

            // 변경된 항목만 추가
            if (isModified) {
                updatedCameras.append(updatedCamera);
            }
        }

        // 변경된 항목이 있는 경우에만 Patch 요청
        if (!updatedCameras.isEmpty()) {
            patchCameraInfo(updatedCameras);
        } else {
            showCustomMessage("Info", "No changes were made.");
        }

        dialog->accept();
    });

    // Close Button
    QPushButton *closeButton = new QPushButton("Close", dialog);
    closeButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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

    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    dialog->setLayout(mainLayout);
    dialog->exec();
}

// 수정한 정보 서버로 PATCH 요청
void MediaApp::patchCameraInfo(const QJsonArray &updatedCameras)
{
    QString baseUrl = "http://snackticon.iptime.org/cameraInfo";
    for (const QJsonValue &value : updatedCameras) {
        QJsonObject cameraData = value.toObject();

        // Ensure the required fields are present
        if (!cameraData.contains("description") || !cameraData.contains("ipAddr")) {
            showCustomMessage("Error", "Invalid camera data.");
            continue;
        }

        QString ipAddr = cameraData["ipAddr"].toString();
        QUrl url(baseUrl);

        if (!url.isValid()) {
            showCustomMessage("Error", "Invalid URL for camera: " + ipAddr);
            continue;
        }

        // Prepare request body
        QJsonObject requestBody;
        requestBody["description"] = cameraData["description"];
        requestBody["groupNumber"] = cameraData["groupNumber"];
        requestBody["isMaster"] = cameraData["isMaster"];

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *reply = networkManager->sendCustomRequest(request, "PATCH", QJsonDocument(requestBody).toJson());

        connect(reply, &QNetworkReply::finished, this, [this, reply, ipAddr]() {
            if (reply->error() == QNetworkReply::NoError) {
                showCustomMessage("Success", "Camera information for " + ipAddr + " updated successfully.");
            } else {
                showCustomMessage("Error", "Failed to update camera information for " + ipAddr + ".\nError: " + reply->errorString());
            }
            reply->deleteLater();
        });
    }
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
    // 사용자에게 FPS, name, and description 입력 받는 Dialog
    DraggableDialog *dialog = new DraggableDialog(this);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setStyleSheet("background-color: #19232D; color: #ffffff; border-radius: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // FPS 입력
    QLabel *fpsLabel = new QLabel("FPS:", dialog);
    fpsLabel->setStyleSheet("font-family: Arial; font-size: 14px; font-weight: bold; color: white;");
    QLineEdit *fpsInput = new QLineEdit(dialog);
    fpsInput->setPlaceholderText("Enter FPS");

    // 파일 이름 입력
    QLabel *nameLabel = new QLabel("File Name:", dialog);
    nameLabel->setStyleSheet("font-family: Arial; font-size: 14px; font-weight: bold; color: white;");
    QLineEdit *nameInput = new QLineEdit(dialog);
    nameInput->setPlaceholderText("Enter File Name");

    // 설명 입력
    QLabel *descLabel = new QLabel("Description:", dialog);
    descLabel->setStyleSheet("font-family: Arial; font-size: 14px; font-weight: bold; color: white;");
    QLineEdit *descInput = new QLineEdit(dialog);
    descInput->setPlaceholderText("Enter Description");

    // 파일 선택
    QLabel *fileLabel = new QLabel("Select File:", dialog);
    fileLabel->setStyleSheet("font-size: 14px; color: white;");
    QPushButton *fileButton = new QPushButton("Choose File", dialog);
    fileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #455364;
            color: #ffffff;
            border: 1px solid #60798B;
            border-radius: 5px;
            padding: 5px 10px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #54687A;
        }
        QPushButton:pressed {
            background-color: #60798B;
        }
    )");
    QLabel *filePathLabel = new QLabel(dialog);

    QString filePath;
    connect(fileButton, &QPushButton::clicked, [&]() {
        filePath = QFileDialog::getOpenFileName(this, "Select File", "", "All Files (*)");
        filePathLabel->setText(filePath.isEmpty() ? "No file selected" : filePath);
    });

    // 확인 버튼
    QPushButton *okButton = new QPushButton("OK", dialog);
    okButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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
    connect(okButton, &QPushButton::clicked, dialog, [&]() {
        if (fpsInput->text().isEmpty() || nameInput->text().isEmpty() || descInput->text().isEmpty()) {
            showCustomMessage("Error", "All fields are required.");
            return;
        }

        dialog->accept();
        sendProgramToServer(fpsInput->text(), nameInput->text(), descInput->text());
    });

    // 취소 버튼
    QPushButton *closeButton = new QPushButton("Close", dialog);
    closeButton->setStyleSheet(R"(
        QPushButton {
            font-family: Arial;
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
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::reject);

    // 레이아웃 구성
    layout->addWidget(fpsLabel);
    layout->addWidget(fpsInput);

    layout->addWidget(nameLabel);
    layout->addWidget(nameInput);

    layout->addWidget(descLabel);
    layout->addWidget(descInput);

    layout->addWidget(fileLabel);
    layout->addWidget(fileButton);
    layout->addWidget(filePathLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    dialog->setLayout(layout);
    dialog->exec();
}

// 서버로 POST 요청
void MediaApp::sendProgramToServer(const QString &fps, const QString &name, const QString &description)
{
    QString serverUrl = "http://snackticon.iptime.org/program";
    // QString serverUrl = "https://b9d1644e-063c-44a9-9607-fc86dc9a3842.mock.pstmn.io/post_program";
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

// 채널 크기 최대화
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

// 사이즈 원상복귀 (640*480)
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

// 채널 닫고 Empty Channel 상태로
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

// 오류 메세지 출력 다이얼로그 (QMessageBox 대체)
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

