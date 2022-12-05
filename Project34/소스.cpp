#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <random>

void make_vertexShader();
void make_fragmentShader();
void InitBuffer();
void InitShader();
void TimerFunction(int value);
//void SKeyboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid drawScene();
GLvoid Keyboard(unsigned char key, int x, int y);
char* filetobuf(const char* file);
void Boxinit(int x, int y);
void Mazeinit();
void Draw_filed();

typedef struct Box {
	glm::vec3 Bscale;
	glm::vec3 Blocate;
	glm::vec3 BColor;

	float Max_height;
	float Speed;

	int Yplus;
}Box;


GLchar* vertexsource, * fragmentsource; //--- �ҽ��ڵ� ���� ����
GLuint vertexshader, fragmentshader; //--- ���̴� ��ü
GLuint s_program;
GLint result;
GLuint vao, vbo[2], EBO[2], linevbo[2], linevao;

float Cameraz = 15.0f;
float Camerax = 0.0f;
float ySelfRot = 0.0f;
float yZeroRot = 0.0f;

bool ra, Drop = true, What_view, Mani,Vani,rani,rminus;
int xcount = 0, Ycount = 0;
float yz = 0.0f;
glm::vec3 Bscale[25][25];
glm::vec3 Blocate[25][25];
glm::vec3 BColor[25][25];
bool* maze;
Box* All_Box;
float dropboard = 3.0, drop = 3.0;

GLUquadricObj* qobj = gluNewQuadric();

glm::vec3 cameraPos;
glm::vec3 cameraPos1;
glm::vec3 cameraDirection; //--- ī�޶� �ٶ󺸴� ����
glm::vec3 cameraUp; //--- ī�޶� ���� ����

float vPositionList[] = {
0.5f, 1.0f, 0.5f, // �� ���� ���
0.5f, 1.0f, -0.5f, // �� ���� ���
-0.5f, 1.0f, -0.5f, // �� ���� ���
-0.5f, 1.0f, 0.5f, // �� ���� ���
-0.5f,0.0f,0.5f, // �� ���� �ϴ�
-0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.5f,0.0f,0.5f,		// �� ���� �ϴ�

0.5f,0.0f,0.5f,
0.5f,0.0f,-0.5f,
-0.5f,0.0f,0.5f, // �� ���� �ϴ�
-0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.0f,0.5f,0.0f
};

unsigned int index[] = {
1, 3, 0, // ù ��° �ﰢ��			// �� �簢��
1, 2, 3, // �� ��° �ﰢ��
5, 4, 2,									// ���� �簢��
3, 2, 4,
0, 7, 6,							// ������ �簢��
6, 1, 0,
3, 4, 7,							// �� �簢��
7, 0, 3,
6, 5, 2,								// �� �簢��
6, 2, 1,
6, 7, 4,	// �� ��° �ﰢ��			// �Ʒ� �簢��
6, 4, 5,	// �� ��° �ﰢ��

8,9,10,
10,9,11,
10,8,12,		// �� �ﰢ��
8,9,12,			// ������
9,11,12,		// ����
11,10,12		// ����
};

float line[]{
5.0f,0.0f,0.0f,
-5.0f,0.0f,0.0f,
0.0f,5.0f,0.0f,
0.0f,-5.0f,0.0f,
0.0f,0.0f,5.0f,
0.0f,0.0f,-5.0f
};

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;

	printf("x X y");
	scanf("%d %d", &xcount,&Ycount);
	Boxinit(xcount,Ycount);

	glewInit();
	InitShader();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է� �ݹ��Լ� ����
	//glutSpecialFunc(SKeyboard);
	glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();
}

void make_vertexShader() {
	vertexsource = filetobuf("vertex.glsl");//--- ���ؽ� ���̴� ��ü �����
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexshader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLchar errorLog[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexshader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShader() {
	fragmentsource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentshader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLchar errorLog[512];
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void InitShader()
{
	make_vertexShader(); //--- ���ؽ� ���̴� �����
	make_fragmentShader(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);

	GLchar errorLog[512];
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
	//--- Shader Program ����ϱ�
	glUseProgram(s_program);
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]); //--- GL_ELEMENT_ARRAY_BUFFER ���� �������� ���ε�
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &linevao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(linevao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, linevbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
	cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
	cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� �ٶ󺸴� ����
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

}

void drawScene() {
	glUseProgram(s_program);
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	glm::mat4 Prev_rotation = glm::mat4(1.0f);



	if (!What_view) {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
		projection = glm::rotate(projection, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
		//projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- ������ �ణ �ڷ� �̷���
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	else{
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f); //--- ���� ������ [-100.0, 100.0] �������� ����
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}


	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	Draw_filed();
	glViewport(600, 400, 200, 200);
	

	view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
	projection = glm::translate(projection, glm::vec3(0.0, -3.0, 15.0)); //--- ������ �ణ �ڷ� �̷���
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	Draw_filed();
	glViewport(0, 0, 800, 600);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
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
	if (Drop) {
		if (drop >= -0.2)
			drop -= 0.2;
		if (dropboard >= -10.0)
			dropboard -= 0.2;
		else
			Drop = false;
	}
	else{
		if (Mani) {
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {

						if (!rani || maze[i + k]) {
							if (All_Box[i + k].Yplus == 1) {
								All_Box[i + k].Bscale[1] += All_Box[i + k].Speed;
								if (All_Box[i + k].Bscale[1] >= All_Box[i + k].Max_height)
									All_Box[i + k].Yplus = 2;
							}
							else if (All_Box[i + k].Yplus == 2) {
								All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
								if (All_Box[i + k].Bscale[1] <= 1)
									All_Box[i + k].Yplus = 1;
							}
						}
					}
				
			}
		}
		else if (Vani) {
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {
					if (maze[i + k]) {
						if (All_Box[i + k].Bscale[1] > 1)
							All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
						else
							All_Box[i + k].Bscale[1] = 1;
					}
				}
			}
		}
		
		if (rminus) {
			rminus = false;
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {
					if (!maze[i + k]) {
						if (All_Box[i + k].Bscale[1] > -2.0) {
							All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
							rminus = true;
						}
					}
				}
			}
		}
	}
	glutTimerFunc(50, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'o') {
		What_view = true;					 // ���� ����
		cameraPos = glm::vec3(Camerax, 0.0f, 10);
	}
	else if (key == 'r') {
		rani = rani ? false : true;
		rminus = true;
	}
	else if (key == 'p') {
		What_view = false;					// ���� ����
		cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
	}
	else if (key == 'z') {
		if (What_view == false) {
			Cameraz -= 0.1;					// ���� �����Ͻ� Camerz ���̱�
			cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
			cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
		}
	}
	else if (key == 'Z') {
		if (What_view == false) {
			Cameraz += 0.1;					// ���� �����Ͻ� Camerz �ø���
			cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
			cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
		}
	}
	else if (key == 'm') {
		Mani = Mani ? false : true;			// �簢�� ������������ �ִϸ��̼� ���
		Vani = false;
	}
	else if (key == 'v') {
		if (Vani == false) {
			Vani = true;
			Mani = false;
		}
		else if (Vani == true) {
			Vani = false;
			Mani = true;
		}
	}
	else if (key == 'y') {
		yZeroRot += 1.0f;					// �ٴ� ���� Y ������ �ð�ݴ����
		glm::vec3 front;
		front.x = 0 * cos(glm::radians(yZeroRot)) + Cameraz * sin(glm::radians(yZeroRot));
		front.y = 0.0f;
		front.z = 0 * (-sin(glm::radians(yZeroRot))) + Cameraz * cos(glm::radians(yZeroRot));

		cameraPos = front;

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 direction = glm::normalize(cameraPos - target);
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), direction));
		
		if (What_view)
			cameraPos.y = 0;
		else
			cameraPos.y = 15.0f;

		cameraDirection = -direction;
	}
	else if (key == 'Y') {
		yZeroRot -= 1.0f;							// �ٴ� ���� Y ������ �ð����
		glm::vec3 front;
		front.x = 0 * cos(glm::radians(yZeroRot)) + Cameraz * sin(glm::radians(yZeroRot));
		front.y = 0.0f;
		front.z = 0 * (-sin(glm::radians(yZeroRot))) + Cameraz * cos(glm::radians(yZeroRot));

		cameraPos = front;

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 direction = glm::normalize(cameraPos - target);
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), direction));

		if (What_view)
			cameraPos.y = 0;
		else
			cameraPos.y = 15.0f;
		cameraDirection = -direction;
	}
	else if (key == 'c') {
		cameraPos.y = 15.0f;
		Camerax = 0;
		Cameraz = 15.0f;
		Vani = false;
		Mani = false;
		What_view == false;
		yZeroRot = 0;

		for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
			for (int k = 0; k < xcount; k++) {
				All_Box[i + k].Bscale[1] = All_Box[i + k].Max_height;
			}
		}
	}
	else if (key == 'q')
		exit(0);
	glutPostRedisplay();
}

void Boxinit(int x, int y) {

	All_Box = (Box*)malloc(sizeof(Box)* x * y);		// ���ϴ� xz������ŭ �迭 ����
	maze = (bool*)malloc(sizeof(bool) * x * y);		// ���ϴ� xz������ŭ �̷ο� ����� �迭 ����.

	srand(time(NULL));
	Mazeinit();
	

	float xScale = (float)10 / x;
	float yScale = (float)10 / y;
	float xlocate = -5 + (xScale / 2);
	float ylocate = -5 + (yScale / 2);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(4, 8);
	std::uniform_real_distribution<> Speed(0.05, 0.3);

	int a = 0;
	for (int i = 0; i < Ycount * Ycount; i = i + Ycount,a++) {
		for (int k = 0; k < x; k++) {
			All_Box[i  + k].Bscale[0] = xScale;
			All_Box[i + k].Bscale[1] = dist(gen);
			All_Box[i + k].Bscale[2] = yScale;
			All_Box[i + k].Blocate[0] = xlocate + xScale * k;
			All_Box[i + k].Blocate[1] = 0.0f;
			All_Box[i + k].Blocate[2] = ylocate + yScale * a;
			All_Box[i + k].BColor[0] = rand() % 10;
			All_Box[i + k].BColor[1] = rand() % 10;
			All_Box[i + k].BColor[2] = rand() % 10;
			All_Box[i + k].Max_height = All_Box[i + k].Bscale[1];
			All_Box[i + k].Yplus = rand() %2 + 1;
			All_Box[i + k].Speed = Speed(gen);
		}
	}

}

void Mazeinit() {

	for (int i = 0; i < Ycount; i++) {
		for (int j = 0; j < xcount; j++) {
			maze[i * Ycount + j] = 1;
		}
	}

	for (int i = 1; i < Ycount - 1; i++)
	{
		for (int j = 1; j < xcount - 1; j += 2)
		{
			if (i % 2 == 0)
				maze[i *Ycount + xcount - 2] = 0;
			else {
				//���� ���� ���� ���ϴ� �̶��(�� ������ ��) ��ŵ
				if (i == Ycount - 2 && j == xcount - 2) {
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
					continue;
				}
				//���� ���� �� �Ʒ� �����̶�� ���������� �� �ձ�
				if (i == Ycount - 2) {
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
					continue;
				}
				//���� ���� �� ������ �����̶�� �Ʒ������� �� �ձ�
				if (j == xcount - 2) {
					maze[i * Ycount + j] = 0;
					continue;
				}

				//���ٸ� Ư�̻����� ���� �ƴϸ� ������ �Ǵ� �Ʒ� �� ���� �ձ�
				if (rand() % 2 == 0)
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
				else
					for (int k = 0; k < 2; k++)
						maze[(i + k) * Ycount + j] = 0;
			}
		}
	}

	//if (xcount % 2 == 0) {
	//	for (int i = 1; i < Ycount - 1; i++) {
	//		maze[i * Ycount + xcount - 2] = 0;
	//	}
	//}

	for (int i = 0; i < Ycount; i++) {
		for (int j = 0; j < xcount; j++)
			printf("%d ", maze[i * Ycount + j]);
		}
		printf("\n");
	}




void Draw_filed() {
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	unsigned int modelLocation1 = glGetUniformLocation(s_program, "in_Color"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������

	glBindVertexArray(vao);

	for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
		for (int j = 0; j < xcount; j++) {
			glm::mat4 TR = glm::mat4(1.0f);
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);

			glUniform3f(modelLocation1, 0.1 * All_Box[i + j].BColor[0], 0.1 * All_Box[i + j].BColor[1], 0.1 * All_Box[i + j].BColor[2]);
			Scale = glm::scale(Scale, glm::vec3(All_Box[i + j].Bscale)); //		�� �簢�� ũ�� 
			Tx = glm::translate(Tx, All_Box[i + j].Blocate);
			Tx = glm::translate(Tx, glm::vec3(0.0f, drop, 0.0f));

			TR = Tx * Scale * TR;

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform ������ ��ȯ �� �����ϱ�
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
		}
	}

	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	Tx = glm::translate(Tx, glm::vec3(0.0f, dropboard, 0.0f));
	Scale = glm::scale(Scale, glm::vec3(11.0f, 10.0f, 11.0f));
	TR = Tx * Scale * TR;
	glUniform3f(modelLocation1, 0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform ������ ��ȯ �� �����ϱ�
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));

}