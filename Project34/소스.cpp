#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <random>

typedef struct Box {
	glm::vec3 Bscale;
	glm::vec3 Blocate;

	glm::vec3 Color;
	int offset;
}Box;

typedef struct Player {
	glm::vec3 Pscale;
	glm::vec3 Plocate;
	glm::vec3 PColor;
	glm::vec3 Move;

	glm::vec3 cameraPos;
	glm::vec3 cameraDirection;

	glm::vec3 collider_box[2];

	int x, y, z;
	int prev_locate[2];
	float lotate;
	float gravity;

	float speed;
	bool view;
}Player;

void make_vertexShader();
void make_fragmentShader();
void InitBuffer();
void InitShader();
void TimerFunction(int value);
void SKeyDownboard(int key, int x, int y);
void SKeyUpboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid drawScene();
GLvoid Keyboard(unsigned char key, int x, int y);
char* filetobuf(const char* file);
void Boxinit(int x, int y);
void Draw_filed();
void playerinit();
void Box_crash(Player* p);
void player_Camera(Player* p);



GLchar* vertexsource, * fragmentsource; //--- 소스코드 저장 변수
GLuint vertexshader, fragmentshader; //--- 세이더 객체
GLuint s_program;
GLint result;
GLuint vao, vbo[2], EBO[2], linevbo[2], linevao;

float Cameraz = 15.0f;
float Camerax = 0.0f;
float ySelfRot = 0.0f;
float yZeroRot = 0.0f;
float Viewz = -1.0f;
float Viewy = 15.0f;
float xScale;
float yScale;
float zScale;
float BlockSpeed = 0.0f;

bool ra, Drop = true, What_view, Mani,Vani,rani,rminus,ani_1, ani_3;
int xcount = 20, Ycount = 20;
float yz = 0.0f;
Box All_Box[20][20];
Player player;

GLUquadricObj* qobj = gluNewQuadric();

glm::vec3 cameraPos;
glm::vec3 cameraPos_1;
glm::vec3 cameraDirection; //--- 카메라 바라보는 방향
glm::vec3 cameraDirection_1;
glm::vec3 cameraUp; //--- 카메라 위쪽 방향

glm::vec3 originPos;

float vPositionList[] = {
0.5f, 1.0f, 0.5f, // 앞 우측 상단
0.5f, 1.0f, -0.5f, // 뒤 우측 상단
-0.5f, 1.0f, -0.5f, // 뒤 좌측 상단
-0.5f, 1.0f, 0.5f, // 앞 좌측 상단
-0.5f,0.0f,0.5f, // 앞 좌측 하단
-0.5f,0.0f,-0.5f,	// 뒤 좌측 하단
0.5f,0.0f,-0.5f,	// 뒤 우측 하단
0.5f,0.0f,0.5f,		// 앞 우측 하단

0.5f,0.0f,0.5f,
0.5f,0.0f,-0.5f,
-0.5f,0.0f,0.5f, // 앞 좌측 하단
-0.5f,0.0f,-0.5f,	// 뒤 좌측 하단
0.0f,0.5f,0.0f
};

unsigned int index[] = {
1, 3, 0, // 첫 번째 삼각형			// 위 사각형
1, 2, 3, // 두 번째 삼각형
5, 4, 2,									// 왼쪽 사각형
3, 2, 4,
0, 7, 6,							// 오른쪽 사각형
6, 1, 0,
3, 4, 7,							// 앞 사각형
7, 0, 3,
6, 5, 2,								// 뒤 사각형
6, 2, 1,
6, 7, 4,	// 세 번째 삼각형			// 아래 사각형
6, 4, 5,	// 네 번째 삼각형

8,9,10,
10,9,11,
10,8,12,		// 앞 삼각형
8,9,12,			// 오른쪽
9,11,12,		// 뒤쪽
11,10,12		// 왼쪽
};

float line[]{
5.0f,0.0f,0.0f,
-5.0f,0.0f,0.0f,
0.0f,5.0f,0.0f,
0.0f,-5.0f,0.0f,
0.0f,0.0f,5.0f,
0.0f,0.0f,-5.0f
};

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1260, 700);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;

	Boxinit(xcount, Ycount);
	glewInit();
	InitShader();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수 지정
	/*glutKeyboardUpFunc();*/
	glutSpecialFunc(SKeyDownboard);
	glutSpecialUpFunc(SKeyUpboard);
	glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();
}

void make_vertexShader() {
	vertexsource = filetobuf("vertex.glsl");//--- 버텍스 세이더 객체 만들기
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexshader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLchar errorLog[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexshader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShader() {
	fragmentsource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentshader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLchar errorLog[512];
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void InitShader()
{
	make_vertexShader(); //--- 버텍스 세이더 만들기
	make_fragmentShader(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);

	GLchar errorLog[512];
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
	//--- 세이더 삭제하기
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
	//--- Shader Program 사용하기
	glUseProgram(s_program);
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기
	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]); //--- GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &linevao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(linevao); //--- VAO를 바인드하기
	glGenBuffers(2, linevbo); //--- 2개의 VBO를 지정하고 할당하기
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	cameraPos = glm::vec3(Camerax, 17.0f, Cameraz);
	cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 바라보는 방향
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

}

void drawScene() {
	glUseProgram(s_program);
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	glm::mat4 Prev_rotation = glm::mat4(1.0f);


	glm::mat4 view = glm::mat4(1.0f);

	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- 뷰잉 변환 설정
	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z축에 대하여 회전 행렬
	projection = glm::translate(projection, glm::vec3(0.0, -3.0, 15.0)); //--- 공간을 약간 뒤로 미뤄줌
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	Draw_filed();
	glViewport(830, 500, 200, 200);

	Draw_filed();
	glViewport(630, 0, 630, 700);


	view = glm::mat4(1.0f);


	projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0)); 
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3)); //--- 공간을 약간 뒤로 미뤄줌
	projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	
	view = glm::lookAt(player.cameraPos, player.cameraPos + player.cameraDirection, cameraUp);
	

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	Draw_filed();

	glViewport(0, 0, 630, 700);

	Draw_filed();
	glViewport(430, 500, 200, 200);



	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

void TimerFunction(int value) {

	player.Move[0] += cos(glm::radians(-player.lotate)) * 0.02 * player.x;			// 플레이어 좌우 움직임 할때 쓰는 계산
	player.Move[2] += sin(glm::radians(-player.lotate)) * 0.02 * player.x;
	player.Move[0] += sin(glm::radians(player.lotate)) * 0.02 * player.z;			// 플레이어 위 아래 움직임 할때 쓰는 계산
	player.Move[2] += cos(glm::radians(player.lotate)) * 0.02 * player.z;
	Box_crash(&player);																// 움직이고나서 해당 위치 블록 체크	(현재는 배열로 체크하는 중. 배열로 체크시 각 꼭짓점마다 이전 위치 저장해줘야할듯?)
	player_Camera(&player);																// 플레이어 카메라 위치 계산.

	glutTimerFunc(50, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'a') {
			player.lotate += 10;					// 플레이어 돌리기
	}
	else if (key == 'd') {
		player.lotate -= 10;
	}
	else if (key == 'q')
		exit(0);
	glutPostRedisplay();
}

void SKeyDownboard(int key, int x, int y) {
	switch (key) {
	
	case GLUT_KEY_LEFT:
		player.x = -1;
		break;
	case GLUT_KEY_RIGHT:
		player.x = +1;
		break;
	case GLUT_KEY_UP:
		player.z = -1;
		break;
	case GLUT_KEY_DOWN:
		player.z = +1;
	}
}

void SKeyUpboard(int key, int x, int y) {
	switch (key) {

	case GLUT_KEY_LEFT:
		player.x = 0;

		break;
	case GLUT_KEY_RIGHT:
		player.x = 0;
		break;
	case GLUT_KEY_UP:
		player.z = 0;

		break;
	case GLUT_KEY_DOWN:
		player.z = 0;

	}
}

void Boxinit(int x, int y) {				// 박스 갯수 추후에 25/25로 늘려도 박스 배열만 변경해주고 x,z값만 25로 넘겨주면 25*25 박스들이 만들어짐
	xScale = (float)10 / x;
	yScale = (float)10 / y;
	zScale = (float)10 / y;
	float xlocate = 5 - (xScale / 2);
	float zlocate = 5 - (zScale / 2);

	int a = 0;
	for (int i = 0; i < Ycount; i = i++,a++) {
		for (int k = 0; k < x; k++) {
			All_Box[i][k].Bscale[0] = xScale;
			All_Box[i][k].Bscale[1] = yScale;
			All_Box[i][k].Bscale[2] = zScale;
			All_Box[i][k].Color = 0;
			All_Box[i][k].Blocate[0] = xlocate - xScale * k;
			All_Box[i][k].Blocate[1] = -yScale;
			All_Box[i][k].Blocate[2] = zlocate - zScale * a;

			All_Box[i][k].offset = i * k;

		}

	}

	playerinit();
}


void Draw_filed() {
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	unsigned int modelLocation1 = glGetUniformLocation(s_program, "in_Color"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기

	glBindVertexArray(vao);

	for (int i = 0; i < Ycount; i = i++) {					// 박스를 그려주는 부분.
		for (int j = 0; j < xcount; j++) {
			glm::mat4 TR = glm::mat4(1.0f);							
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);

			if (All_Box[i][j].Color == 0) {
				glUniform3f(modelLocation1, 0.0f, 0.0f, 0.0f);
			}
			else if (All_Box[i][j].Color == 1) {
				glUniform3f(modelLocation1, 1.0f, 0.0f, 0.0f);
			}
			else {
				glUniform3f(modelLocation1, .0f, 1.0f, 1.0f);
			}
			Scale = glm::scale(Scale, glm::vec3(All_Box[i][j].Bscale)); //		각 사각형 크기 
			Tx = glm::translate(Tx, All_Box[i][j].Blocate);

			TR = Tx * Scale * TR;

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
		}
	}

	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);



		TR = glm::mat4(1.0f);					// 플레이어를 그려주는 부분.
		Tx = glm::mat4(1.0f);
		Scale = glm::mat4(1.0f);
		glm::mat4 Rotate = glm::mat4(1.0f);

		glUniform3f(modelLocation1, player.PColor[0], player.PColor[1], player.PColor[2]);

		Rotate = glm::rotate(Rotate, glm::radians(player.lotate), glm::vec3(0.0, 1.0, 0.0)); //--- z축에 대하여 회전 행렬
		Scale = glm::scale(Scale, player.Pscale); //		플레이어
		Tx = glm::translate(Tx, player.Plocate + player.Move);
		TR = Tx * Rotate * Scale * TR;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
	
}

void playerinit() {
	player.PColor = { 1.0,1.0,0.0 };
	player.Plocate = All_Box[0][0].Blocate;
	player.Plocate[1] = 0.0f;
	player.Pscale = { 0.2,0.2,0.2 };
	player.lotate = 0;
	player.prev_locate[0] = 0;
	player.prev_locate[1] = 0;
	player.x = 0;
	player.y = 0;
	player.z = 0;

	player.speed = 0.02f;
}

void Box_crash(Player* p) {								// 박스 충돌체크
	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);

	Rotate = glm::rotate(Rotate, glm::radians(p->lotate), glm::vec3(0.0, 1.0, 0.0)); //--- z축에 대하여 회전 행렬
	Scale = glm::scale(Scale, p->Pscale); //		플레이어
	Tx = glm::translate(Tx, p->Plocate + p->Move);
	TR = Tx * Rotate * Scale * TR;

	glm::vec4 left_front = TR * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);					// 플레이어 왼,오른,앞,뒤  원래 좌표를 각각 vec로 저장
	glm::vec4 right_front = TR * glm::vec4(0.5f, 0.0f, -0.5f, 1.0f);					//얘네가 콜라이더 박스
	glm::vec4 left_back = TR * glm::vec4(-0.5f, 0.0f, 0.5f, 1.0f);
	glm::vec4 right_back = TR * glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);


	left_front[0] += 5.0f + 0.01,left_front[2] += 5.0f + 0.01;							// 현재 좌표를 계산 후, 해당 좌표를 배열 인덱스로 변환 
	left_front[0] = left_front[0] / xScale, left_front[2] = left_front[2] / zScale;

	right_front[0] += 5.0f + 0.01, right_front[2] += 5.0f + 0.01;
	right_front[0] = right_front[0] / xScale, right_front[2] = right_front[2] / zScale;

	left_back[0] += 5.0f + 0.01, left_back[2] += 5.0f + 0.01;
	left_back[0] = left_back[0] / xScale, left_back[2] = left_back[2] / zScale;

	right_back[0] += 5.0f + 0.01, right_back[2] += 5.0f + 0.01;
	right_back[0] = right_back[0] / xScale, right_back[2] = right_back[2] / zScale;

	
	if (19 - int(left_front[2]) != p->prev_locate[0] || 19 - int(left_front[0]) != p->prev_locate[1]) {			// 각각의 변환된 인덱스가 이전 인덱스랑 같은지 비교 다르면, 이전 인덱스값 변경 후 해당 인덱스 박스 색 변경
		All_Box[19 - int(left_front[2])][19 - int(left_front[0])].Color = 1;
		p->prev_locate[0] = 19 - int(left_front[2]);
		p->prev_locate[1] = 19 - int(left_front[0]);
	}
	if (19 - int(right_front[2]) != p->prev_locate[0] || 19 - int(right_front[0]) != p->prev_locate[1]) {
		All_Box[19 - int(right_front[2])][19 - int(right_front[0])].Color = 1;
		p->prev_locate[0] = 19 - int(right_front[2]);
		p->prev_locate[1] = 19 - int(right_front[0]);
	}
	if (19 - int(left_back[2]) != p->prev_locate[0] || 19 - int(left_back[0]) != p->prev_locate[1]) {
		All_Box[19 - int(left_back[2])][19 - int(left_back[0])].Color = 1;
		p->prev_locate[0] = 19 - int(left_back[2]);
		p->prev_locate[1] = 19 - int(left_back[0]);
	}
	if (19 - int(right_back[2]) != p->prev_locate[0] || 19 - int(right_back[0]) != p->prev_locate[1]) {
		All_Box[19 - int(right_back[2])][19 - int(right_back[0])].Color = 1;
		p->prev_locate[0] = 19 - int(right_back[2]);
		p->prev_locate[1] = 19 - int(right_back[0]);
	}
}

void player_Camera(Player* p) {
	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);

	Rotate = glm::rotate(Rotate, glm::radians(p->lotate), glm::vec3(0.0, 1.0, 0.0));
	Scale = glm::scale(Scale, p->Pscale); //		플레이어
	Tx = glm::translate(Tx, p->Plocate + p->Move);
	TR = Tx * Rotate * Scale * TR;

	p->cameraPos = TR * glm::vec4(0.0f, Viewy, 0.0f, 1.0f);
	p->cameraDirection = glm::vec3(0.0f, 0.5f, -1.0f);

	p->cameraDirection[0] = sin(glm::radians(p->lotate)) * -1.0;
	p->cameraDirection[2] = cos(glm::radians(p->lotate)) * -1.0;

}

int collide(Player* p, Box* b)
{
	glm::vec3 player_bounding_box[2] = { (p->Plocate + p->Move) - p->Pscale, (p->Plocate - p->Move) + p->Pscale}; // 0이 min, 1이 max
	glm::vec3 box_bounding_box[2] = { b->Blocate - b->Bscale, b->Blocate + b->Bscale };

	//플레이어가 박스보다 좌측 즉 플레이어 max < 박스 min
	if (player_bounding_box[1][0] < box_bounding_box[0][0] && player_bounding_box[1][1] < box_bounding_box[0][1] && player_bounding_box[1][2] < box_bounding_box[0][2]) return 0;
	//플레이어 min > 박스 max
	if (player_bounding_box[0][0] > box_bounding_box[1][0] && player_bounding_box[0][1] > box_bounding_box[1][1] && player_bounding_box[0][2] > box_bounding_box[1][2]) return 0;

	return b->offset;

}

void Crash(Player* p)
{
	int inspection = collide(p, *All_Box);

	if (inspection > 0) {
		p->y += p->gravity;
	}

	for (int i = 0; i < xcount; i++) {
		for (int k = 0; k < Ycount; k++) {
			if (inspection == All_Box[i][k].offset)
				All_Box[i][k].Color = p->PColor;
		}
	}
}


