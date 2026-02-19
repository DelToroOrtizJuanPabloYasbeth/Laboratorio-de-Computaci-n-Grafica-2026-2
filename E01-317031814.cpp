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
const double INTERVALO_COLOR = 1.0;

// Vertex Shader: Recibe posición (0) y color (1) por atributo
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
layout (location = 1) in vec3 inColor;				\n\
out vec3 outColor;									\n\
void main()											\n\
{													\n\
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f); 	\n\
    outColor = inColor;								\n\
}";

// Fragment Shader: Recibe el color "outColor" del Vertex Shader
static const char* fShader = "						\n\
#version 330										\n\
in vec3 outColor;									\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(outColor, 1.0f);	 				\n\
}";

void CrearFiguras()
{
	// Estructura: X, Y, Z, R, G, B
	GLfloat vertices[] = {
		// ROMBO (Triángulo 1) - Color Negro (0,0,0)
		-0.3f,  0.3f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.0f,  0.6f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.3f,  0.3f, 0.0f,   0.0f, 0.0f, 0.0f,
		 // ROMBO (Triángulo 2) - Color Negro (0,0,0)
		 -0.3f,  0.3f, 0.0f,   0.0f, 0.0f, 0.0f,
		  0.3f,  0.3f, 0.0f,   0.0f, 0.0f, 0.0f,
		  0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

		  // TRAPECIO (Triángulo 1) - Color Blanco (1,1,1)
		  -0.5f, -0.2f, 0.0f,   1.0f, 1.0f, 1.0f,
		   0.5f, -0.2f, 0.0f,   1.0f, 1.0f, 1.0f,
		  -0.3f, -0.6f, 0.0f,   1.0f, 1.0f, 1.0f,
		  // TRAPECIO (Triángulo 2) - Color Blanco (1,1,1)
		  -0.3f, -0.6f, 0.0f,   1.0f, 1.0f, 1.0f,
		   0.5f, -0.2f, 0.0f,   1.0f, 1.0f, 1.0f,
		   0.3f, -0.6f, 0.0f,   1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Atributo 0: Posición (X,Y,Z). Salto (stride) de 6 floats.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo 1: Color (R,G,B). Empieza en el float número 3.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1] = { shaderCode };
	GLint codeLength[1] = { (GLint)strlen(shaderCode) };
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);
	glLinkProgram(shader);
}

void ActualizarColorFondo()
{
	double currentTime = glfwGetTime();
	if (currentTime - lastTime >= INTERVALO_COLOR) {
		lastTime = currentTime;
		colorState = (colorState + 1) % 3;
		if (colorState == 0) { redColor = 1.0f; greenColor = 0.0f; blueColor = 0.0f; }
		else if (colorState == 1) { redColor = 0.0f; greenColor = 1.0f; blueColor = 0.0f; }
		else { redColor = 0.0f; greenColor = 0.0f; blueColor = 1.0f; }
	}
}

int main()
{
	if (!glfwInit()) return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "E01-317031814", NULL, NULL);
	if (!mainWindow) { glfwTerminate(); return 1; }

	glfwMakeContextCurrent(mainWindow);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) return 1;

	glViewport(0, 0, WIDTH, HEIGHT);
	CrearFiguras();
	CompileShaders();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();
		ActualizarColorFondo();
		glClearColor(redColor, greenColor, blueColor, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);

		// Dibujamos todo (12 vértices). OpenGL ya sabe qué color ponerle a cada uno 
		// gracias a los atributos del VBO.
		glDrawArrays(GL_TRIANGLES, 0, 12);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}