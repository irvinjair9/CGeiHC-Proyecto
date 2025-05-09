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

//La musica
#include "inc/fmod.hpp"
#include <inc/fmod_errors.h>

FMOD::System* fmodSystem;
FMOD::Sound* music;
FMOD::Channel* channel = nullptr;


//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;



//Constante para la animaci�n
float angulovaria = 0.0f;

float sunAngle = 0.0f;
//float sunSpeed = 0.05f;
float sunSpeed = 0.1f; //Velocidad de rotaci�n del sol


// Variables para la animaci�n de bateo
float tiempoLanzamiento = 0.0f;          // Contador para el lanzamiento de pelotas
const float intervaloLanzamiento = 3.0f;  // Intervalo de 3 segundos entre lanzamientos
bool pelotaEnMovimiento = false;         // Indica si la pelota est� en movimiento
float velocidadPelota = 1.0f;           // Velocidad de la pelota
glm::vec3 posicionPelota;                // Posici�n actual de la pelota
glm::vec3 posicionInicial;               // Posici�n inicial de la pelota
float distanciaPelota = 0.0f;            // Distancia recorrida por la pelota

// Variables para la animaci�n del brazo y bate
float anguloGolpe = 0.0f;                // �ngulo de rotaci�n para el golpe
float velocidadGolpe = 0.95f;           // Velocidad de rotaci�n del brazo
bool preparandoGolpe = false;            // Indica si est� preparando el golpe
bool batGolpeando = false;               // Indica si el bat est� golpeando
bool regresandoBat = false;              // Indica si el bat est� regresando a posici�n

// Variables para controlar la aparici�n/desaparici�n de las medusas
struct EstadoMedusa {
	bool visible;          // Si la medusa est� visible o no
	float altura;          // Posici�n Y actual de la medusa
	float alturaInicial;   // Altura base cuando est� oculta
	float alturaMaxima;    // Altura m�xima cuando est� completamente visible
	float velocidad;       // Velocidad de movimiento
	float tiempoVisible;   // Cu�nto tiempo permanece visible
	float tiempoActual;    // Contador de tiempo actual
	bool golpeada;         // Si fue golpeada por el martillo
	float tiempoGolpeada;  // Tiempo que lleva golpeada
};

// Variables adicionales para ciclos de aparici�n de medusas
float cicloAparicion = 50.0f;       // Tiempo total del ciclo completo
float tiempoEntreApariciones = 4.0f; // Tiempo entre cada aparici�n de medusa
float tiempoActualCiclo = 0.0f;    // Contador del ciclo actual

// Variables para la animaci�n de Jake (martillo)
float anguloMartillo = 0.0f;
bool martilloEnMovimiento = false;
float velocidadMartillo = 5.0f;
float anguloMaximoMartillo = 90.0f;
bool martilloSubiendo = false;

// Variables adicionales para controlar el movimiento autom�tico del martillo
float tiempoEntreGolpes = 0.1f;     // Tiempo entre golpes autom�ticos del martillo
float tiempoActualMartillo = 0.0f;  // Contador de tiempo para los golpes

// Crear un arreglo para controlar el estado de cada medusa 
EstadoMedusa estadoMedusas[5];


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture pisoETexture; //Piso del kiosko
Texture pisoH; //Piso Hora de aventura
Texture pisoB; //Piso Bob Esponja
Texture pisoL; //Piso Los padrinos magicos
Texture camino; //Piso camino de conexi�n

//Modelos hora de aventura
Model BMO;
Model CasaDelArbol;
Model JakeCuerpo;
Model JakeBrazoDer;
Model JakeBrazoIzq;
Model JakePiernaDer;
Model JakePiernaIzq;
Model Prismo;
Model MarcelinesGuitar;
Model CoinReceptor;





//Modelos Bob esponja
Model Banca;
Model BancaCom;
Model LamparaBob;
Model CasaBob;
Model BobBrazoDer;
Model BobBrazoIzq;
Model BobCuerpo;
Model BobPiernaDer;
Model BobPiernaIzq;
Model Cangre;
Model CasaCalam;
Model CasaPatricio;
Model CrustaceoCas;
Model FredN;
Model HaroldN;
Model Jaula;
Model Pizzas;
Model Topo;
Model Bat;
Model Martillo;
Model Medusa1;
Model Pelota;
Model Coin;



//Modelos Los padrinos magicos
Model varita;
Model Cosmo;
Model Wanda;
Model Timmy;
Model CasaTimmy;


Skybox skyboxDay;
Skybox skyboxNight;

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


//funci�n de calculo de normales por promedio de v�rtices 
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

//Funci�n para la musica con Fmod
void iniciarFMOD() {
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	fmodSystem->createSound("sounds/Fondo_Feria.wav", FMOD_LOOP_NORMAL, 0, &music);
	fmodSystem->playSound(music, 0, false, &channel);
}

void actualizarFMOD() {
	fmodSystem->update();
}

void liberarFMOD() {
	music->release();
	fmodSystem->close();
	fmodSystem->release();
}

//Creaci�n de bancas para decorar
void RenderBanca(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	Banca.RenderModel();
}

//Creaci�n de bancas comedor para decorar
void RenderBancaCom(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	BancaCom.RenderModel();
}

//Creaci�n lamparas 
void RenderLamparaBob(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(0.7f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	LamparaBob.RenderModel();
}

void RenderVarita(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1000.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	varita.RenderModel();
}

void RenderRecibidorMonedas(glm::vec3 posicion, float rotY, GLuint uniformModel, glm::vec3 escala = glm::vec3(1.0f)) {
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::translate(base, posicion);
	base = glm::rotate(base, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, escala);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	CoinReceptor.RenderModel();
}




//ANIMACION---------------------------------------------------------------


// JAULA DE BATEO -------------------------------------------------------
// Funci�n para actualizar la animaci�n de la jaula de bateo
void actualizarAnimacionBateo(GLfloat deltaTime) {
	// Actualizar el temporizador de lanzamiento
	tiempoLanzamiento += deltaTime;

	// Si no hay pelota en movimiento y es tiempo de lanzar una nueva
	if (!pelotaEnMovimiento && tiempoLanzamiento >= intervaloLanzamiento) {
		tiempoLanzamiento = 0.0f;
		pelotaEnMovimiento = true;
		distanciaPelota = 0.0f;
		// Guardar la posici�n inicial de la pelota (seg�n tu c�digo)
		posicionInicial = glm::vec3(-23.0f, -0.3f, -1.5f);
		posicionPelota = posicionInicial;

		// Preparar el brazo para golpear
		preparandoGolpe = true;
		anguloGolpe = 0.0f;
	}

	// Si la pelota est� en movimiento, actualizar su posici�n
	if (pelotaEnMovimiento) {
		// Calcular la nueva distancia recorrida
		distanciaPelota += velocidadPelota * deltaTime;

		// Calcular la posici�n interpolada de la pelota desde la m�quina hasta cerca de Jake
		float t = distanciaPelota / 100.0f;
		if (t > 1.0f) t = 1.0f;

		// La pelota se mueve hacia Jake 
		float newX = posicionInicial.x + t * 100.0f; // Mover 50 unidades en X
		float newY = posicionInicial.y;

		// Actualizar la posici�n de la pelota
		posicionPelota = glm::vec3(newX, newY, posicionInicial.z);

		// Si la pelota llega cerca de Jake (donde est� el bat)
		if (t >= 0.7f && !batGolpeando && preparandoGolpe) {
			// Iniciar el golpe
			batGolpeando = true;
			preparandoGolpe = false;
		}

		// Si la pelota completa su recorrido
		if (t >= 1.0f) {
			// Reiniciar para el pr�ximo lanzamiento
			pelotaEnMovimiento = false;
		}
	}

	// Animaci�n del brazo y bate
	if (preparandoGolpe) {
		// Preparaci�n: llevar el brazo hacia atr�s
		anguloGolpe -= velocidadGolpe * 0.5f * deltaTime;
		if (anguloGolpe <= -45.0f) {
			anguloGolpe = -45.0f;
		}
	}
	else if (batGolpeando) {
		// Golpe: mover el brazo hacia adelante r�pidamente
		anguloGolpe += velocidadGolpe * deltaTime;
		if (anguloGolpe >= 30.0f) {
			anguloGolpe = 30.0f;
			batGolpeando = false;
			regresandoBat = true;
		}

		// Si el bat golpea la pelota (detecci�n simple de colisi�n)
		if (anguloGolpe > 0.0f && anguloGolpe < 15.0f && pelotaEnMovimiento) {
			// La distancia de la pelota est� alrededor del punto donde ocurrir�a el contacto
			float posX = posicionPelota.x - posicionInicial.x;
			if (posX > 55.0f && posX < 65.0f) {  // Si la pelota est� cerca de Jake
				// Cambiar la direcci�n de la pelota (simular que fue golpeada)
				velocidadPelota *= 0.5f; // Acelerar la pelota
				// Invertir direcci�n (la pelota vuelve en direcci�n opuesta)
				distanciaPelota = 100.0f * 0.6f; // Reiniciar la distancia recorrida
			}
		}
	}
	else if (regresandoBat) {
		// Regresar lentamente a la posici�n neutral
		anguloGolpe -= velocidadGolpe * 0.3f * deltaTime;
		if (anguloGolpe <= 0.0f) {
			anguloGolpe = 0.0f;
			regresandoBat = false;
		}
	}
}

//GOLPEA AL TOPO--------------------------------------------------------------

// Funci�n para inicializar los estados de las medusas
void inicializarMedusas() {
	// Inicializar semilla para n�meros aleatorios
	srand(static_cast<unsigned int>(time(nullptr)));

	// Configuraci�n inicial para todas las medusas
	for (int i = 0; i < 5; i++) {
		estadoMedusas[i].visible = false;
		estadoMedusas[i].alturaInicial = 3.3f;    // Altura cuando est� oculta
		estadoMedusas[i].alturaMaxima = 10.0f;     // Altura cuando est� visible
		estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
		estadoMedusas[i].velocidad = 0.1f + (rand() % 10) / 300.0f;  // Velocidad aleatoria
		estadoMedusas[i].tiempoVisible = 6.0f + (rand() % 20) / 10.0f;  // Tiempo visible aleatorio
		estadoMedusas[i].tiempoActual = 0.0f;
		estadoMedusas[i].golpeada = false;
		estadoMedusas[i].tiempoGolpeada = 0.0f;
	}
}

// Funci�n para actualizar la l�gica de animaci�n de las medusas
void actualizarMedusas(float deltaTime) {
	// Actualizar el contador del ciclo
	tiempoActualCiclo += deltaTime;
	if (tiempoActualCiclo > cicloAparicion) {
		tiempoActualCiclo = 0.0f;
	}

	// Fase del ciclo para determinar qu� medusas aparecen
	float faseCiclo = tiempoActualCiclo / cicloAparicion;

	for (int i = 0; i < 5; i++) {
		// Momento espec�fico para que aparezca cada medusa 
		float momentoAparicion = (float)i / 5.0f;
		float margenAparicion = tiempoEntreApariciones / cicloAparicion;

		// Verificar si es el momento de que esta medusa aparezca
		bool debeAparecer = false;
		if (faseCiclo > momentoAparicion && faseCiclo < momentoAparicion + margenAparicion) {
			debeAparecer = true;
		}

		if (estadoMedusas[i].visible) {
			// Si la medusa est� visible, actualizar su tiempo
			estadoMedusas[i].tiempoActual += deltaTime;

			// Si ya pas� su tiempo visible, comenzar a ocultarla
			if (estadoMedusas[i].tiempoActual >= estadoMedusas[i].tiempoVisible) {
				estadoMedusas[i].altura -= estadoMedusas[i].velocidad * deltaTime;

				// Si lleg� a su posici�n inicial, marcarla como no visible
				if (estadoMedusas[i].altura <= estadoMedusas[i].alturaInicial) {
					estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
					estadoMedusas[i].visible = false;
					estadoMedusas[i].tiempoActual = 0.0f;
				}
			}
			else {
				// Si todav�a est� en tiempo visible, asegurarse de que est� en su altura m�xima
				if (estadoMedusas[i].altura < estadoMedusas[i].alturaMaxima) {
					estadoMedusas[i].altura += estadoMedusas[i].velocidad * deltaTime;
					if (estadoMedusas[i].altura > estadoMedusas[i].alturaMaxima) {
						estadoMedusas[i].altura = estadoMedusas[i].alturaMaxima;
					}
				}
			}
		}
		else if (debeAparecer) {
			// Si la medusa debe aparecer seg�n el ciclo
			estadoMedusas[i].visible = true;
			estadoMedusas[i].tiempoActual = 0.0f;

			// Variar ligeramente la velocidad y tiempo para que no sea demasiado predecible
			estadoMedusas[i].velocidad = 0.1f + (rand() % 10) / 300.0f;
			estadoMedusas[i].tiempoVisible = 6.0f + (rand() % 20) / 20.0f;
		}
	}

	// A�adir aleatorizaci�n ocasional - medusa extra aleatoria cada cierto tiempo
	if (rand() % 100 < 2) {
		int medusaAleatoria = rand() % 5;
		if (!estadoMedusas[medusaAleatoria].visible) {
			estadoMedusas[medusaAleatoria].visible = true;
			estadoMedusas[medusaAleatoria].tiempoActual = 0.0f;
			estadoMedusas[medusaAleatoria].velocidad = 0.15f + (rand() % 10) / 100.0f; // M�s r�pida
			estadoMedusas[medusaAleatoria].tiempoVisible = 5.0f + (rand() % 10) / 20.0f; // Menos tiempo visible
		}
	}
}

// Funci�n para actualizar la animaci�n del martillo
void actualizarMartillo(float deltaTime) {
	// Actualizar el contador de tiempo
	tiempoActualMartillo += deltaTime;

	// Si no est� en movimiento y ha pasado el tiempo necesario, iniciar un nuevo golpe
	if (!martilloEnMovimiento && tiempoActualMartillo >= tiempoEntreGolpes) {
		martilloEnMovimiento = true;
		martilloSubiendo = true;
		tiempoActualMartillo = 0.0f;
	}

	// Animar el martillo si est� en movimiento
	if (martilloEnMovimiento) {
		if (martilloSubiendo) {
			anguloMartillo += velocidadMartillo * deltaTime;
			if (anguloMartillo >= anguloMaximoMartillo) {
				anguloMartillo = anguloMaximoMartillo;
				martilloSubiendo = false;
			}
		}
		else {
			anguloMartillo -= velocidadMartillo * deltaTime;
			if (anguloMartillo <= 0.0f) {
				anguloMartillo = 0.0f;
				martilloEnMovimiento = false;
			}
		}
	}
}

void actualizarAnimaciones(float deltaTime) {
	actualizarMedusas(deltaTime);
	actualizarMartillo(deltaTime);
}





int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();





	mainLight.SetAmbientIntensity(0.05f); //intensidad de la luz
	mainLight.SetDiffuseIntensity(0.05f); //intensidad de la luz difusa


	CreateObjects();
	CreateShaders();
	iniciarFMOD(); //iniciar la musica


	camera = Camera(glm::vec3(-10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
	
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	pisoH = Texture("Textures/pisoH.tga");
	pisoH.LoadTextureA();
	pisoB = Texture("Textures/pisoB.tga");
	pisoB.LoadTextureA();
	pisoL = Texture("Textures/pisoL.tga");
	pisoL.LoadTextureA();
	pisoETexture = Texture("Textures/pisoE.tga");
	pisoETexture.LoadTextureA();

	camino = Texture("Textures/camino.tga");
	camino.LoadTextureA();

	//Modelos hora de aventura
	BMO = Model();
	BMO.LoadModel("Models/BMO.obj");

	CasaDelArbol = Model();
	CasaDelArbol.LoadModel("Models/CasaDelArbol.obj");

	JakeCuerpo = Model();
	JakeCuerpo.LoadModel("Models/CuerpoJake.obj");

	JakeBrazoDer = Model();
	JakeBrazoDer.LoadModel("Models/BrazoDerechoJake.obj");

	JakeBrazoIzq = Model();
	JakeBrazoIzq.LoadModel("Models/BrazoIzquierdoJake.obj");

	JakePiernaDer = Model();
	JakePiernaDer.LoadModel("Models/PiernaDerechaJake.obj");

	JakePiernaIzq = Model();
	JakePiernaIzq.LoadModel("Models/PiernaIzquierdaJake.obj");

	Prismo = Model();
	Prismo.LoadModel("Models/Prismo.obj");

	MarcelinesGuitar = Model();
	MarcelinesGuitar.LoadModel("Models/MarcelinesGuitar.obj");

	CoinReceptor = Model();
	CoinReceptor.LoadModel("Models/CoinReceptor.obj");






	//Modelos Bob esponja
	Banca = Model();
	Banca.LoadModel("Models/Banca.obj");

	BancaCom = Model();
	BancaCom.LoadModel("Models/BancaCom.obj");

	LamparaBob = Model();
	LamparaBob.LoadModel("Models/LamparaBob.obj");

	CasaBob = Model();
	CasaBob.LoadModel("Models/CasaBob.obj");

	BobBrazoDer = Model();
	BobBrazoDer.LoadModel("Models/BobBrazoDer1.obj");

	BobBrazoIzq = Model();
	BobBrazoIzq.LoadModel("Models/BobBrazoIzq1.obj");

	BobCuerpo = Model();
	BobCuerpo.LoadModel("Models/BobCuerpo1.obj");

	BobPiernaDer = Model();
	BobPiernaDer.LoadModel("Models/BobPiernaDer1.obj");

	BobPiernaIzq = Model();
	BobPiernaIzq.LoadModel("Models/BobPiernaIzq1.obj");

	Cangre = Model();
	Cangre.LoadModel("Models/Cangre.obj");

	CasaCalam = Model();
	CasaCalam.LoadModel("Models/CasaCalam.obj");

	CasaPatricio = Model();
	CasaPatricio.LoadModel("Models/CasaPatricio.obj");

	CrustaceoCas = Model();
	CrustaceoCas.LoadModel("Models/CrustaceoCas.obj");

	FredN = Model();
	FredN.LoadModel("Models/FredN.obj");

	HaroldN = Model();
	HaroldN.LoadModel("Models/HaroldN.obj");

	Jaula = Model();
	Jaula.LoadModel("Models/Jaula.obj");

	Pizzas = Model();
	Pizzas.LoadModel("Models/Pizzas.obj");

	Topo = Model();
	Topo.LoadModel("Models/Topo.obj");

	Bat = Model();
	Bat.LoadModel("Models/Bat.obj");

	Martillo = Model();
	Martillo.LoadModel("Models/Martillo.obj");

	Medusa1 = Model();
	Medusa1.LoadModel("Models/Medusa1.obj");

	Pelota = Model();
	Pelota.LoadModel("Models/Pelota.obj");

	Coin = Model();
	Coin.LoadModel("Models/Coin.obj");








	//Modelos Los padrinos magicos

	varita = Model();
	varita.LoadModel("Models/varita.obj");

	CasaTimmy = Model();
	CasaTimmy.LoadModel("Models/casa-timmy.obj");

	Cosmo = Model();
	Cosmo.LoadModel("Models/cosmo.obj");

	Wanda = Model();
	Wanda.LoadModel("Models/wanda.obj");

	Timmy = Model();
	Timmy.LoadModel("Models/timmy.obj");








	//Skybox
	std::vector<std::string> skyboxFacesDay;
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFacesDay.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skyboxDay = Skybox(skyboxFacesDay);

	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");
	skyboxNight = Skybox(skyboxFacesNight);


	//



	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f, 0.0f); //Iluminaci�n del sol //Direcci�n inicial
	
	
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	inicializarMedusas();

	// Inicializa el estado del martillo
	anguloMartillo = 0.0f;
	martilloEnMovimiento = false;
	martilloSubiendo = false;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		actualizarAnimacionBateo(deltaTime);
		actualizarAnimaciones(deltaTime);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::vec3 camPos = camera.getCameraPosition();

		if (mainWindow.getLantern() == 1)
		{
			//linterna
			spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
				0.0f, 2.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				5.0f);
			spotLightCount++;//Encender lampara
		}
		else if (mainWindow.getLantern() == 0)
		{
			//linterna
			spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 2.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				5.0f);
			spotLightCount--; //Apagar lampara
		}



		// luz ligada a la c�mara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
		
		
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection()); //Luz se mueva respecto a la camara

		//informaci�n al shader de fuentes de iluminaci�n
		





		//Actualizar �ngulo del sol
		sunAngle += sunSpeed * deltaTime;
		if (sunAngle > 360.0f)
			sunAngle -= 360.0f;	
		float radians = glm::radians(sunAngle); //Convertir a radianes
		glm::vec3 direction = glm::vec3(0.0f, sin(radians), -cos(radians)); //Calcular direcci�n	
		pointLightCount = 0; //Reiniciar contador de luces puntuales

		//Cambio para que la transici�n sea m�s suave
		//Funci�n dinamica de actualizaci�n encendido de luces


		if (direction.y > 0.2f) {
			skyboxDay.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else if (direction.y <= 0.2f) {
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection);
			
			//Luces
			// Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, -48.0f,
					0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, -48.0f)) < 100.0f) {
				spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, -48.0f,
					-0.5f, -1.0f, 0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(-48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[3] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					-48.0f, 8.0f, 48.0f,
					0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			//Luz poste
			if (glm::distance(camPos, glm::vec3(48.0f, 0.0f, 48.0f)) < 100.0f) {
				spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount++;
			}
			else {
				spotLights[4] = SpotLight(0.0f, 0.0f, 0.0f,
					1.0f, 1.5f,
					48.0f, 8.0f, 48.0f,
					-0.5f, -1.0f, -0.5f,
					1.0f, 0.0f, 0.0f,
					50.0f);
				spotLightCount--;
			}

			
			//Luces de las varitas
			struct VaritaLight {
				glm::vec3 position;
			};

			std::vector<VaritaLight> varitas = {
				{{320.0f, 55.f, 0.0f}},
				{{300.0f, 55.f, 499.0f}},
				{{-320.0f, 55.f, 0.0f}},
				{{-300.0f, 55.f, 499.0f}},
				{{0.0f,   55.f, 320.0f}},
				{{300.0f, 55.f, -320.0f}},
				{{-300.0f,55.f, -320.0f}},
				{{0.0f,   55.f, -499.0f}}
			};

			for (int i = 0; i < varitas.size() && pointLightCount < MAX_POINT_LIGHTS; ++i) {
				if (glm::distance(camPos, varitas[i].position) < 100.0f) {
					pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
						2.0f, 3.0f,
						varitas[i].position.x, varitas[i].position.y, varitas[i].position.z,
						0.3f, 0.05f, 0.01f);
					pointLightCount++;
				}
			}





		}
	






		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		//Nueva implemantaci�n cambio de luz de d�a a noche con interpolaci�n

		mainLight.SetDirection(direction); //Actualizar direcci�n de la luz direccional

		float intensidadMin = 0.5f;
		float intensidadMax = 1.0f;

		// Clamp la direcci�n Y para que siempre est� entre 0 y 1
		float t = glm::clamp(direction.y, 0.0f, 1.0f);

		// Interpolaci�n de luz seg�n la posici�n del sol
		float ambient = glm::mix(intensidadMin, intensidadMax, t);
		float diffuse = glm::mix(intensidadMin, intensidadMax, t);

		mainLight.SetAmbientIntensity(ambient);
		mainLight.SetDiffuseIntensity(diffuse);

		//Esto ya estaba
		shaderList[0].SetDirectionalLight(&mainLight); //Mainlight para el sol
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Piso principal escenario
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 1.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Piso de cada isla
		
		//Isla Hora de aventura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoH.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Isla padrinos magicos		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoL.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Isla Bob Esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoB.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Kiosko 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoETexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -75.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 170.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 12.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 300.0f));
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, -0.8f, -95.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 19.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Caminos de conexion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-255.0f, -0.8f, -300.0f));
		model = glm::scale(model, glm::vec3(5.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		camino.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();





		//Colocar modelos hora de aventura
		//Para animaci�n backflip BMO
		static bool animacionIniciada = false;
		static bool animacionTerminada = false;
		static bool camaraEstabaLejos = true;
		static float tiempoInicioAnimacion = 0.0f;

		float tiempo = glfwGetTime();
		float anguloBackflip = 0.0f;
		float altura = 0.0f;


		if (glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) <= 30.0f && camaraEstabaLejos) {
			animacionIniciada = false;
			animacionTerminada = false;
			camaraEstabaLejos = false; // Ya no est� lejos
		}

		if (glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) > 50.0f) {
			camaraEstabaLejos = true;
		}

		if (!animacionTerminada && glm::distance(camPos, glm::vec3(-300.0f, 0.0f, 400.0f)) <= 50.0f) {
			if (!animacionIniciada) {
				animacionIniciada = true;
				tiempoInicioAnimacion = tiempo;
			}

			float tiempoAnimacion = tiempo - tiempoInicioAnimacion;

			if (tiempoAnimacion <= 2.0f) {
				anguloBackflip = glm::radians(-360.0f * (tiempoAnimacion / 2.0f));
				altura = sin(tiempoAnimacion * glm::pi<float>()) * 2.0f;
			}
			else {
				animacionTerminada = true;
				anguloBackflip = 0.0f;
				altura = 0.0f;
			}
		}


		//BMO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, altura, 400.0f));
		model = glm::rotate(model, 2.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, anguloBackflip, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotaci�n para el backflip
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BMO.RenderModel();

		//Casa del arbol
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -0.2f, 400.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaDelArbol.RenderModel();
		
		//Jake el perro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getPosX(), 0.5f, mainWindow.getPosZ()));
		model = glm::rotate(model, glm::radians(mainWindow.getDireccion()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));                  
		glm::mat4 modelJake = model;                                
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeCuerpo.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getBrazoDerAng()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getBrazoIzqAng()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoIzq.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
		model = glm::rotate(model, glm::radians(mainWindow.getPiernaDerAng()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
		model = glm::rotate(model, glm::radians(mainWindow.getPiernaIzqAng()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaIzq.RenderModel();

		//Prismo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, -1.4f, 470.0));
		model = glm::scale(model, glm::vec3(23.0f, 23.0f, 23.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Prismo.RenderModel();

		//Guitarra de Marceline
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 2.5f, 450.0));
		//model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(100.5f, 100.5f, 100.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MarcelinesGuitar.RenderModel();


		//AQU� DEBEN DE PONER LAS UBICACIONES DE DONDE VAN A QUERER LOS RECIBIDORES DE MONEDAS (SOLO EDITEN X y Z)
		//Recibidor de monedas
		RenderRecibidorMonedas(glm::vec3(-220.0f, -0.9f, 419.0f), 0.0f, uniformModel);


		
	












		//Colocar modelos Bob esponja
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 8.8f, -250.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobCuerpo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.9f, -1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.9f, -1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobBrazoIzq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, -2.5f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaDer.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.8f, -2.5f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BobPiernaIzq.RenderModel();

		//CASAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, 0.0f, -420.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaBob.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -450.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.8f, 2.8f, 2.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaCalam.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, -3.0f, -140.0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPatricio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(160.0f, -4.0f, -160.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CrustaceoCas.RenderModel();

		//TOPO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -210.0));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Topo.RenderModel();

		glm::vec3 posicionesMedusas[5] = {
			glm::vec3(1.3f, 0.0f, 0.4f),    // Medusa 1
			glm::vec3(-0.1f, 0.0f, 0.4f),   // Medusa 2
			glm::vec3(-1.4f, 0.0f, 0.4f),   // Medusa 3
			glm::vec3(-1.0f, 0.0f, -1.1f),  // Medusa 4
			glm::vec3(0.8f, 0.0f, -1.1f)    // Medusa 5
		};

		// Renderiza cada medusa con su animaci�n correspondiente
		for (int i = 0; i < 5; i++) {
			if (estadoMedusas[i].visible) {
				model = modelaux;
				// Aplica la altura animada a la posici�n Y
				model = glm::translate(model, glm::vec3(
					posicionesMedusas[i].x,
					estadoMedusas[i].altura,  // Altura animada
					posicionesMedusas[i].z
				));

				// Si fue golpeada, aplastar la medusa (escalar en Y)
				if (estadoMedusas[i].golpeada) {
					float factorAplastamiento = 1.0f - (estadoMedusas[i].tiempoGolpeada * 1.5f);
					if (factorAplastamiento < 0.2f) factorAplastamiento = 0.2f;

					model = glm::scale(model, glm::vec3(0.4f, 0.4f * factorAplastamiento, 0.4f));

					// Si est� golpeada, tambi�n le damos un efecto de ensanchamiento lateral
					model = glm::scale(model, glm::vec3(1.0f + (1.0f - factorAplastamiento), 1.0f, 1.0f + (1.0f - factorAplastamiento)));
				}
				else {
					model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
				}

				// Agrega un peque�o balanceo para m�s realismo cuando est� visible
				if (!estadoMedusas[i].golpeada && estadoMedusas[i].altura > estadoMedusas[i].alturaInicial + 1.0f) {
					float balanceo = sin(glfwGetTime() * 2.0f + i * 1.5f) * 0.1f;
					model = glm::rotate(model, balanceo, glm::vec3(0.0f, 0.0f, 1.0f));
				}

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Medusa1.RenderModel();
			}
		}

		//Jake el perro 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, 3.7f, -210.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		modelJake = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeCuerpo.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(1.0f, 0.0f, 0.0f));
		// Aqu� aplicamos la rotaci�n del martillo
		model = glm::rotate(model, glm::radians(anguloMartillo), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoDer.RenderModel();

		model = glm::translate(model, glm::vec3(-2.0f, -0.1f, 0.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Martillo.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(1.05f, 2.0f, 0.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoIzq.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaIzq.RenderModel();

		// Sistema de colisiones b�sico para detectar si golpeamos una medusa
		if (martilloEnMovimiento && !martilloSubiendo) {
			// Calculamos la posici�n del martillo basada en el �ngulo actual
			float anguloRad = glm::radians(anguloMartillo);

			// Posici�n aproximada del martillo en el mundo (calculada con la rotaci�n)
			glm::vec3 posMartillo = glm::vec3(
				-120.0f - 3.0f + sin(anguloRad) * 2.0f,  // Ajuste en X seg�n rotaci�n
				3.7f + 2.0f - cos(anguloRad) * 2.0f,     // Ajuste en Y seg�n rotaci�n
				-210.0f
			);

			// Radio de colisi�n del martillo
			float radioMartillo = 4.0f;

			// Verificar colisi�n con cada medusa visible
			for (int i = 0; i < 5; i++) {
				if (estadoMedusas[i].visible && !estadoMedusas[i].golpeada &&
					estadoMedusas[i].altura > estadoMedusas[i].alturaInicial + 1.0f) {

					// Posici�n de la medusa en el mundo
					glm::vec3 posMedusa = glm::vec3(
						-130.0f + posicionesMedusas[i].x * 2.3f,
						estadoMedusas[i].altura * 2.3f,
						-210.0f + posicionesMedusas[i].z * 2.3f
					);

					// Distancia entre el martillo y la medusa
					float distancia = glm::distance(posMartillo, posMedusa);

					// Si hay colisi�n, marcar la medusa como golpeada
					if (distancia < radioMartillo) {
						estadoMedusas[i].golpeada = true;
						estadoMedusas[i].tiempoGolpeada = 0.0f;
						// Aqu� podr�as agregar efectos de sonido o incrementar la puntuaci�n
					}
				}

				// Procesar medusas golpeadas (animaci�n de "aplastamiento")
				if (estadoMedusas[i].golpeada) {
					estadoMedusas[i].tiempoGolpeada += deltaTime;

					// Despu�s de un tiempo, hacer que desaparezca
					if (estadoMedusas[i].tiempoGolpeada > 0.3f) {
						estadoMedusas[i].altura -= estadoMedusas[i].velocidad * 0.5f * deltaTime;  // Desaparece m�s r�pido

						if (estadoMedusas[i].altura <= estadoMedusas[i].alturaInicial) {
							estadoMedusas[i].altura = estadoMedusas[i].alturaInicial;
							estadoMedusas[i].visible = false;
							estadoMedusas[i].golpeada = false;
						}
					}
				}
			}
		}

		//JAULA 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, -420.0));
		model = glm::scale(model, glm::vec3(2.3f, 2.3f, 2.3f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Jaula.RenderModel();

		if (pelotaEnMovimiento) {
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-23.0f + posicionPelota.x, 10.0f + posicionPelota.y, -421.5f + posicionPelota.z));
			model = glm::scale(model, glm::vec3(3.3f, 3.3f, 3.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Pelota.RenderModel();
		}
		else {
			// Si la pelota no est� en movimiento, mostrarla en la m�quina lanzadora
			model = modelaux;
			model = glm::translate(model, glm::vec3(-23.0f, -0.3f, -1.5));
			model = glm::scale(model, glm::vec3(3.3f, 3.3f, 3.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Pelota.RenderModel();
		}

		//Jake el perro 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(58.0f, 3.7f, -433.0f));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		modelJake = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeCuerpo.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.95f, 2.0f, 0.0f));
		// Aplicar rotaci�n para el golpe en el brazo izquierdo - cambiado a rotaci�n en Y para movimiento horizontal
		model = glm::rotate(model, glm::radians(anguloGolpe), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakeBrazoIzq.RenderModel();

		model = glm::translate(model, glm::vec3(1.5f, -0.2f, 0.0));
		model = glm::rotate(model, -1.57f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bat.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(-0.38f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaDer.RenderModel();

		model = modelJake;
		model = glm::translate(model, glm::vec3(0.51f, 0.2f, -0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JakePiernaIzq.RenderModel();

		//PIZZAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 13.0f, -340.0));
		model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.32f, 0.32f, 0.32f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pizzas.RenderModel();

		//CANGREBURGUERS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 9.4f, -120.0));
		model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cangre.RenderModel();


		//Bancas
		RenderBanca(glm::vec3(-48.0f, -1.0f, 25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, 25.0f), 1.6f, uniformModel);
		RenderBanca(glm::vec3(-48.0f, -1.0f, -25.0f), -1.6f, uniformModel);
		RenderBanca(glm::vec3(48.0f, -1.0f, -25.0f), 1.6f, uniformModel);



		//Mesas de comer
		RenderBancaCom(glm::vec3(30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, -40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(30.0f, -1.0f, 40.0f), 0.0f, uniformModel);
		RenderBancaCom(glm::vec3(-30.0f, -1.0f, 40.0f), 0.0f, uniformModel);



		//Lamparas
		//En kiosko
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, -49.0f), -0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, -49.0f), 3.5f, uniformModel);
		RenderLamparaBob(glm::vec3(-49.0f, -0.8f, 49.0f), 0.5f, uniformModel);
		RenderLamparaBob(glm::vec3(49.0f, -0.8f, 49.0f), 2.5f, uniformModel);
		


		//Coins
		static bool monedaAnimando = false;
		static float alturaMoneda = 3.0f;
		static float rotacionCoin = 0.0f;

		if (glm::distance(camPos, glm::vec3(-220.0f, 3.0f, 419.0f)) < 10.0f && mainWindow.getJuego() == 1) {
			
			if (!monedaAnimando && alturaMoneda == 3.0f) {
				monedaAnimando = true;
			}

			// Si se est� animando, baja y gira
			if (monedaAnimando) {
				alturaMoneda -= deltaTime * 0.01f; // Velocidad de ca�da
				rotacionCoin += deltaTime * glm::radians(360.0f) * 0.001f; // Giro

				if (alturaMoneda <= 0.0f) {
					alturaMoneda = 3.0f;
					rotacionCoin = 0.0f;
					monedaAnimando = false;
				}
			}

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-220.0f, alturaMoneda, 419.0f));
			model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1, 0.0f));
			model = glm::rotate(model, rotacionCoin, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Coin.RenderModel();

		}
		









		//Colocar modelos Los padrinos magicos

		//Varitas
		RenderVarita(glm::vec3(320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(300.0f, -0.8f, 499.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(-320.0f, -0.8f, 0.0f), 0.0f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, 499.0f), 1.5f, uniformModel);

		RenderVarita(glm::vec3(0.0f, -0.8f, 320.0f), 1.5f, uniformModel);


		RenderVarita(glm::vec3(300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(-300.0f, -0.8f, -320.0f), 1.5f, uniformModel);
		RenderVarita(glm::vec3(0.0f, -0.8f, -499.0f), 1.5f, uniformModel);

		//Casa Timmy
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, 2.0f, 300.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaTimmy.RenderModel();

		//Cosmo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, 10.2f, 240.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cosmo.RenderModel();
		
		//Wanda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(301.0f, 8.2f, 240.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Wanda.RenderModel();
		
		//Timmy
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(301.0f, 0.2f, 240.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Timmy.RenderModel();






		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
