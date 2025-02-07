shader.h , shader.cpp : GLSL 코드를 읽어와 shader를 aplication과 연결하는 class
shape.h , shape.cpp : object를 구성하는 기본 단위들에 대한 class
matrixStack.h , matrixStack.cpp :  shader에 넘겨줄 model view matrix 에 대한 stack class
fShader.glsl, vShader.glsl :  vertex, fragment shader 코드
assn3_2.cpp :  게임에 대한 전반적인 기능을 다루는 main 파일

플레이 방법:
방향키 상,하,좌,우를 통해 캐릭터를 움직여 길을 건넌다. 대각선으로 이동할 수 있으며, 아이템을 먹거나, 점프대를 이용할 수 있다. 화면 끝에 도착하면 종료.
방향키 : 이동
스페이스바: 점프
r키: rendering 모드 조정
e키: edge 가시모드 조정
q키: 게임 재시작
p키: 디버그모드
a,s,d,f : 각각 시각적 효과 적용