#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	// Inicialización de variables de control
	muevex = 0.0f;
	rotallanta = 0.0f;
	cofre = 0.0f;
	rotacion1 = 0.0f;
	rotacion2 = 0.0f;
	rotacion3 = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	// --- INICIALIZACIÓN DE VARIABLES ESPECÍFICAS (Coche y Octaedro) ---
	muevex = 0.0f;
	rotallanta = 0.0f;
	cofre = 0.0f;
	rotacion1 = 0.0f;
	rotacion2 = 0.0f;
	rotacion3 = 0.0f;

	mouseFirstMoved = true;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica 7: Iluminacion 1", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD

	//Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// Lógica de control para movimiento y piezas (Action Press o Repeat)
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// TRASLACIÓN EN X (Teclas K / L)
		if (key == GLFW_KEY_K) theWindow->muevex += 0.5f;
		if (key == GLFW_KEY_L) theWindow->muevex -= 0.5f;

		// ROTACIÓN LLANTAS EN Z (Teclas H / J)
		if (key == GLFW_KEY_H) theWindow->rotallanta += 5.0f;
		if (key == GLFW_KEY_J) theWindow->rotallanta -= 5.0f;

		// COFRE EN Z (Teclas F / G) - Límite 45°
		if (key == GLFW_KEY_F && theWindow->cofre > -45.0f) theWindow->cofre -= 2.0f;
		if (key == GLFW_KEY_G && theWindow->cofre < 0.0f)   theWindow->cofre += 2.0f;

		// --- ROTACIONES OCTAEDRO (B, N, M)  ---
		if (key == GLFW_KEY_B) theWindow->rotacion1 += 2.0f; // Eje X
		if (key == GLFW_KEY_N) theWindow->rotacion2 += 2.0f; // Eje Y
		if (key == GLFW_KEY_M) theWindow->rotacion3 += 2.0f; // Eje Z
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}