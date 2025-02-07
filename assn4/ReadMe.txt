shader.h , shader.cpp : GLSL 코드를 읽어와 shader를 aplication과 연결하는 class
shape.h , shape.cpp : object를 구성하는 기본 단위들에 대한 class
matrixStack.h , matrixStack.cpp :  shader에 넘겨줄 model view matrix 에 대한 stack class
gouraudFShader.glsl, gouraudVShader.glsl :  gouraud shading에 대한 vertex, fragment shader 코드
phongFShader.glsl, phongVShader.glsl :  gouraud shading에 대한 vertex, fragment shader 코드
assn4.cpp :  게임에 대한 전반적인 기능을 다루는 main 파일
textures : texture 사진들을 담은 폴더

플레이 방법:
방향키 상,하,좌,우를 통해 캐릭터를 움직여 길을 건넌다. 대각선으로 이동할 수 있으며, 아이템을 먹거나, 점프대를 이용할 수 있다. 화면 끝에 도착하면 종료.
방향키 : 이동
스페이스바: 점프
r키: rendering 모드 조정
e키: edge 가시모드 조정
q키: 게임 재시작
p키: 디버그모드
a : 왜곡 효과 적용
'x' : shaing 모드 변경
't' : diffuse texture 적용 유무 변경
'n': normal texture 적용 유무 변경