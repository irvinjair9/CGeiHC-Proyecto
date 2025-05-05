#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	Lantern = 0.0f;
	Juego = 0.0f;

	muevex = 2.0f;

	// Inicialización de variables para Jake
	posX = 0.0f;
	posZ = 0.0f;
	direccion = 0.0f;
	brazoDerAng = 0.0f;
	brazoIzqAng = 0.0f;
	piernaDerAng = 0.0f;
	piernaIzqAng = 0.0f;
	enMovimiento = false;
	velocidadAnim = 3.0f;
	rangoAnim = 30.0f;  // 30 grados de oscilación para extremidades
	direccionAnim = true;  // true = hacia adelante, false = hacia atrás

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
	mainWindow = glfwCreateWindow(width, height, "Proyecto Feria CG", NULL, NULL);

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
	// Asignar valores de la ventana y coordenadas

	//Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
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


void Window::actualizarAnimacionJake()
{
	// Si Jake está en movimiento, actualizar la animación de las extremidades
	if (enMovimiento)
	{
		// Determinamos la dirección de la animación (oscilación)
		if (direccionAnim)
		{
			brazoDerAng += velocidadAnim;
			brazoIzqAng -= velocidadAnim;
			piernaDerAng -= velocidadAnim;
			piernaIzqAng += velocidadAnim;

			// Cambiar dirección cuando alcance el límite
			if (brazoDerAng >= rangoAnim)
				direccionAnim = false;
		}
		else
		{
			brazoDerAng -= velocidadAnim;
			brazoIzqAng += velocidadAnim;
			piernaDerAng += velocidadAnim;
			piernaIzqAng -= velocidadAnim;

			// Cambiar dirección cuando alcance el límite
			if (brazoDerAng <= -rangoAnim)
				direccionAnim = true;
		}
	}
	else
	{
		// Si no está en movimiento, regresar lentamente a la posición original
		if (brazoDerAng > 0.5f)
			brazoDerAng -= 1.0f;
		else if (brazoDerAng < -0.5f)
			brazoDerAng += 1.0f;
		else
			brazoDerAng = 0.0f;

		if (brazoIzqAng > 0.5f)
			brazoIzqAng -= 1.0f;
		else if (brazoIzqAng < -0.5f)
			brazoIzqAng += 1.0f;
		else
			brazoIzqAng = 0.0f;

		if (piernaDerAng > 0.5f)
			piernaDerAng -= 1.0f;
		else if (piernaDerAng < -0.5f)
			piernaDerAng += 1.0f;
		else
			piernaDerAng = 0.0f;

		if (piernaIzqAng > 0.5f)
			piernaIzqAng -= 1.0f;
		else if (piernaIzqAng < -0.5f)
			piernaIzqAng += 1.0f;
		else
			piernaIzqAng = 0.0f;
	}

	// Procesar las teclas de movimiento WASD
	GLfloat velocidad = 0.1f;
	enMovimiento = false;

	if (keys[GLFW_KEY_Z])
	{
		posZ -= velocidad;
		enMovimiento = true;
		direccion = 180.0f;
	}

	if (keys[GLFW_KEY_X])
	{
		posZ += velocidad;
		enMovimiento = true;
		direccion = 0.0f;
	}

	if (keys[GLFW_KEY_C])
	{
		posX -= velocidad;
		enMovimiento = true;
		direccion = 270.0f;
	}

	if (keys[GLFW_KEY_V])
	{
		posX += velocidad;
		enMovimiento = true;
		direccion = 90.0f;
	}

	// Combinaciones de teclas para movimiento diagonal
	if (keys[GLFW_KEY_Z] && keys[GLFW_KEY_C])
		direccion = 135.0f;
	if (keys[GLFW_KEY_Z] && keys[GLFW_KEY_V])
		direccion = 225.0f;
	if (keys[GLFW_KEY_X] && keys[GLFW_KEY_C])
		direccion = 45.0f;
	if (keys[GLFW_KEY_X] && keys[GLFW_KEY_V])
		direccion = 315.0f;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow->muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow->muevex -= 1.0;
	}

	//Para activar la linterna
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		theWindow->Lantern = 0;
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->Lantern = 1;
	}

	//Para activar el juego
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		theWindow->Juego = 0;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->Juego = 1;
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
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
