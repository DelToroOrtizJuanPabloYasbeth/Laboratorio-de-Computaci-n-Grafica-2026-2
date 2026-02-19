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
float redColor, greenColor, blueColor; // No las inicializamos en 1.0 para evitar el blanco
double lastTime = 0;
const double INTERVALO_CAMBIO = 2.0;

// Vertex Shader: Recibe posición (0) y color (1)
static const char* vShader = "						\n\
#version 330										\n\
layout (location = 0) in vec3 pos;					\n\
layout (location = 1) in vec3 inColor;				\n\
out vec3 outColor;									\n\
void main()											\n\
{													\n\
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0); 	\n\
    outColor = inColor;								\n\
}";

// Fragment Shader
static const char* fShader = "						\n\
#version 330										\n\
in vec3 outColor;									\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
    color = vec4(outColor, 1.0);	 				\n\
}";

void GenerarColorAleatorio() {
    redColor = (float)rand() / (float)RAND_MAX;
    greenColor = (float)rand() / (float)RAND_MAX;
    blueColor = (float)rand() / (float)RAND_MAX;
}

void CrearLetrasDTJ()
{
    // Datos de las letras con color negro intercalado (Stride de 6 floats)
    GLfloat vertices[] = {
        // === LETRA D  ===
        -0.85f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.75f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.85f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.85f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.75f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.75f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.75f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.75f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.75f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.75f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.75f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.75f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.50f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.40f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.50f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.50f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.40f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.40f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.60f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.50f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.60f, -0.4f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.40f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.50f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.60f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.50f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.60f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.60f, -0.8f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.40f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.50f, -0.7f, 0.0f,  0.0f, 0.0f, 0.0f,

        // === LETRA T ===
        -0.05f, -0.2f, 0.0f,  0.0f, 0.0f, 0.0f,    0.05f, -0.2f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.05f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.05f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,    0.05f, -0.2f, 0.0f,  0.0f, 0.0f, 0.0f,    0.05f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.25f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,    0.25f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,   -0.25f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,
        -0.25f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.25f,  0.2f, 0.0f,  0.0f, 0.0f, 0.0f,    0.25f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,

        // === LETRA J ===
         0.75f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.85f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.75f,  0.8f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.75f,  0.8f, 0.0f,  0.0f, 0.0f, 0.0f,    0.85f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.85f,  0.8f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.55f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.75f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.55f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.55f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,    0.75f,  0.3f, 0.0f,  0.0f, 0.0f, 0.0f,    0.75f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.52f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,    0.62f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,    0.52f,  0.55f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.52f,  0.55f, 0.0f, 0.0f, 0.0f, 0.0f,    0.62f,  0.4f, 0.0f,  0.0f, 0.0f, 0.0f,    0.62f,  0.55f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);
    const GLchar* theCode[1] = { shaderCode };
    GLint codeLength[1] = { (GLint)strlen(shaderCode) };
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);
    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);
    glLinkProgram(shader);
}

int main() {
    srand((unsigned int)time(NULL));
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "P01-317031814", NULL, NULL);
    if (!mainWindow) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(mainWindow);
    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, WIDTH, HEIGHT);

    // --- IMPORTANTE: Generar el primer color antes de que inicie el loop ---
    GenerarColorAleatorio();

    CrearLetrasDTJ();
    CompileShaders();

    lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();

        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= INTERVALO_CAMBIO) {
            GenerarColorAleatorio();
            lastTime = currentTime;
        }

        // Aquí usamos los colores generados (ya no inicia en 1,1,1 blanco)
        glClearColor(redColor, greenColor, blueColor, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);

        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(mainWindow);
    }
    glfwTerminate();
    return 0;
}