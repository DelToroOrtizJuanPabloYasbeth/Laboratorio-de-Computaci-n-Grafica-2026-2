/*
Prática 9 sesión 2
 Animación compleja: Por medio de funciones, algoritmos y textura Animada
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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

// Variables para el fuego
float toffsetfuegou = 0.0f;
float velocidadFuego = 0.00000025f;
bool derechaFuego = true;
bool mostrarFuego = false;
float TIEMPO_FUEGO = 180.0f;

//Variables  Para el humo
float toffsetHumoV = 0.0f;
float velocidadHumo = 0.003f;
bool subiendoHumo = true;
bool subiendoPosHumo = true;

// Variables para animación
float rotAeolipile = 0.0f;
float timerAeolipile = 0.0f;
bool animacionActiva = false;

// Configuración de intervalos (en segundos)
const float TIEMPO_ESPERA = 50.0f;   // No hace nada
const float TIEMPO_GIRANDO = 270.0f;  // Gira
const float TIEMPO_TOTAL = 500.0f;    // Se detiene (Espera + Gira + Margen)

// Variables para animación de la catapulta
float rotCatapulta = 0.0f;
const float TIEMPO_CATAPULTA_INICIO = TIEMPO_ESPERA + 60.0f; // Empieza poco después que el Aeolipile

// --- Variables de la Bala  ---
float posX_Bala = 2.5f;
float posY_Bala = 7.0f;
float posZ_Bala = -0.5f;
bool balaLanzada = false;
float rotContinuaZ = 0.0f; 

// --- Variables de Estado de la Bala ---
int estadoBala = 0; // 0: Unida al brazo, 1: Vuelo parabólico, 2: Rebotando, 3: Detenida
float tiempoVueloBala = 0.0f;
float posX_Lanzamiento = 0.0f;
float posY_Lanzamiento = 0.0f;
float posX_Bala_Actual = 0.0f;
float posY_Bala_Actual = 0.0f;
int numRebotes = 0;

// Variables físicas 
float velX_Bala = -4.0f;    // Negativo para ir a la izquierda, mayor para más distancia
float velY_Bala = 6.0f;     // Mayor para un arco de lanzamiento inicial más alto
float gravedadBala = 6.0f; // Ligeramente menor para que la bala "flote" más tiempo

//Variables texturas
Texture humoTexture;
Texture fuegoTexture;
Texture AgujeroTexture; 

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


//Modelos aelopile
Model Aeolipile_base_M;
Model Aeolipile_M;
//modelos de catapulta y esfera
Model Catapulta_M;
Model Bala_M;

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



//cálculo del promedio de las normales para sombreado de Phong
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
	   4,5,6,
	   4,6,7
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	

	//Carga texturas
	humoTexture = Texture("Textures/humo.tga");
	humoTexture.LoadTextureA();
	fuegoTexture = Texture("Textures/fuego.tga");
	fuegoTexture.LoadTextureA();
	AgujeroTexture = Texture("Textures/agujero.tga");
	AgujeroTexture.LoadTextureA();

	//Carga modelos
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");
	Catapulta_M = Model();
	Catapulta_M.LoadModel("Models/catapulta.obj");
	Bala_M = Model();
	Bala_M.LoadModel("Models/bala_cañon.obj");

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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		// Si se presiona R, activamos la secuencia y reseteamos el cronómetro
		if (mainWindow.getReiniciarAnimacion()) {
			animacionActiva = true;
			timerAeolipile = 0.0f;
			rotAeolipile = 0.0f; 
			rotCatapulta = 0.0f;

			// Reset de la bala
			estadoBala = 0;
			tiempoVueloBala = 0.0f;
			numRebotes = 0;
			velX_Bala = -8.0f; // Asegurar dirección izquierda al reiniciar
			velY_Bala = 6.0f;  // Asegurar arco alto

			mainWindow.apagarReinicio(); // Apagamos la bandera para que no reinicie cada frame
		}

		//--- Animación ---
		if (animacionActiva) {
			timerAeolipile += deltaTime;

			// 1. Animación de Textura de Fuego
			if (derechaFuego) {
				toffsetfuegou += 0.02f * deltaTime; // Aumentado de 0.005f a 0.02f
				if (toffsetfuegou >= 0.1f) derechaFuego = false;
			}
			else {
				toffsetfuegou -= 0.02f * deltaTime; // Aumentado de 0.005f a 0.02f
				if (toffsetfuegou <= 0.0f) derechaFuego = true;
			}

			// 2. Rotación del Aeolipile
			if (timerAeolipile > TIEMPO_ESPERA && timerAeolipile <= (TIEMPO_ESPERA + TIEMPO_GIRANDO)) {
				rotAeolipile += 10.0f * deltaTime;
			}

			// 3. Lógica de la Catapulta y Disparo
			if (timerAeolipile > TIEMPO_CATAPULTA_INICIO && timerAeolipile <= (TIEMPO_ESPERA + TIEMPO_GIRANDO)) {
				float tiempoRelativo = timerAeolipile - TIEMPO_CATAPULTA_INICIO;
				float duracionFase = TIEMPO_GIRANDO / 4.0f;

				if (tiempoRelativo < duracionFase) {
					// Fase 1: Retroceso
					rotCatapulta -= 0.5f * deltaTime;
					if (rotCatapulta < -45.0f) rotCatapulta = -45.0f;
				}
				else {
					// Fase 2: Lanzamiento rápido
					rotCatapulta += 2.5f * deltaTime; // Un poco más de impulso

					if (rotCatapulta >= 45.0f) {
						rotCatapulta = 45.0f;

						// --- DISPARO CORREGIDO ---
						if (estadoBala == 0) {
							estadoBala = 1;
							tiempoVueloBala = 0.0f; // REINICIO CRÍTICO: Evita que "salte" al infinito

							// Cálculo de posición inicial global (Pivote en 0.0, 4.5)
							float theta = rotCatapulta * toRadians;
							float dx = 2.5f; // Distancia horizontal de la bala al pivote en reposo
							float dy = 2.5f; // Distancia vertical (7.0 - 4.5)

							// Rotación de la posición relativa para obtener la global
							posX_Lanzamiento = 0.0f + (dx * cos(theta) - dy * sin(theta));
							posY_Lanzamiento = 4.5f + (dx * sin(theta) + dy * cos(theta));

							posX_Bala_Actual = posX_Lanzamiento;
							posY_Bala_Actual = posY_Lanzamiento;
						}
					}
				}
			}

			// 4. Lógica de Físicas: Tiro Parabólico y Rebote
			if (estadoBala == 1 || estadoBala == 2) {
				// --- AJUSTE AQUÍ ---
				// Cambiamos 0.12f por un valor menor (ej. 0.05f) para ralentizar el vuelo.
				// Cuanto menor sea el número, más lenta será la parábola y el rebote.
				tiempoVueloBala += deltaTime * 0.05f;

				// Ecuaciones de movimiento (Se mantienen igual)
				posX_Bala_Actual = posX_Lanzamiento + (velX_Bala * tiempoVueloBala);
				posY_Bala_Actual = posY_Lanzamiento + (velY_Bala * tiempoVueloBala) - (0.5f * gravedadBala * pow(tiempoVueloBala, 2));

				// Detección de suelo
				if (posY_Bala_Actual <= -1.5f) {
					posY_Bala_Actual = -1.5f;

					if (numRebotes < 4) {
						estadoBala = 2;
						numRebotes++;

						// El tiempo se resetea a 0, por lo que la "lentitud" se aplica 
						// automáticamente al nuevo arco del rebote.
						tiempoVueloBala = 0.0f;
						posX_Lanzamiento = posX_Bala_Actual;
						posY_Lanzamiento = posY_Bala_Actual;

						velY_Bala = abs(velY_Bala) * 0.75f;
						velX_Bala *= 0.9f;
					}
					else {
						estadoBala = 3;
					}
				}
			}

			if (timerAeolipile > (TIEMPO_ESPERA + TIEMPO_GIRANDO)) {
				animacionActiva = false;
			}
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//reiniciar variables antes de que sean enviadas al shader

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		
		// --- 1. AEOLIPILE BASE (Estática) --- 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_base_M.RenderModel();

		// --- 2. AEOLIPILE ESFERA (Rotación en Z con R) ---
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, rotAeolipile * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();

		// ========== ANIMACIÓN DEL HUMO  ==========
		if (subiendoHumo) {
			toffsetHumoV += velocidadHumo;
			if (toffsetHumoV >= 0.09f) {  // Rango pequeño
				subiendoHumo = false;
			}
		}
		else {
			toffsetHumoV -= velocidadHumo;
			if (toffsetHumoV <= 0.0f) {
				subiendoHumo = true;
			}
		}

		// ========== RENDERIZADO DE LAS INSTANCIAS DE HUMO ========== 
		// Solo se dibujan si la animación está activa Y estamos en el intervalo de giro
		if (animacionActiva && (timerAeolipile > TIEMPO_ESPERA && timerAeolipile <= (TIEMPO_ESPERA + TIEMPO_GIRANDO)))
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			humoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Animación de la textura
			toffset = glm::vec2(0.0f, toffsetHumoV);
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

			// --- INSTANCIA 1 (Derecha) ---
			glm::mat4 modelHumo1 = model; // 'model' ya tiene la rotación de la esfera
			modelHumo1 = glm::translate(modelHumo1, glm::vec3(0.6f, 0.8f, 0.0f));
			modelHumo1 = glm::rotate(modelHumo1, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo1));
			meshList[5]->RenderMesh();

			// --- INSTANCIA 2 (Izquierda - Rotada 180 en Z) ---
			glm::mat4 modelHumo2 = model;
			modelHumo2 = glm::rotate(modelHumo2, 180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			modelHumo2 = glm::translate(modelHumo2, glm::vec3(0.6f, 0.8f, 0.0f));
			modelHumo2 = glm::rotate(modelHumo2, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelHumo2));
			meshList[5]->RenderMesh();
			glDisable(GL_BLEND);
		}


		// ========== RENDERIZADO DEL PLANO DE FUEGO (Vinculado a R) ========== 
		if (animacionActiva && (timerAeolipile <= TIEMPO_ESPERA + TIEMPO_FUEGO))
		{
			if (derechaFuego) {
				toffsetfuegou += velocidadFuego;
				if (toffsetfuegou >= 0.1f) derechaFuego = false;
			}
			else {
				toffsetfuegou -= velocidadFuego;
				if (toffsetfuegou <= 0.0f) derechaFuego = true;
			}

			toffset = glm::vec2(toffsetfuegou, 0.0f);

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 0.7f, 0.1f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			fuegoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[5]->RenderMesh();
			glDisable(GL_BLEND);
		}

		// --- RENDERIZADO DE LA CATAPULTA  ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.5f, -0.5f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (45.0f + rotCatapulta) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		// Envío de uniformes y renderizado
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Catapulta_M.RenderModel();

		// --- RENDERIZADO DE LA BALA ---
		model = glm::mat4(1.0);
		if (estadoBala == 0) {
			// ESTADO 0: Unida al brazo (Movimiento Jerárquico)
			model = glm::translate(model, glm::vec3(0.0f, 4.5f, -0.5f)); // Traslación al pivote
			model = glm::rotate(model, rotCatapulta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación del brazo
			model = glm::translate(model, glm::vec3(posX_Bala - 0.0f, posY_Bala - 4.5f, posZ_Bala - (-0.5f))); // Posición relativa
		}
		else {
			// ESTADOS 1, 2 y 3: Vuelo parabólico, rebote o detenida en el suelo (Independiente)
			model = glm::translate(model, glm::vec3(posX_Bala_Actual, posY_Bala_Actual, posZ_Bala));
		}
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		Bala_M.RenderModel();

		
		// ========== RENDERIZADO DEL  HUECO  ========== 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-42.0f, -1.5f, -0.2f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AgujeroTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
} 