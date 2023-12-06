///////////////////////////////////////////////////////////////////
//																 //
//						Dylan's OpenGL Game Engine				 //
//																 //		
///////////////////////////////////////////////////////////////////


#define _USE_MATH_DEFINES
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>
#include "World_Transform.h"
#include "math_3d.h"
#include "ogldev_util.h"
#include "Camera.h"

GLuint VBO;
GLuint IBO;
GLint gWorldLocation;
int WindowHeight = 720;
int WindowWidth = 1280;

WorldTrans CubeWorldTransform;

float FOV = 120.0f;
float zNear = 1.0f;
float zFar = 10.0f;

Camera GameCamera;

static void KeyboardCB(unsigned char key, int mouse_x, int mouse_y)
{
	GameCamera.OnKeyboard(key);

}

static void SpecialKeyboardCB(int key, int mouse_x, int mouse_y)
{
	GameCamera.OnKeyboard(key);

}

PersProjInfo PPInfo = { FOV, WindowWidth, WindowHeight, zNear, zFar };
static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float YRotationAngle = 1.0f;

	CubeWorldTransform.SetPosition(0.0f, 0.0f, 2.0f);
	CubeWorldTransform.Rotate(0.0f, YRotationAngle, 0.0f);
	Matrix4f World = CubeWorldTransform.GetMatrix();

	Matrix4f View = GameCamera.GetMatrix();

	Matrix4f Projection;
	Projection.InitPersProjTransform(PPInfo);

	Matrix4f WVP = Projection * View * World;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &WVP.m[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutPostRedisplay();

	glutSwapBuffers();
	}

struct Vertex {
	Vector3f pos;
	Vector3f color;

	Vertex() {}

	Vertex(float x, float y, float z) {
		pos = Vector3f(x, y, z);

		float red = (float)rand() / (float)RAND_MAX;
		float green = (float)rand() / (float)RAND_MAX;
		float blue = (float)rand() / (float)RAND_MAX;

		color = Vector3f(red, green, blue);
	}
};

static void CreateVertexBuffer()
{
	Vertex Vertices[8];
						//x    y      z
	Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);
	Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f);
	Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f);
	Vertices[3] = Vertex(0.5f, -0.5f, -0.5f);
	Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
	Vertices[5] = Vertex(0.5f, 0.5f, -0.5f);
	Vertices[6] = Vertex(0.5f, -0.5f, 0.5f);
	Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

}

static void CreateIndexBuffer()
{
	unsigned int Indices[] = {	
								0, 1, 2,
								1, 3, 4,
								5, 6, 3,
								7, 3, 6,
								2, 4, 7,
								0, 7, 6,
								0, 5, 1,
								1, 5, 3,
								5, 0, 6,
								7, 4, 3,
								2, 1, 4,
								0, 2, 7
						 };
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

}


	
// Add Shaders
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;

	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s' \n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}


const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// Compiles Shaders
	static void CompileShaders()
	{
		GLuint ShaderProgram = glCreateProgram(); // allocated a handle for our program

		if (ShaderProgram == 0) {
			fprintf(stderr, "Error creating shader program\n");
			exit(1);
		}

		std::string vs, fs;
		
		if (!LookFile(pVSFileName, vs)) {
			exit(1);
		};
		
		AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

		if (!LookFile(pFSFileName, fs)) {
			exit(1);
		};

		AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

		GLint Success = 0;
		GLchar ErrorLog[1024] = { 0 };

		glLinkProgram(ShaderProgram);

		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
		if (Success == 0)
		{
			glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s' \n", ErrorLog);
			exit(1);
		}

		gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
		if (gWorldLocation == -1) {
			printf("Error getting uniform location of 'gWorld' \n");
			exit(1);
		}


		glValidateProgram(ShaderProgram);
		glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s' \n", ErrorLog);
			exit(1);
		}
		glUseProgram(ShaderProgram);
	}

	// main
	int main(int argc, char** argv)
	{
	
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

		
		glutInitWindowSize(WindowWidth, WindowHeight);

		glutInitWindowPosition(200, 100);
		int win = glutCreateWindow("Dylan's OpenGL Game Engine");
		printf("window id: %d\n", win);

		// ** MUST BE DONE AFTER GLUT IS INITALIZED ** 
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			fprintf(stderr, "Error : '%s' \n", glewGetErrorString(res));
			return 1;
		}


		GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
		glClearColor(Red, Green, Blue, Alpha);

		glEnable(GL_CULL_FACE);	//culling object so both sides dont render
		glFrontFace(GL_CW);		// declaring the front face of the object
		glCullFace(GL_BACK);	// declaring which side to cull

		CreateVertexBuffer();
		CreateIndexBuffer();

		CompileShaders();

		glutDisplayFunc(RenderSceneCB);
		glutKeyboardFunc(KeyboardCB);
		glutSpecialFunc(SpecialKeyboardCB);

		glutMainLoop();
	
		return 0;
	}