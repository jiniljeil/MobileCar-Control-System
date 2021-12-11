## IoT 최종 설계 과제 
작성자: 강병석, 김진일 

이 프로젝트는 크게 Device와 Controller로 구성되어 있습니다. 

우선, QT 환경이 마련된 라즈베리파이에서 진행한다는 가정하에 설명하도록 하겠습니다. 

또한, 아래의 지시 사항을 Device와 Controller 각각 진행해야합니다.  

가장 먼저 다운로드 받은 압축 파일을 해제해줍니다. 

#### 프로젝트 실행 방법 

1. 배경화면에서 왼쪽 상단 라즈베리파이 아이콘을 클릭해줍니다. 
	-	아이콘 클릭 시 목록 중 Programming 이라는 항목이 존재 

2. Programming 이라는 항목에 존재하는 리스트를 보시면, QT Creator 라는 Tool 을 클릭해줍니다. 
	-	QT Creator 창이 뜨게 됨
	-	정상적으로 따라 했을 경우, QT Creator 창이 열리며 New Project, Open Project 등이 보임. 

3. QT Creator 창이 떴다면, New Project 오른쪽에 Open Project 라는 버튼을 클릭해줍니다. 
	-	Open File 이라는 창이 뜨게 됩니다. 

4. Open File 창이 떴다면, 자신이 Controller 혹은 Device를 다운로드 받아 저장해놓은 디렉터리로 이동해줍니다. 
	-	다운로드만 받았다는 가정하에 /home/pi/downloads 디렉터리에 해당 파일이 존재할 경우
		왼쪽 Computer와 pi 항목 중 pi를 클릭하여 /home/pi 디렉터리로 이동한 후, 
		/home/pi 위치에서 우측 리스트에 나오는 Downloads를 더블 클릭합니다. 

5. 다운로드 받은 Controller 혹은 Device 디렉터리로 이동하면, Device의 경우 Team5Device, Controller의 경우 5zoController 디렉터리로 이동했을 것 입니다. 

6. Device의 경우, pro File 타입인 Team5Device.pro 클릭한 후, Open을 클릭해줍니다. Controller의 경우, pro File 타입인 5zoController.pro 클릭한 후, Open을 클릭해줍니다. 

7. 현재까지 잘 따라왔을 경우, 왼쪽 상단에 Device는 Team5Device라는 폴더가 보이고, Controller는 5zoController라는 폴더가 보이게 됩니다. 

8. 이제 왼쪽 하단에 보시면, 초록색 세모 모양이 있는 것을 확인할 수 있습니다. 이때, Device 쪽을 먼저 진행한 후, Controller 쪽을 진행해주면 됩니다. 왼쪽 하단 밑에서 3번째 초록색 세모를 클릭해주면 됩니다. 
	-	초록색 세모 2개 중 벌레가 붙어있지 않은 초록색 세모를 클릭하시면 됩니다. 

9. 버튼을 클릭했을 경우, 실행이 되는 것을 확인할 수 있습니다.  
	-	Device가 먼저 실행되는 것을 확인한 후, Controller를 켜줍니다. 

