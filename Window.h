#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return (GLfloat)bufferWidth; }
	GLfloat getBufferHeight() { return (GLfloat)bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }

	// --- GETTERS PARA EL COCHE  ---
	GLfloat getrotallanta() { return rotallanta; }
	GLfloat getcofre() { return cofre; }
	GLfloat getrotacion1() { return rotacion1; }
	GLfloat getrotacion2() { return rotacion2; }
	GLfloat getrotacion3() { return rotacion3; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;

	// Variables de control
	GLfloat muevex;
	GLfloat rotallanta;
	GLfloat cofre;
	GLfloat rotacion1;
	GLfloat rotacion2;
	GLfloat rotacion3;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};