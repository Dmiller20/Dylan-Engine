///////////////////////////////////////////////////////////////////
//																 //
//						Dylan's OpenGL Game Engine				 //
//																 //		
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string>

#include "math_3d.h"
#include "ogldev_util.h"

GLuint VBO;
GLint gScaleLocation;

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	static float Delta = 0.005f;

	Scale += Delta;
	if ((Scale >= 1.0f) || (Scale <= -1.0f)) {
		Delta *= -1.0f;
	}

	glUniform1f(gScaleLocation, Scale);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	glutPostRedisplay();

	glutSwapBuffers();
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

		gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
		if (gScaleLocation == -1) {
			printf("Error getting uniform location of 'gScale' \n");
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

	//creates VB
	static void CreateVertexBuffer()
	{
	
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_FRONT);
		Vector3f Vertices[3];
		Vertices[0] = Vector3f( -0.5f, 0.5f, 0.0f);  // bottom left
		Vertices[1] = Vector3f(0.5f, -0.5f, 0.0f);	//bottom right
		Vertices[2] = Vector3f(0.0f, 0.5f, 0.0f);	 //top
		

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}
	

	// main
	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

		
		glutInitWindowSize(1280, 720);

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

		CreateVertexBuffer();

		CompileShaders();

		GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
		glClearColor(Red, Green, Blue, Alpha);

		glutDisplayFunc(RenderSceneCB);

		glutMainLoop();
	
		return 0;
	}