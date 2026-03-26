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

	// Getter de posición global (Punto 4: Traslación solo en X)
	GLfloat getmuevex() { return muevex; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	// --- GETTERS PARA EL COCHE ---
	GLfloat getrotay() { return rotay; }           // Getter para la rotación base del auto
	GLfloat getrotallanta() { return rotallanta; } // Rotación en Z (Teclas H / J)
	GLfloat getcofre() { return cofre; }           // Rotación en Z (Teclas F / G)

	// --- GETTERS PARA EL OCTAEDRO ---
	GLfloat getrotacion1() { return rotacion1; } // Rotación en X (Tecla B)
	GLfloat getrotacion2() { return rotacion2; } // Rotación en Y (Tecla N)
	GLfloat getrotacion3() { return rotacion3; } // Rotación en Z (Tecla M)

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

	// Variable de traslación única (Punto 4: Teclas K / L)
	GLfloat muevex;

	bool mouseFirstMoved;

	// --- VARIABLES DE CONTROL (Solo rotaciones en Z) ---
	GLfloat rotay;      
	GLfloat rotallanta;
	GLfloat cofre;

	// --- VARIABLES DE CONTROL OCTAEDRO ---
	GLfloat rotacion1; // Eje X
	GLfloat rotacion2; // Eje Y
	GLfloat rotacion3; // Eje Z

	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};