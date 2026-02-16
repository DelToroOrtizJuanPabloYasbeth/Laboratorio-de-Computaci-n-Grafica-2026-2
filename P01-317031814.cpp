#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glew.h>
#include <glfw3.h>

// Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// Variables para el color de fondo aleatorio
float redColor = 1.0f, greenColor = 1.0f, blueColor = 1.0f;
double lastTime = 0;
const double INTERVALO_CAMBIO = 2.0; // Cambiar cada 2 segundos

// Color fijo para las letras (Negro)
const float letraColorR = 0.0f;
const float letraColorG = 0.0f;
const float letraColorB = 0.0f;

// LENGUAJE DE SHADER (GLSL)
// Vertex Shader: Recibe la posición y la pasa a la tarjeta gráfica
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
void main()											\n\
{													\n\
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0); 	\n\
}";

// Fragment Shader: Recibe el color desde una variable Uniform
static const char* fShader = "						\n\
#version 330										\n\
uniform vec3 shapeColor;							\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
    color = vec4(shapeColor, 1.0);	 				\n\
}";

void CrearLetrasDTJ()
{
    GLfloat vertices[] = {
        // === LETRA D  ===
        // Barra vertical izquierda
        -0.85f, -0.8f, 0.0f,  -0.75f, -0.8f, 0.0f,  -0.85f, -0.4f, 0.0f,
        -0.85f, -0.4f, 0.0f,  -0.75f, -0.8f, 0.0f,  -0.75f, -0.4f, 0.0f,
        // Barra superior
        -0.75f, -0.5f, 0.0f,  -0.60f, -0.5f, 0.0f,  -0.75f, -0.4f, 0.0f,
        -0.75f, -0.4f, 0.0f,  -0.60f, -0.5f, 0.0f,  -0.60f, -0.4f, 0.0f,
        // Barra inferior
        -0.75f, -0.8f, 0.0f,  -0.60f, -0.8f, 0.0f,  -0.75f, -0.7f, 0.0f,
        -0.75f, -0.7f, 0.0f,  -0.60f, -0.8f, 0.0f,  -0.60f, -0.7f, 0.0f,
        // Barra vertical derecha
        -0.50f, -0.7f, 0.0f,  -0.40f, -0.7f, 0.0f,  -0.50f, -0.5f, 0.0f,
        -0.50f, -0.5f, 0.0f,  -0.40f, -0.7f, 0.0f,  -0.40f, -0.5f, 0.0f,
        // Chaflán Superior Derecho (\)
        -0.60f, -0.4f, 0.0f,  -0.50f, -0.5f, 0.0f,  -0.60f, -0.5f, 0.0f,
        -0.60f, -0.4f, 0.0f,  -0.40f, -0.5f, 0.0f,  -0.50f, -0.5f, 0.0f,
        // Chaflán Inferior Derecho (/)
        -0.60f, -0.8f, 0.0f,  -0.50f, -0.7f, 0.0f,  -0.60f, -0.7f, 0.0f,
        -0.60f, -0.8f, 0.0f,  -0.40f, -0.7f, 0.0f,  -0.50f, -0.7f, 0.0f,

        // === LETRA T (Centro) ===
        -0.05f, -0.2f, 0.0f,   0.05f, -0.2f, 0.0f,  -0.05f,  0.2f, 0.0f,
        -0.05f,  0.2f, 0.0f,   0.05f, -0.2f, 0.0f,   0.05f,  0.2f, 0.0f,
        -0.25f,  0.2f, 0.0f,   0.25f,  0.2f, 0.0f,  -0.25f,  0.3f, 0.0f,
        -0.25f,  0.3f, 0.0f,   0.25f,  0.2f, 0.0f,   0.25f,  0.3f, 0.0f,

        // === LETRA J (Arriba-Derecha) ===
        0.75f,  0.3f, 0.0f,   0.85f,  0.3f, 0.0f,   0.75f,  0.8f, 0.0f,
        0.75f,  0.8f, 0.0f,   0.85f,  0.3f, 0.0f,   0.85f,  0.8f, 0.0f,
        0.55f,  0.3f, 0.0f,   0.75f,  0.3f, 0.0f,   0.55f,  0.4f, 0.0f,
        0.55f,  0.4f, 0.0f,   0.75f,  0.3f, 0.0f,   0.75f,  0.4f, 0.0f,
        0.52f,  0.4f, 0.0f,   0.62f,  0.4f, 0.0f,   0.52f,  0.55f, 0.0f,
        0.52f,  0.55f, 0.0f,   0.62f,  0.4f, 0.0f,   0.62f,  0.55f, 0.0f
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
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error al compilar el shader %d: %s \n", shaderType, eLog);
        return;
    }
    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();
    if (!shader) {
        printf("Error creando el shader");
        return;
    }
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error al linkear: %s \n", eLog);
        return;
    }
    glValidateProgram(shader);
}

void GenerarColorAleatorio() {
    redColor = (float)rand() / (float)RAND_MAX;
    greenColor = (float)rand() / (float)RAND_MAX;
    blueColor = (float)rand() / (float)RAND_MAX;
}

int main()
{
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        printf("Falló inicializar GLFW");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "P01 - DTJ Final Unificado", NULL, NULL);
    if (!mainWindow) {
        printf("Fallo en crearse la ventana");
        glfwTerminate();
        return 1;
    }

    int BufferWidth, BufferHeight;
    glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Falló inicialización de GLEW");
        return 1;
    }

    glViewport(0, 0, BufferWidth, BufferHeight);
    CrearLetrasDTJ();
    CompileShaders();

    GenerarColorAleatorio();
    lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        // Cambio de color de fondo por tiempo
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= INTERVALO_CAMBIO) {
            GenerarColorAleatorio();
            lastTime = currentTime;
        }

        glClearColor(redColor, greenColor, blueColor, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        // Asignar el color de las letras mediante el Uniform
        GLint shapeColorLoc = glGetUniformLocation(shader, "shapeColor");
        glUniform3f(shapeColorLoc, letraColorR, letraColorG, letraColorB);

        glBindVertexArray(VAO);
        // Dibujamos los 72 vértices (24 triángulos) que componen la D, T y J
        glDrawArrays(GL_TRIANGLES, 0, 72);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwTerminate();
    return 0;
}