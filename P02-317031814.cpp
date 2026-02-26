//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

const float toRadians = 3.14159265f / 180.0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

float angulo = 0.0f;

//Modificación a Pirámide (base cuadrada, 4 caras triangulares) 
void CreaPiramide()
{
	unsigned int indices[] = {
		// Cara frontal
		0, 1, 4,
		// Cara derecha
		1, 2, 4,
		// Cara trasera
		2, 3, 4,
		// Cara izquierda
		3, 0, 4,
		// Base (cuadrada) - dividida en 2 triángulos
		0, 2, 1,
		0, 3, 2
	};
		GLfloat vertices[] = {
		// Base - Vértices de la base cuadrada (y = -0.5f)
		-0.5f, -0.5f,  0.5f,  // 0: Frontal izquierdo
		 0.5f, -0.5f,  0.5f,  // 1: Frontal derecho
		 0.5f, -0.5f, -0.5f,  // 2: Trasero derecho
		-0.5f, -0.5f, -0.5f,  // 3: Trasero izquierdo

		// Punta de la pirámide (y = 0.5f)
		 0.0f,  0.5f,  0.0f   // 4: Punta superior
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 15, 18); // 5 vértices * 3 coordenadas = 15, 18 índices
	meshList.push_back(obj1);
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4,
		3, 2, 6, 6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Función para crear la letra D mayúscula
void CrearLetraD()
{
	// Vértices para la letra D mayúscula (estilo bloque)
	GLfloat vertices_D[] = {
		// Barra vertical izquierda
		-0.8f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 0
		-0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 1
		-0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 2
		-0.8f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 3
		-0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 4
		-0.8f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 5
		// Barra superior
		-0.4f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 6
		 0.4f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 7
		 0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 8
		-0.4f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 9
		 0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 10
		-0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 11
		// Barra inferior
		-0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 12
		 0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 13
		 0.4f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 14
		-0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 15
		 0.4f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 16
		-0.4f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,  // 17
		// Barra vertical derecha (curva simulada)
		 0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 18
		 0.7f, -0.3f, 0.5f,     1.0f, 0.0f, 0.0f,  // 19
		 0.7f,  0.3f, 0.5f,     1.0f, 0.0f, 0.0f,  // 20
		 0.4f, -0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 21
		 0.7f,  0.3f, 0.5f,     1.0f, 0.0f, 0.0f,  // 22
		 0.4f,  0.8f, 0.5f,     1.0f, 0.0f, 0.0f,  // 23
	};
	MeshColor* letraD = new MeshColor();
	letraD->CreateMeshColor(vertices_D, 144); // 24 vértices * 6 componentes (X,Y,Z,R,G,B)
	meshColorList.push_back(letraD);
}

// Función para crear la letra T mayúscula (azul)
void CrearLetraT()
{
	// Vértices para la letra T mayúscula
	GLfloat vertices_T[] = {
		// Barra superior horizontal
		-0.6f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 0 - Azul
		 0.6f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 1
		 0.6f,  0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 2
		-0.6f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 3
		 0.6f,  0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 4
		-0.6f,  0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 5
		// Barra vertical central
		-0.2f, -0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 6
		 0.2f, -0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 7
		 0.2f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 8
		-0.2f, -0.8f, 0.5f,     0.0f, 0.0f, 1.0f,  // 9
		 0.2f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 10
		-0.2f,  0.4f, 0.5f,     0.0f, 0.0f, 1.0f,  // 11
	};
	MeshColor* letraT = new MeshColor();
	letraT->CreateMeshColor(vertices_T, 72); // 12 vértices * 6 componentes
	meshColorList.push_back(letraT);
}

// Función para crear la letra J mayúscula (verde) 
void CrearLetraJ()
{
	// Vértices para la letra J mayúscula con forma de gancho
	GLfloat vertices_J[] = {
		// Barra superior horizontal
		-0.6f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 0 - Verde
		 0.6f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 1
		 0.6f,  0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 2
		-0.6f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 3
		 0.6f,  0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 4
		-0.6f,  0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 5
		// Barra vertical central
		-0.2f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 6
		 0.2f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 7
		 0.2f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 8
		-0.2f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 9
		 0.2f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 10
		-0.2f,  0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 11
		//Barra inferior horizontal (gancho)
		-0.6f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 12
		-0.2f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 13
		-0.2f, -0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 14
		-0.6f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 15
		-0.2f, -0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 16
		-0.6f, -0.4f, 0.5f,     0.0f, 1.0f, 0.0f,  // 17
		//Barra vertical izquierda inferior (gancho)
		-0.6f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 18
		-0.4f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 19
		-0.4f, -0.6f, 0.5f,     0.0f, 1.0f, 0.0f,  // 20
		-0.6f, -0.8f, 0.5f,     0.0f, 1.0f, 0.0f,  // 21
		-0.4f, -0.6f, 0.5f,     0.0f, 1.0f, 0.0f,  // 22
		-0.6f, -0.6f, 0.5f,     0.0f, 1.0f, 0.0f,  // 23
	};
	MeshColor* letraJ = new MeshColor();
	letraJ->CreateMeshColor(vertices_J, 144); // 24 vértices * 6 componentes
	meshColorList.push_back(letraJ);
}


void CreateShaders()
{
	// Shaders originales
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1); // [0]
	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2); // [1]

	// Shaders para la casa (usando shader_colores.vert y .frag por color)
	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles("shaders/shader_colores.vert", "shaders/rojo.frag");
	shaderList.push_back(*shaderRojo); // [2]

	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles("shaders/shader_colores.vert", "shaders/verde.frag");
	shaderList.push_back(*shaderVerde); // [3]

	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles("shaders/shader_colores.vert", "shaders/azul.frag");
	shaderList.push_back(*shaderAzul); // [4]

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles("shaders/shader_colores.vert", "shaders/cafe.frag");
	shaderList.push_back(*shaderCafe); // [5]

	Shader* shaderVerdeOscuro = new Shader();
	shaderVerdeOscuro->CreateFromFiles("shaders/shader_colores.vert", "shaders/verdeoscuro.frag");
	shaderList.push_back(*shaderVerdeOscuro); // [6]

}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();    //índice 1 en MeshList
	CrearLetraD();  //índice 0 en meshColorList
	CrearLetraT();  //índice 1 en meshColorList
	CrearLetraJ();  //índice 2 en meshColorList

	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f),
	mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	glm::mat4 model(1.0);

	
	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ====================================================================================//
		//				SECCIÓN 1: INICIALES DE COLORES
		// ====================================================================================//

		/*
		// Usar shader para figuras con color
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
			
		// Dibujar la letra D mayúscula (roja) - Margen inferior izquierdo
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f)); // Escala reducida
		model = glm::translate(model, glm::vec3(-1.2f, -1.8f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[0]->RenderMeshColor(); // Letra D

		// Dibujar la letra T mayúscula (azul) - Centro
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f)); // Misma escala
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[1]->RenderMeshColor(); // Letra T

		// Dibujar la letra J mayúscula (verde) - Margen superior derecho
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f)); // Misma escala
		model = glm::translate(model, glm::vec3(1.5f, 1.4f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[2]->RenderMeshColor(); // Letra J
		*/
		

		// ====================================================================================//
		//									SECCIÓN 2: CASA EN 3D
		// ====================================================================================//
		
		// Actualizar ángulo para rotación (más lento)
		angulo += 0.025f; // Reducido de 0.5f a 0.025f para que gire más lento

		// CASA: CUERPO DE LA CASA (cubo rojo)
		shaderList[2].useShader(); // Shader rojo (índice 2)
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)


		// CASA: TECHO AZUL (pirámide)
		shaderList[4].useShader(); // Shader azul (índice 4)
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: más ancha que la casa
		model = glm::scale(model, glm::vec3(2.8f, 1.5f, 2.5f));
		// Posición: encima del cubo
		model = glm::translate(model, glm::vec3(0.0f, 1.25f, -4.0f));
		// Misma rotación que el cubo
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); // Pirámide (índice 0 en meshList)


		// CASA: PUERTA VERDE 
		shaderList[3].useShader(); // Shader verde (índice 3)
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.05f, alto 0.05f, profundo 0.2f
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.2f));
		// Posición: centrado y hacia abajo
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -3.9f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)

		// CASA: VENTANA DERECHA VERDE 
		shaderList[3].useShader(); // Shader verde (índice 3)
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.04f, alto 0.04f, profundo 0.2f
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.2f));
		// Posición: arriba y hacia la derecha
		model = glm::translate(model, glm::vec3(1.5f, 1.0f, -3.9f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)
		

		// CASA: VENTANA IZQUIERDA VERDE 
		shaderList[3].useShader(); // Shader verde (índice 3)
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.04f, alto 0.04f, profundo 0.2f
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.2f));
		// Posición: arriba y hacia la izquierda
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, -3.9f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)

		// CASA: TRONCO DERECHO (café)
		shaderList[5].useShader(); // Shader café (índice 5)
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.04f, alto 0.04f, profundo 0.2f (forma de tronco)
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.2f));
		// Posición: a la derecha de la casa (X positivo), en la base (Y negativo)
		model = glm::translate(model, glm::vec3(7.5f, -2.6f, -4.5f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)

		// CASA: TRONCO IZQUIERDO (café)
		shaderList[5].useShader(); // Shader café (índice 5)
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.04f, alto 0.04f, profundo 0.2f (forma de tronco)
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.2f));
		// Posición: a la derecha de la casa (X negativo), en la base (Y negativo)
		model = glm::translate(model, glm::vec3(-7.5f, -2.6f, -4.5f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); // Cubo (índice 1 en meshList)

		// CASA: ÁRBOL DERECHO (verde oscuro) 
		shaderList[6].useShader(); // Shader verde oscuro (índice 6)
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.5f, alto 0.8f, profundo 0.8f (forma de árbol)
		model = glm::scale(model, glm::vec3(0.5f, 0.8f, 0.8f));
		// Posición: sobre el tronco derecho (X positivo), en la base del tronco
		model = glm::translate(model, glm::vec3(2.15f, 0.13f, -4.0f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); // Pirámide (índice 0 en meshList)


		// CASA: ÁRBOL IZQUIERDO (verde oscuro)
		shaderList[6].useShader(); // Shader verde oscuro (índice 6)
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		// Escala: ancho 0.5f, alto 0.8f, profundo 0.8f (forma de árbol)
		model = glm::scale(model, glm::vec3(0.5f, 0.8f, 0.8f));
		// Posición: sobre el tronco derecho (X negativo), en la base del tronco
		model = glm::translate(model, glm::vec3(-2.15f, 0.13f, -4.0f));
		// Misma rotación que la casa
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); // Pirámide (índice 0 en meshList)
		

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
