#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>

// Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// Variables para el control de color de fondo
int colorState = 0;
float redColor = 1.0f, greenColor = 0.0f, blueColor = 0.0f;
double lastTime = 0.0;
const double INTERVALO_COLOR = 1.0; // Cambio cada 1.0 segundo

// Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
void main()											\n\
{													\n\
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f); 	\n\
}";

// Fragment Shader con variable Uniform para cambiar el color de las figuras
static const char* fShader = "						\n\
#version 330										\n\
uniform vec3 shapeColor;							\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(shapeColor, 1.0f);	 				\n\
}";

void CrearFiguras()
{
	GLfloat vertices[] = {
		// ROMBO (dividido en dos triángulos)
		-0.3f, 0.3f, 0.0f,   0.0f, 0.6f, 0.0f,   0.3f, 0.3f, 0.0f,
		-0.3f, 0.3f, 0.0f,   0.3f, 0.3f, 0.0f,   0.0f, 0.0f, 0.0f,

		// TRAPECIO ISÓSCELES (dividido en dos triángulos)
		-0.5f, -0.2f, 0.0f,  0.5f, -0.2f, 0.0f,  -0.3f, -0.6f, 0.0f,
		-0.3f, -0.6f, 0.0f,  0.5f, -0.2f, 0.0f,   0.3f, -0.6f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error al compilar el shader %d: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error al linkear: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error al validar: %s \n", eLog);
		return;
	}
}

void ActualizarColorFondo() {
	double currentTime = glfwGetTime();

	if (currentTime - lastTime >= INTERVALO_COLOR) {
		lastTime = currentTime;
		colorState = (colorState + 1) % 3;

		switch (colorState) {
		case 0: // Rojo
			redColor = 1.0f; greenColor = 0.0f; blueColor = 0.0f;
			break;
		case 1: // Verde
			redColor = 0.0f; greenColor = 1.0f; blueColor = 0.0f;
			break;
		case 2: // Azul
			redColor = 0.0f; greenColor = 0.0f; blueColor = 1.0f;
			break;
		}
	}
}

int main()
{
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Nombre de la ventana siguiendo el formato de la práctica
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "P01-317031814", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}

	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
	glfwMakeContextCurrent(mainWindow);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, BufferWidth, BufferHeight);

	CrearFiguras();
	CompileShaders();

	lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		ActualizarColorFondo();
		glClearColor(redColor, greenColor, blueColor, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		GLint shapeColorLoc = glGetUniformLocation(shader, "shapeColor");

		glBindVertexArray(VAO);

		// Dibujar ROMBO (Color Negro)
		glUniform3f(shapeColorLoc, 0.0f, 0.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Dibujar TRAPECIO (Color Blanco)
		glUniform3f(shapeColorLoc, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}