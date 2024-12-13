# CAM-CLIENT
[QT Application]

Qt 프레임워크와 GStreamer를 활용한 이벤트 기반 감시 시스템

- 무인 매장 도난 방지 감시 솔루션

<Dependency(의존성)>
	
- QT Framework (>=5.15.0 or Qt 6.x) 
- GStreamer

<Project Structure(구조)>
	
	/src
	├── main.cpp             
	├── mediaapp.h/.cpp          # 메인 어플리케이션 로직	
	├── player.h/.cpp            # GStreamer를 사용하는 멀티미디어 플레이어
	├── DraggableDialog.h/.cpp   # 프레임리스 다이얼로그에 드래그 기능 구현
	├── SubWindow.h              # 4채널 영상 관리를 위한 SubWindow
	
<UI 및 동작>
1. 서버와의 데이터 통신을 통해 라이브 카메라 스트리밍(RTSP), 저장된 동영상 재생, 카메라 정보 조회 및 업데이트 기능 제공

![image](https://github.com/user-attachments/assets/824b2edb-de20-49af-b16d-80a83ece7a6c)
      
2. 실시간 스트리밍 - 서버로부터 카메라 IP와 포트번호를 받아와(GET 요청) RTSP URL 구성하고 해당 URL로 스트리밍 수행
      
- 최대 4채널 스트리밍 지원, 채널 크기 변경(확대/축소) 및 초기화 기능 제공

![image](https://github.com/user-attachments/assets/f0f55133-b8cb-4fe8-925a-179c916a135f)

![image](https://github.com/user-attachments/assets/87336833-5c49-45de-afe4-39a594046359)

![image](https://github.com/user-attachments/assets/75ed004d-6337-441e-a8e1-d0d995932363)

![image](https://github.com/user-attachments/assets/00a7ea93-e5af-4677-8e13-6eddd2eb23d2)

3. 서버에 저장된 동영상 재생 - 서버에서 저장된 비디오 목록과 URL(http~.mp4) 받아와(GET 요청) 사용자에게 표시
      
- 최대 4채널 영상 재생 지원, 목록에서 재생할 비디오를 선택하고 다운로드 여부 및 경로 선택

4. 카메라 정보 조회 - 서버로부터 카메라 정보(Description, Group number, IP address, Is Master) 조회(GET 요청)
      
- 정보 중 Group number와 Is Master 는 사용자가 편집하고 서버에 업데이트하는 기능 제공(PATCH)

![image](https://github.com/user-attachments/assets/faeb16d8-7ea7-409c-a80a-9b54d04e75b5)

5. 카메라에 프로그램 업로드 - 사용자로부터 fps, file name, description을 입력받아 서버로 POST 요청 수행

- 서버가 .so 파일 카메라로 전송

  ![image](https://github.com/user-attachments/assets/88e5f03e-a4de-46b3-a946-6986f5265eb8)
