/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

//--- FAROL TEXTURIZADO ---
Model Farola_M;
// --- PECERA TEXTURIZADA ---
Model Pecera_M;
// -- PEZ ---
Model Pez_M;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	// --- CARGA DE TEXTURAS ---
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	
	// --- CARGA DE MODELOS ---
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Farola_M = Model();
	Farola_M.LoadModel("Models/LAP.obj");
	Pecera_M = Model();
	Pecera_M.LoadModel("Models/acuario.obj");
	Pez_M = Model();
	Pez_M.LoadModel("Models/Pez.obj");

	// --- SKYBOX ---
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// --- LUZ DEL SOL (Direccional) ---
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.4f, 0.4f,
		0.0f, -1.0f, 0.0f);

	// -- LUZ DE LA FAROLA (En el índice 0) --
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,   // Color Blanco
		0.5f, 1.0f,                                 // IA, ID
		0.0f, 1.5f, 5.0f,                           // Posición (X, Y, Z)
		1.0f, 0.5f, 0.0f);                          // Atenuación

	// -- LUZ DEL PEZ (En el índice 1) - Color Azul --
	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,   // Color Azul
		0.5f, 1.0f,                                 // IA, ID
		0.0f, 0.0f, 0.0f,                           // Posición temporal
		0.0f, 0.5f, 0.5f);                          // Atenuación

	unsigned int spotLightCount = 0;

	// --- LUZ DE LA CÁMARA (Índice 0) ---
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,      // Color Blanco
		0.0f, 2.0f,                                  // IA, ID
		0.0f, 0.0f, 0.0f,                            // Posición
		0.0f, -1.0f, 0.0f,                           // Dirección
		1.0f, 0.0f, 0.0f,                            // Atenuación
		5.0f);                                       // Ángulo
	spotLightCount++;

	// --- LUZ HELICÓPTERO ADELANTE (Índice 1) ---
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,      // Color Amarillo
		0.0f, 2.0f,                                  // IA, ID
		0.0f, 0.0f, 0.0f,                            // Posición temporal
		-1.0f, -1.0f, 0.0f,                          // Dirección hacia adelante (-X)
		1.0f, 1.0f, 0.0f,                            // Atenuación
		15.0f);                                      // Ángulo
	spotLightCount++;

	// --- LUZ HELICÓPTERO ATRÁS (Índice 2) ---
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,      // Color Amarillo
		0.0f, 2.0f,                                  // IA, ID
		0.0f, 0.0f, 0.0f,                            // Posición temporal
		1.0f, -1.0f, 0.0f,                           // Dirección hacia atrás (+X)
		1.0f, 1.0f, 0.0f,                            // Atenuación
		15.0f);                                      // Ángulo
	spotLightCount++;


	// --- LUZ AMARILLA DEL PEZ (Índice 3) ---
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,      // Color Amarillo
		0.0f, 2.0f,                                  // IA, ID
		0.0f, 0.0f, 0.0f,                            // Posición temporal
		0.0f, 0.0f, -1.0f,                           // Dirección inicial (Frente)
		0.1f, 0.2f, 0.0f,                            // Atenuación
		25.0f);                                      // Ángulo
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	while (!mainWindow.getShouldClose())
	{
		// Cálculo del deltaTime para mantener velocidades constantes independientemente de los FPS
		GLfloat now = glfwGetTime();
		deltaTime = (now - lastTime) / limitFPS;
		lastTime = now;

		// Procesamiento de eventos de entrada (teclado y mouse)
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpieza de los buffers de color y profundidad para el nuevo frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Dibujado del fondo ambiental (Skybox)
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Activación del shader principal y obtención de las ubicaciones de variables uniform
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		// Carga de matrices de proyección, vista y posición de la cámara al shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Configuración de la luz global del sol
		shaderList[0].SetDirectionalLight(&mainLight);

		// --- CICLO DE LUZ PEZ Y FARO ---
		// Control de encendido/apagado de luces puntuales según el estado capturado en Window
		switch (mainWindow.getLuzPrendida())
		{
		case 0: shaderList[0].SetPointLights(pointLights, 2); break; // Ambas encendidas
		case 1: shaderList[0].SetPointLights(pointLights, 0); break; // Ambas apagadas
		case 2: shaderList[0].SetPointLights(&pointLights[1], 1); break; // Solo pez (Azul)
		case 3: shaderList[0].SetPointLights(&pointLights[0], 1); break; // Solo farola (Blanca)
		}

		// --- LÓGICA DE SPOTLIGHTS ---
		// Actualización de la luz de linterna acoplada a la posición y dirección de la cámara
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f; // Desplazamiento ligero hacia abajo para simular que se sostiene
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Actualización de luces del helicóptero basadas en su posición de movimiento en X
		glm::vec3 posHeli = glm::vec3(mainWindow.getmuevex(), 3.0f, 0.0f);
		spotLights[1].SetFlash(posHeli, glm::vec3(-1.0f, -0.8f, 0.0f));
		spotLights[2].SetFlash(posHeli, glm::vec3(1.0f, -0.8f, 0.0f));

		// --- LÓGICA PEZ Y LUZ AMARILLA ---
		// Obtención de variables de control dinámico (Movimiento y Rotaciones)
		GLfloat movPez = mainWindow.getcofre();
		GLfloat rotX = mainWindow.getrotacion1();
		GLfloat rotY = mainWindow.getrotacion2();
		GLfloat rotZ = mainWindow.getrotacion3();
		glm::vec3 posBulbo = glm::vec3(5.0f + movPez, 3.2f + movPez, -5.0f);

		// Construcción de la matriz de rotación para orientar la luz según el pez
		glm::mat4 rotMatPez = glm::mat4(1.0f);
		rotMatPez = glm::rotate(rotMatPez, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
		rotMatPez = glm::rotate(rotMatPez, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
		rotMatPez = glm::rotate(rotMatPez, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));

		// Transformación del vector frontal (1,0,0) mediante la matriz de rotación
		glm::vec3 dirLuzPez = glm::vec3(rotMatPez * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		spotLights[3].SetFlash(posBulbo, dirLuzPez);

		// Gestión de visibilidad de luces del helicóptero según la dirección de vuelo
		if (mainWindow.getDireccionHeli() == 1) { // Vuelo hacia adelante
			spotLights[2].SetFlash(glm::vec3(0, -100, 0), glm::vec3(0, -1, 0)); // Ocultar luz trasera
			shaderList[0].SetSpotLights(spotLights, 4);
		}
		else if (mainWindow.getDireccionHeli() == 2) { // Vuelo hacia atrás
			SpotLight temp = spotLights[1];
			spotLights[1] = SpotLight(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0); // Desactivar luz delantera
			shaderList[0].SetSpotLights(spotLights, 4);
			spotLights[1] = temp;
		}
		else { // Helicóptero estático (Apagar ambas luces frontales/traseras)
			SpotLight temp1 = spotLights[1];
			SpotLight temp2 = spotLights[2];
			spotLights[1] = SpotLight(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
			spotLights[2] = SpotLight(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
			shaderList[0].SetSpotLights(spotLights, 4);
			spotLights[1] = temp1;
			spotLights[2] = temp2;
		}

		// --- RENDER PISO ---
		glm::mat4 model(1.0);
		glm::vec3 colorPiso = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorPiso));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// --- FAROLA ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 5.0f));
		// Sincronizar posición de la luz puntual con el modelo visual de la farola
		pointLights[0].SetPos(glm::vec3(model[3][0], model[3][1], model[3][2]));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Farola_M.RenderModel();

		// --- HELICÓPTERO ---
		model = glm::mat4(1.0);
		model = glm::translate(model, posHeli);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();

		// --- ACUARIO (Manejo de Transparencias) ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 2.0f, -5.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// Primera pasada: Dibujar caras opacas (traseras)
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pecera_M.RenderModel();

		// Segunda pasada: Dibujar cristal con mezcla de colores (Blending)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE); // Permitir ver a través
		glDepthMask(GL_FALSE);   // No escribir en profundidad para no bloquear objetos internos
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.0f, -1.0f); // Evitar parpadeo (Z-fighting) con el modelo anterior
		Pecera_M.RenderModel();

		// Restaurar estados de renderizado estándar
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		// ---  PEZ Y SUS LUCES ---
		// Actualización de la luz puntual azul que emana del pez
		pointLights[1] = PointLight(0.0f, 0.0f, 1.0f, 1.5f, 1.5f, posBulbo.x, posBulbo.y, posBulbo.z, 0.0f, 0.5f, 0.5f);

		model = glm::mat4(1.0);
		model = glm::translate(model, posBulbo);

		// Aplicación de rotaciones de usuario al modelo
		model = glm::rotate(model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));

		// Ajuste de orientación y escala final del modelo importado
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pez_M.RenderModel();

		// Finalización del frame: liberación del shader e intercambio de buffers
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}