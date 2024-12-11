# CAM-CLIENT
QT Application

[[ Qt 프레임워크와 GStreamer를 활용해 실시간 스트리밍 및 동영상 모니터링 시스템 ]]

![image](https://github.com/user-attachments/assets/d05da136-351b-4c22-a325-8e8fd92fb21c)

			<시작 화면>
   . 서버와의 데이터 통신을 통해 라이브 카메라 스트리밍(RTSP), 저장된 동영상 재생, 카메라 정보 조회 및 업데이트 기능 제공

1. 실시간 스트리밍 - 서버로부터 카메라 IP와 포트번호를 받아와(GET 요청) RTSP URL 구성하고 해당 URL로 스트리밍 수행
		최대 4채널 스트리밍 지원, 채널 크기 변경(확대/축소) 및 초기화 기능 제공

2. 서버에 저장된 동영상 재생 - 서버에서 저장된 비디오 목록과 URL(http~.mp4) 받아와(GET 요청) 사용자에게 표시
		최대 4채널 영상 재생 지원, 목록에서 재생할 비디오를 선택하고 다운로드 여부 및 경로 선택

3. 카메라 정보 조회 - 서버로부터 카메라 정보(Description, Group number, IP address, Is Master) 조회(GET 요청)
		정보 중 Group number와 Is Master 는 사용자가 편집하고 서버에 업데이트하는 기능 제공(PATCH)

4. 카메라에 프로그램 업로드 - 사용자로부터 fps, file name, description을 입력받아 서버로 POST 요청 수행 (서버가 .so 파일 카메라로 			전송)
