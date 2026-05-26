#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <sstream>

// GLAD: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator
// https://glad.dav1d.de/
#include <glad/glad.h>

// GLFW: https://www.glfw.org/
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

#include <irrKlang.h>
using namespace irrklang;

// Functions
bool Start();
bool Update();

// Definición de callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void UpdateCharacterDirection();
void UpdateCharacterCameras();

// Gobals
GLFWwindow* window;

// Tamaño en pixeles de la ventana
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Definición de cámara (posición en XYZ)
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
Camera camera3rd(glm::vec3(0.0f, 0.0f, 0.0f));
Camera roverCamera(glm::vec3(0.0f, 2.0f, 10.0f));

// Controladores para el movimiento del mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Variables para la velocidad de reproducción
// de la animación
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 position(0.0f,0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 3.0f;
float     scaleV = 0.06f;
float     rotateCharacter = 0.0f;
float	  rotateA = 0.0f;	
float	  doorScale = 0.22f;
float     doorTargetScale = 0.22f;
float     doorAnimSpeed = 1.2f;
bool      doorTogglePressed = false;
bool      viewTogglePressed = false;
float     firstPersonOffset = 0.6f;
bool      isCharacterMoving = false;
bool      lightsEnabled = true;
bool      lightsTogglePressed = false;
float     lightsBrightness = 1.0f;        // <-- NUEVO: brillo actual exterior
float     lightsTargetBrightness = 1.0f;  // <-- NUEVO: brillo objetivo exterior

bool      insideLightsEnabled = true;
bool      insideLightsTogglePressed = false;
float     insideLightsBrightness = 1.0f;        // <-- NUEVO: brillo actual interior
float     insideLightsTargetBrightness = 1.0f;  // <-- NUEVO: brillo objetivo interior

float     lightsFadeSpeed = 3.0f;  // <-- NUEVO: velocidad del fade (ajústala a tu gusto)
float     lampsBrightness = 2.5f;         // <-- NUEVO: brillo actual de lámparas externas
float     insideLampsBrightness = 3.5f;   // <-- NUEVO: brillo actual de lámparas internas

// Control de cámara de rovers
int activeRoverCamera = -1; // -1 = character view, 0 = rover1, 1 = rover2, 2 = rover3
bool roverCameraTogglePressed = false;

glm::vec3 rover1Pos(-1.0f, 0.0f, 5.0f);
glm::vec3 rover2Pos(-0.16f, 0.0f, 4.0f);
glm::vec3 rover3Pos(16.4f, 0.0f, 2.8f);

// Índices de huesos de cámara para cada rover
int rover1CamBoneIndex = 22; 
int rover2CamBoneIndex = 22;  
int rover3CamBoneIndex = 24; 

// Shaders
Shader *mLightsShader;
Shader *cubemapShader;
Shader *dynamicShader;
Shader *solarPanelShader;
Shader	*bumpShader;
Shader	*emissiveShader;

// Control de brillo de electrónicos
float electronicsBrightness = 3.5f;
bool fKeyPressed = false;
bool gKeyPressed = false;

// CICLO DÍA-NOCHE
float cycleDuration = 240.0f;  // Duración total del ciclo en segundos	
float cycleTime = 0.0f;        // Tiempo actual en el ciclo
float timeMultiplier = 1.0f;   // Multiplicador de velocidad del tiempo
bool timeAccelPressed = false;
bool showTimeInfo = true;      // Mostrar información del ciclo
float dayPhase;
float sunIntensity;

// --- VARIABLES DE INTERACCIÓN (COMIDA) ---
bool isFood1Visible = true;
bool isFood2Visible = true;
bool isFood3Visible = true;
bool isFood4Visible = true;

bool isEating = false;
int activeFood = 0; // 0 = Ninguna, 1 = Comida1, 2 = Comida2, etc.
float eatingTimer = 0.0f;


// Carga la información del modelo
Model   *objeto1;
Model   *doorFrame1;
Model   *doorFrame2;
Model   *doorFrame3;
Model   *doorFrame4;
Model   *doorFrame5;
Model   *doorFrame6;
Model   *doorDoor1;
Model   *doorDoor2;
Model   *doorDoor3;
Model   *doorDoor4;
Model   *doorDoor5;
Model   *doorDoor6;
Model	*conectores;
Model	*domos;
Model	*paneles;
Model	*terrenoLunar;
Model	*lamps;
Model	*Debug;
Model	*lucesinternas;
Model	*electronicos;
Model	*metalicos;
Model	*paneles_control;
Model	*cubiculo;
Model   *cartel_lab;
Model   *cartel_granja;
Model	*instruccion1;
Model	*instruccion2;
Model   *cartelBienvenida;
Model   *cartelMapa;
Model	*comida1;
Model	*comida2;
Model	*comida3;
Model	*comida4;

// Modelos animados
AnimatedModel   *character01;
AnimatedModel	*characterAnim01;
AnimatedModel	*characterAnim02;
AnimatedModel	*characterAnim03;
AnimatedModel	*characterAnim04;
AnimatedModel	*characterAnim05;
AnimatedModel	*characterAnim06;
AnimatedModel	*characterAnim07;
AnimatedModel	*characterAnim08;
AnimatedModel	*characterAnim09;

// Cubemap
CubeMap *mainCubeMap;

// Light gLight;
std::vector<Light> gLights;

// Materiales
Material metal;
Material metal2;
Material cristal;
Material frameMaterial;
Material panelMaterial;

// Audio
ISoundEngine *SoundEngine = createIrrKlangDevice();

// selección de cámara
bool    activeCamera = 1; // activamos la primera cámara

// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}

bool Start() {
	// Inicialización de GLFW

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creación de la ventana con GLFW
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Ocultar el cursor mientras se rota la escena
 glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: Cargar todos los apuntadores
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Activación de buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	// Compilación y enlace de shaders
	mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/Blinn-Phong.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	dynamicShader = new Shader("shaders/10_vertex_skinning-IT2.vs", "shaders/10_fragment_skinning-IT.fs");
	solarPanelShader = new Shader("shaders/12_SolarPanel.vs", "shaders/12_SolarPanel.fs");
	bumpShader = new Shader("shaders/11_BasicPhongShader-bump.vs", "shaders/11_BasicPhongShader-bump.fs");
	emissiveShader = new Shader("shaders/EmissiveShader.vs", "shaders/EmissiveShader.fs");

	// Máximo número de huesos: 100
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

 //Rutas
	//Escena
	objeto1 = new Model("../modelos-fbx/escena.fbx");

	//Marco de Puertas
	doorFrame1 = new Model("../modelos-fbx/Marco.fbx");
	doorFrame2 = new Model("../modelos-fbx/Marco.fbx");
	doorFrame3 = new Model("../modelos-fbx/Marco.fbx");
	doorFrame4 = new Model("../modelos-fbx/Marco.fbx");
	doorFrame5 = new Model("../modelos-fbx/Marco.fbx");
	doorFrame6 = new Model("../modelos-fbx/Marco.fbx");

	//Puertas
	doorDoor1 = new Model("../modelos-fbx/Puerta.fbx");
	doorDoor2 = new Model("../modelos-fbx/Puerta.fbx");
	doorDoor3 = new Model("../modelos-fbx/Puerta.fbx");
	doorDoor4 = new Model("../modelos-fbx/Puerta.fbx");
	doorDoor5 = new Model("../modelos-fbx/Puerta.fbx");
	doorDoor6 = new Model("../modelos-fbx/Puerta.fbx");

	//Conectores entre domos
	conectores = new Model("../modelos-fbx/conectores.fbx");

	//Domos
	domos = new Model("../modelos-fbx/cuatrodomos.fbx");

	//Paneles solares
	paneles = new Model("../modelos-fbx/paneles_solares.fbx");

	//Terreno
	terrenoLunar = new Model("../modelos-fbx/plano.obj");

	//Lamparas y luces
	lamps = new Model("../modelos-fbx/lamparas.fbx");
	lucesinternas = new Model("../modelos-fbx/luces_internas.fbx");
	
	//Electronicos
	electronicos = new Model("../modelos-fbx/electronicos.fbx");

	//Metalicos
	metalicos = new Model("../modelos-fbx/metalicos.fbx");

	//Paneles de control
	paneles_control = new Model("../modelos-fbx/paneles_control.fbx");

	//Cubiculo y carteles
	cubiculo = new Model("../modelos-fbx/cubiculo.fbx");
	cartel_lab = new Model("../modelos-fbx/s_lab.fbx");
	cartel_granja = new Model("../modelos-fbx/s_granja.fbx");
	instruccion1 = new Model("../modelos-fbx/instruccion-1.fbx");
	instruccion2 = new Model("../modelos-fbx/instruccion-2.fbx");
	cartelBienvenida = new Model("../modelos-fbx/cartel-bienvenida.fbx");
	cartelMapa = new Model("../modelos-fbx/cartel-mapa.fbx");

	//Comida
	comida1 = new Model("../modelos-fbx/comida1.fbx");
	comida2 = new Model("../modelos-fbx/comida2.fbx");
	comida3 = new Model("../modelos-fbx/comida3.fbx");
	comida4 = new Model("../modelos-fbx/comida4.fbx");

	//Objetos Animados
	character01 = new AnimatedModel("../modelos-fbx/Personaje.fbx");
	characterAnim01 = new AnimatedModel("../modelos-fbx/rovermov3.fbx");
	characterAnim02 = new AnimatedModel("../modelos-fbx/rovermov2.fbx");
	characterAnim03 = new AnimatedModel("../modelos-fbx/rovermov1.fbx");
	characterAnim04 = new AnimatedModel("../modelos-fbx/antena_parabolica.fbx");
	characterAnim05 = new AnimatedModel("../modelos-fbx/antena_parabolica.fbx");
	characterAnim06 = new AnimatedModel("../modelos-fbx/antena_parabolica.fbx");
	characterAnim07 = new AnimatedModel("../modelos-fbx/antena-a.fbx");
	characterAnim08 = new AnimatedModel("../modelos-fbx/antena-a.fbx");
	characterAnim09 = new AnimatedModel("../modelos-fbx/npc-computadora.fbx");

	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/01/bkg1_right.png",
		"textures/cubemap/01/bkg1_left.png",
		"textures/cubemap/01/bkg1_top.png",
		"textures/cubemap/01/bkg1_bot.png",
		"textures/cubemap/01/bkg1_front.png",
		"textures/cubemap/01/bkg1_back.png"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	UpdateCharacterCameras();

	// Luz del Sol
	
	Light light01;
	light01.Position = glm::vec3(50.0f, 50.0f, 50.0f);
	light01.Color = glm::vec4(0.4f, 0.25f, 0.1f, 1.0f);
	gLights.push_back(light01);
	
	Light light02;
    light02.Position = glm::vec3(-4.72f, 5.58f, -3.45f);
	light02.Color = glm::vec4(0.011f, 0.011f, 0.01f, 1.0f);
	gLights.push_back(light02);

	Light light03;
	light03.Position = glm::vec3(12.9f, 5.58f, -16.74f);
	light03.Color = glm::vec4(0.011f, 0.011f, 0.01f, 1.0f);
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(12.9f, 5.58f, 1.63f);
	light04.Color = glm::vec4(0.011f, 0.01f, 0.01f, 1.0f);
	gLights.push_back(light04);

	//Luces interiores 
	
	Light light05;
	light05.Position = glm::vec3(10.65f, 6.72f, -7.26f);
	light05.Color = glm::vec4(0.08f, 0.06f, 0.04f, 1.0f);
	light05.alphaIndex = 100; 
	light05.Power = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	gLights.push_back(light05);

	Light light06;
	light06.Position = glm::vec3(10.62f, 6.72f, -10.6f);
	light06.Color = glm::vec4(0.08f, 0.06f, 0.04f, 1.0f);
	light06.Power = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	light06.alphaIndex = 100;
	gLights.push_back(light06);

	Light light07;
	light07.Position = glm::vec3(10.65f, 6.72f, 9.35f);
	light07.Color = glm::vec4(0.08f, 0.06f, 0.04f, 1.0f);
	light07.Power = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	light07.alphaIndex = 100;
	gLights.push_back(light07);

	Light light08;
	light08.Position = glm::vec3(-12.33f, 6.72f, -7.95f);
	// Esta luz la dejé un poquitito más fuerte por si es la central
	light08.Color = glm::vec4(0.12f, 0.09f, 0.06f, 1.0f);
	light08.Power = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	light08.alphaIndex = 100;
	gLights.push_back(light08);
	
	// SoundEngine->play2D("sound/-.mp3", true);

	metal.ambient = glm::vec4(0.25f, 0.25f, 0.2f, 1.0f);
	metal.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	metal.specular = glm::vec4(0.775f, 0.775f, 0.775f, 1.0f);
	metal.transparency = 1.0f;

	metal2.ambient = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f);
	metal2.diffuse = glm::vec4(0.2f, 0.2f, 0.22f, 1.0f);
	metal2.specular = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f);
	metal2.transparency = 1.0f;

	cristal.ambient = glm::vec4(0.25f, 0.25f, 0.2f, 1.0f);
	cristal.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	cristal.specular = glm::vec4(0.775f, 0.775f, 0.775f, 1.0f);
	cristal.transparency = 0.6f;
	return true;
}


void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec3(uniformName.c_str(), value);
}

void UpdateCharacterDirection() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	forwardView = glm::normalize(glm::vec3(viewVector));
}

// ============ FUNCIÓN DE CICLO DÍA-NOCHE ============
void UpdateDayNightCycle() {

	// Actualizamos el tiempo del ciclo
	cycleTime += deltaTime * timeMultiplier;
	if (cycleTime >= cycleDuration) {
		cycleTime = 0.0f; // Reinicia el ciclo
	}

	// Calculamos la posición normalizada en el ciclo (0.0 a 1.0)
	float cycleProgress = cycleTime / cycleDuration;

	// Fórmula de seno para suavizar la transición: 
	// 0 = amanecer, 0.25 = mediodía, 0.5 = atardecer, 0.75 = medianoche
	dayPhase = glm::sin(cycleProgress * 2.0f * glm::pi<float>());

	// Calculamos el ángulo de rotación del sol (de 0 a 360 grados alrededor del eje Y)
	float sunAngle = cycleProgress * 360.0f;

	// Posición del sol: rota alrededor de la escena a una altura fija
	float sunRadius = 50.0f;
	float sunHeight = 50.0f + glm::sin(cycleProgress * glm::pi<float>()) * 30.0f;

	gLights[0].Position = glm::vec3(
		glm::cos(glm::radians(sunAngle)) * sunRadius,
		sunHeight,
		glm::sin(glm::radians(sunAngle)) * sunRadius
	);

	// Intensidad del sol (0 en medianoche, 1 en mediodía)
	sunIntensity = glm::max(0.0f, dayPhase);

	// Color del sol cambia según la hora:
	// Amanecer/Atardecer = naranja-rojo, Mediodía = amarillo-blanco
	glm::vec3 sunColor;
	if (cycleProgress < 0.25f) {
		// Amanecer: oscuro -> naranja
		float t = cycleProgress / 0.25f;
		sunColor = glm::mix(glm::vec3(0.1f, 0.05f, 0.0f), glm::vec3(1.0f, 0.6f, 0.2f), t);
	}
	else if (cycleProgress < 0.5f) {
		// Mañana-Mediodía: naranja -> amarillo-blanco
		float t = (cycleProgress - 0.25f) / 0.25f;
		sunColor = glm::mix(glm::vec3(1.0f, 0.6f, 0.2f), glm::vec3(1.0f, 0.95f, 0.8f), t);
	}
	else if (cycleProgress < 0.75f) {
		// Tarde-Atardecer: amarillo -> rojo-naranja
		float t = (cycleProgress - 0.5f) / 0.25f;
		sunColor = glm::mix(glm::vec3(1.0f, 0.95f, 0.8f), glm::vec3(1.0f, 0.3f, 0.1f), t);
	}
	else {
		// Noche: rojo -> oscuro
		float t = (cycleProgress - 0.75f) / 0.25f;
		sunColor = glm::mix(glm::vec3(1.0f, 0.3f, 0.1f), glm::vec3(0.1f, 0.05f, 0.0f), t);
	}

	// Aplicamos la intensidad al color
	gLights[0].Color = glm::vec4(sunColor * sunIntensity, 1.0f);

	// Ajustamos la potencia de la luz
	float lightPower = sunIntensity * 50.0f;
	gLights[0].Power = glm::vec4(lightPower, lightPower, lightPower, 1.0f);

	// Luz ambiental global: más clara de día, más oscura de noche
}

glm::mat4 GetRoverBoneTransform(AnimatedModel* rover, glm::vec3 basePos, glm::vec3 scale, int boneIndex)
{
	// Verificamos que el rover y el hueso existan
	if (!rover || boneIndex < 0 || boneIndex >= rover->bones.size()) {
		glm::mat4 fallback = glm::mat4(1.0f);
		return glm::translate(fallback, basePos);
	}

	// 1. Obtenemos las matrices que componen el Skinning
	glm::mat4 skinningMatrix = rover->gBones[boneIndex];
	glm::mat4 offsetMatrix = rover->bones[boneIndex].offsetMatrix;
	glm::mat4 globalInverse = rover->m_GlobalInverseTransform;

	// 2. MAGIA MATEMÁTICA: Despejamos la transformación real del hueso
	glm::mat4 realBoneTransform = glm::inverse(globalInverse) * skinningMatrix * glm::inverse(offsetMatrix);

	// 3. Aplicamos la posición base y la escala de tu escena
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, basePos);
	model = glm::scale(model, scale);

	// Devolvemos TODA la matriz (Posición + Rotación + Escala)
	return model * realBoneTransform;
}

void UpdateCharacterCameras() {
 camera.Position = position + forwardView * firstPersonOffset;
	camera.Position.y += 1.7f;

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = camera.Front;

	// Actualizar cámara de rover si está activa
	if (activeRoverCamera >= 0) {
		glm::vec3 roverPos;
		glm::vec3 roverScale;
		AnimatedModel* roverModel = nullptr;
		int camBoneIndex = 0;
		float heightOffset = 0.5f;

		if (activeRoverCamera == 0) {
			roverPos = rover1Pos;
			roverScale = glm::vec3(0.0022f, 0.0022f, 0.0022f);
			roverModel = characterAnim01;
			camBoneIndex = rover1CamBoneIndex;
			heightOffset = 0.2f;
		}
		else if (activeRoverCamera == 1) {
			roverPos = rover2Pos;
			roverScale = glm::vec3(0.003f, 0.003f, 0.002f);
			roverModel = characterAnim02;
			camBoneIndex = rover2CamBoneIndex;
			heightOffset = 0.0f;
		}
		else if (activeRoverCamera == 2) {
			roverPos = rover3Pos;
			roverScale = glm::vec3(0.002f, 0.002f, 0.002f);
			roverModel = characterAnim03;
			camBoneIndex = rover3CamBoneIndex;
			heightOffset = 0.4f;
		}

		if (roverModel) {
			// Obtenemos toda la matriz del hueso
			glm::mat4 boneTransform = GetRoverBoneTransform(roverModel, roverPos, roverScale, camBoneIndex);

			// Extraemos la posición (Columna 3 de la matriz)
			glm::vec3 currentRoverPos = glm::vec3(boneTransform[3]);

			// Extraemos la dirección "Frente" (Suele ser la columna 2, el eje Z del hueso)
			glm::vec3 boneForward = glm::normalize(glm::vec3(-boneTransform[2]));

			currentRoverPos += boneForward * 0.2f;

			roverCamera.Position = currentRoverPos;
			roverCamera.Position.y += heightOffset;

			// Le asignamos la rotación del hueso a la cámara
			roverCamera.Front = boneForward;
		}
	}
}


bool Update() {
	// Cálculo del framerate
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Procesa la entrada del teclado o mouse
	processInput(window);

	// ACTUALIZAR CICLO DÍA-NOCHE
	UpdateDayNightCycle();

	UpdateCharacterCameras();

	if (doorScale < doorTargetScale) {
		doorScale += doorAnimSpeed * deltaTime;
		if (doorScale > doorTargetScale)
			doorScale = doorTargetScale;
	}
	else if (doorScale > doorTargetScale) {
		doorScale -= doorAnimSpeed * deltaTime;
		if (doorScale < doorTargetScale)
			doorScale = doorTargetScale;
	}

	// Renderizado R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;

	if (activeRoverCamera >= 0) {
		// Usar cámara de rover
		projection = glm::perspective(glm::radians(roverCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = roverCamera.GetViewMatrix();
	}
	else if (activeCamera) {
		// Cámara en primera persona
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera.GetViewMatrix();
	}
	else {
		// cámara en tercera persona
		projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera3rd.GetViewMatrix();
	}

	// Cubemap (fondo)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}
	
	 {
		mLightsShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		mLightsShader->setMat4("projection", projection);
		mLightsShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		mLightsShader->setMat4("model", model);

		// Animación de fade para luces exteriores (L)
		lightsTargetBrightness = lightsEnabled ? 1.0f : 0.0f;
		lightsBrightness += (lightsTargetBrightness - lightsBrightness) * lightsFadeSpeed * deltaTime;

		// Animación de fade para luces interiores (K)
		insideLightsTargetBrightness = insideLightsEnabled ? 1.0f : 0.0f;
		insideLightsBrightness += (insideLightsTargetBrightness - insideLightsBrightness) * lightsFadeSpeed * deltaTime;

		mLightsShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			glm::vec4 lightColor = gLights[i].Color;
			if (i >= 1 && i <= 3)
				lightColor *= lightsBrightness;        // fade exterior
			if (i >= 4)
				lightColor *= insideLightsBrightness;  // fade interior
			SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(mLightsShader, "Color", i, lightColor);
			SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
		}
		
		mLightsShader->setVec3("eye", camera.Position);

		// Aplicamos propiedades materiales
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);

        //Agregar escena
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		objeto1->Draw(*mLightsShader);

		// Actualizamos el temporizador si estamos comiendo
		if (isEating) {
			eatingTimer += deltaTime;
			if (eatingTimer > 1.5f) {
				isEating = false;
				// Desaparecemos definitivamente la comida que nos acabamos de comer
				if (activeFood == 1) isFood1Visible = false;
				if (activeFood == 2) isFood2Visible = false;
				if (activeFood == 3) isFood3Visible = false;
				if (activeFood == 4) isFood4Visible = false;
				activeFood = 0;
			}
		}

		// ANIMACIÓN (ESTILO MINECRAFT) ---
		glm::vec3 cameraRight = glm::normalize(glm::cross(camera.Front, camera.Up));
		glm::vec3 posFrente = camera.Position + (camera.Front * 0.5f) - (camera.Up * 0.2f);
		posFrente.y += glm::sin(eatingTimer * 25.0f) * 0.03f; // Efecto de mordida rápida
		float anguloCamara = std::atan2(camera.Front.x, camera.Front.z);

		// DIBUJAR COMIDA 1 
		if (isFood1Visible) {
			glm::mat4 modelComida1 = glm::mat4(1.0f);
			if (isEating && activeFood == 1) {
				modelComida1 = glm::translate(modelComida1, posFrente);
				modelComida1 = glm::rotate(modelComida1, anguloCamara + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				modelComida1 = glm::rotate(modelComida1, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida1 = glm::rotate(modelComida1, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida1 = glm::rotate(modelComida1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida1 = glm::scale(modelComida1, glm::vec3(0.2f));
			}
			else {
				modelComida1 = glm::translate(modelComida1, glm::vec3(-14.035f, 0.9448f, -8.0994f));
				modelComida1 = glm::rotate(modelComida1, glm::radians(-89.45f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida1 = glm::rotate(modelComida1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida1 = glm::scale(modelComida1, glm::vec3(0.3f));
			}
			mLightsShader->setMat4("model", modelComida1);
			comida1->Draw(*mLightsShader);
		}

		// DIBUJAR COMIDA 2
		if (isFood2Visible) {
			glm::mat4 modelComida2 = glm::mat4(1.0f);
			if (isEating && activeFood == 2) {
				modelComida2 = glm::translate(modelComida2, posFrente);
				modelComida2 = glm::rotate(modelComida2, anguloCamara + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				modelComida2 = glm::rotate(modelComida2, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida2 = glm::rotate(modelComida2, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida2 = glm::rotate(modelComida2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida2 = glm::scale(modelComida2, glm::vec3(0.2f));
			}
			else {
				modelComida2 = glm::translate(modelComida2, glm::vec3(-14.01f, 0.9523f, -9.192f));
				modelComida2 = glm::rotate(modelComida2, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida2 = glm::rotate(modelComida2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida2 = glm::scale(modelComida2, glm::vec3(0.3f));
			}
			mLightsShader->setMat4("model", modelComida2);
			comida2->Draw(*mLightsShader);
		}

		// DIBUJAR COMIDA 3
		if (isFood3Visible) {
			glm::mat4 modelComida3 = glm::mat4(1.0f);
			if (isEating && activeFood == 3) {
				modelComida3 = glm::translate(modelComida3, posFrente);
				modelComida3 = glm::rotate(modelComida3, anguloCamara + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				modelComida3 = glm::rotate(modelComida3, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida3 = glm::rotate(modelComida3, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida3 = glm::rotate(modelComida3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida3 = glm::scale(modelComida3, glm::vec3(0.2f));
			}
			else {
				modelComida3 = glm::translate(modelComida3, glm::vec3(-14.773f, 0.985f, -7.095f));
				modelComida3 = glm::rotate(modelComida3, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida3 = glm::rotate(modelComida3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida3 = glm::scale(modelComida3, glm::vec3(0.3f));
			}
			mLightsShader->setMat4("model", modelComida3);
			comida3->Draw(*mLightsShader);
		}

		// DIBUJAR COMIDA 4
		if (isFood4Visible) {
			glm::mat4 modelComida4 = glm::mat4(1.0f);
			if (isEating && activeFood == 4) {
				modelComida4 = glm::translate(modelComida4, posFrente);
				modelComida4 = glm::rotate(modelComida4, anguloCamara + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				modelComida4 = glm::rotate(modelComida4, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida4 = glm::rotate(modelComida4, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida4 = glm::rotate(modelComida4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida4 = glm::scale(modelComida4, glm::vec3(0.2f));
			}
			else {
				modelComida4 = glm::translate(modelComida4, glm::vec3(-14.92f, 0.935f, -8.05f));
				modelComida4 = glm::rotate(modelComida4, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				modelComida4 = glm::rotate(modelComida4, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelComida4 = glm::scale(modelComida4, glm::vec3(0.3f));
			}
			mLightsShader->setMat4("model", modelComida4);
			comida4->Draw(*mLightsShader);
		}

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, -1.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		mLightsShader->setMat4("model", model);
		cartelBienvenida->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.0f, -10.0f, -0.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		mLightsShader->setMat4("model", model);
		cartelMapa->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.41f, 5.22f, 1.2f));
		model = glm::scale(model, glm::vec3(0.9f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		instruccion1->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f, 10.85f, 1.2f));
		model = glm::scale(model, glm::vec3(0.9f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		instruccion2->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::translate(model, glm::vec3(36.7f, 43.5f, 11.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		cartel_lab->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		model = glm::translate(model, glm::vec3(37.5f, 6.0f, 11.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		cartel_granja->Draw(*mLightsShader);

        glm::mat4 cubiculoModel = glm::mat4(1.0f);
		cubiculoModel = glm::rotate(cubiculoModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cubiculoModel = glm::rotate(cubiculoModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		cubiculoModel = glm::scale(cubiculoModel, glm::vec3(0.25f));
		cubiculoModel = glm::translate(cubiculoModel, glm::vec3(-10.0f, 45.0f, 0.0f));
		mLightsShader->setMat4("model", cubiculoModel);
		cubiculo->Draw(*mLightsShader);

		glm::mat4 cubiculoFrameModel = cubiculoModel;
		cubiculoFrameModel = glm::translate(cubiculoFrameModel, glm::vec3(0.0f, -4.0f, 0.0f));
		cubiculoFrameModel = glm::scale(cubiculoFrameModel, glm::vec3(1.12f, 1.0f, 0.88f));
		mLightsShader->setMat4("model", cubiculoFrameModel);
		doorFrame1->Draw(*mLightsShader);

		glm::mat4 cubiculoDoorModel = cubiculoModel;
		cubiculoDoorModel = glm::translate(cubiculoDoorModel, glm::vec3(4.0f, -4.0f, 0.0f));
		cubiculoDoorModel = glm::scale(cubiculoDoorModel, glm::vec3(1.2f, 1.0f, doorScale / 0.25f));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", cubiculoDoorModel);
		doorDoor1->Draw(*mLightsShader);

		glm::mat4 frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(4.1f, 0.0f, -8.48f));
		frameModel = glm::rotate(frameModel, glm::radians(-98.3f), glm::vec3(0.0f, 1.0f, 0.0f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f,0.25f,0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame1->Draw(*mLightsShader);

		glm::mat4 doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(4.0f, 0.0f, -7.4f));
		doorModel = glm::rotate(doorModel, glm::radians(-98.3f), glm::vec3(0.0f, 1.0f, 0.0f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.3f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor1->Draw(*mLightsShader);

		frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(-4.84f, 0.0f, -8.0f));
		frameModel = glm::rotate(frameModel, glm::radians(-88.72f), glm::vec3(0.0f, 1.0f, 0.0f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f, 0.25f, 0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame2->Draw(*mLightsShader);

        doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(-4.8f, 0.0f, -7.0f));
		doorModel = glm::rotate(doorModel, glm::radians(-88.72f), glm::vec3(0.0f, 1.0f, 0.0f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.3f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor2->Draw(*mLightsShader);

		frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(10.8f, 0.0f, 1.84f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f, 0.25f, 0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame3->Draw(*mLightsShader);

		doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(11.8f, 0.0f, 1.84f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.28f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor3->Draw(*mLightsShader);

		frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(10.8f, 0.0f, -16.35f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f, 0.25f, 0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame4->Draw(*mLightsShader);

		doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(11.75f, 0.0f, -16.35f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.3f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor4->Draw(*mLightsShader);

		frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(11.06f, 0.0f, -14.1f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f, 0.25f, 0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame5->Draw(*mLightsShader);

		doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(12.17f, 0.0f, -14.1f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.3f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor5->Draw(*mLightsShader);

		frameModel = glm::mat4(1.0f);
		frameModel = glm::translate(frameModel, glm::vec3(11.47f, 0.0f, -0.94f));
		frameModel = glm::rotate(frameModel, glm::radians(4.9f), glm::vec3(0.0f, 1.0f, 0.0f));
		frameModel = glm::rotate(frameModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		frameModel = glm::scale(frameModel, glm::vec3(0.28f, 0.25f, 0.22f));
		mLightsShader->setMat4("model", frameModel);
		doorFrame6->Draw(*mLightsShader);

		doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(doorModel, glm::vec3(12.58f, 0.0f, -1.03f));
		doorModel = glm::rotate(doorModel, glm::radians(4.9f), glm::vec3(0.0f, 1.0f, 0.0f));
		doorModel = glm::rotate(doorModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		doorModel = glm::scale(doorModel, glm::vec3(0.3f, 0.25f, doorScale));
		mLightsShader->setVec4("MaterialAmbientColor", metal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal.specular);
		mLightsShader->setFloat("transparency", metal.transparency);
		mLightsShader->setMat4("model", doorModel);
		doorDoor6->Draw(*mLightsShader);

		glm::mat4 metalesModel = glm::mat4(1.0f);
		metalesModel = glm::translate(metalesModel, glm::vec3(0.0f, 0.0f, 0.0f));
		metalesModel = glm::rotate(metalesModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		metalesModel = glm::scale(metalesModel, glm::vec3(0.3f));
		mLightsShader->setVec4("MaterialAmbientColor", metal2.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal2.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal2.specular);
		mLightsShader->setFloat("transparency", metal2.transparency);
		mLightsShader->setMat4("model", metalesModel);
		metalicos->Draw(*mLightsShader);

		glm::mat4 panelesControl = glm::mat4(1.0f);
		panelesControl = glm::translate(panelesControl, glm::vec3(0.0f, 0.0f, 0.0f));
		panelesControl = glm::rotate(panelesControl, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		panelesControl = glm::scale(panelesControl, glm::vec3(0.3f));
		mLightsShader->setVec4("MaterialAmbientColor", metal2.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", metal2.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", metal2.specular);
		mLightsShader->setFloat("transparency", metal2.transparency);
		mLightsShader->setMat4("model", panelesControl);
		paneles_control->Draw(*mLightsShader);

		glm::mat4 conectoresModel = glm::mat4(1.0f);
		conectoresModel = glm::translate(conectoresModel, glm::vec3(0.0f, 0.0f, 0.0f));
		conectoresModel = glm::rotate(conectoresModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		conectoresModel = glm::scale(conectoresModel, glm::vec3(0.3f));
		mLightsShader->setVec4("MaterialAmbientColor", cristal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", cristal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", cristal.specular);
		mLightsShader->setFloat("transparency", cristal.transparency);

		mLightsShader->setMat4("model", conectoresModel);
		conectores->Draw(*mLightsShader);

		glm::mat4 domosModel = glm::mat4(1.0f);
		domosModel = glm::translate(domosModel, glm::vec3(0.0f, 0.0f, 0.0f));
		domosModel = glm::rotate(domosModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		domosModel = glm::scale(domosModel, glm::vec3(0.3f));
		mLightsShader->setVec4("MaterialAmbientColor", cristal.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", cristal.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", cristal.specular);
		mLightsShader->setFloat("transparency", cristal.transparency);
		mLightsShader->setMat4("model", domosModel);
		domos->Draw(*mLightsShader);
		glDisable(GL_BLEND);
	 }

	 {
		solarPanelShader->use();

		// Vincular cubemap en slot 10
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mainCubeMap->textureID);
		solarPanelShader->setInt("cubetex", 10);

		// Uniformes básicos
		solarPanelShader->setMat4("projection", projection);
		solarPanelShader->setMat4("view", view);

		solarPanelShader->setVec3("cameraPosition", activeCamera ? camera.Position : camera3rd.Position);

		// Conectamos la posición y color del panel al sol dinámico
		solarPanelShader->setVec3("lightPos", gLights[0].Position);
		solarPanelShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Transformación del modelo
		glm::mat4 panelesModel = glm::mat4(1.0f);
		panelesModel = glm::translate(panelesModel, glm::vec3(0.0f, 0.0f, 0.0f));
		panelesModel = glm::rotate(panelesModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		panelesModel = glm::scale(panelesModel, glm::vec3(0.3f, 0.3f, 0.3f));

		// Variables de Fresnel
		solarPanelShader->setFloat("_Bias", 0.25f);  
		solarPanelShader->setFloat("_Scale", 0.95f);
		solarPanelShader->setFloat("_Power", 3.0f);

		solarPanelShader->setMat4("model", panelesModel);

		paneles->Draw(*solarPanelShader);
	}
	{
		bumpShader->use();

		// Matrices de Cámara
		bumpShader->setMat4("projection", projection);
		bumpShader->setMat4("view", view);
		bumpShader->setVec3("eye", camera.Position);

		// Variables de tu Luz Principal (gLights[0])
		bumpShader->setVec3("lightPosition", gLights[0].Position);
		bumpShader->setVec3("lightDirection", gLights[0].Direction);

		// Propiedades de la luz 
		bumpShader->setVec4("LightColor", gLights[0].Color);
		bumpShader->setVec4("LightPower", gLights[0].Power);
		bumpShader->setFloat("distance", gLights[0].distance);
		bumpShader->setInt("alphaIndex", gLights[0].alphaIndex);

		bumpShader->setVec4("MaterialAmbientColor", metal.ambient); 
		bumpShader->setVec4("MaterialDiffuseColor", metal.diffuse); 
		bumpShader->setVec4("MaterialSpecularColor", metal.specular); 
		bumpShader->setFloat("transparency", metal.transparency); 

		// Transformación y Dibujado
		glm::mat4 modeloRelieve = glm::mat4(1.0f);
		modeloRelieve = glm::translate(modeloRelieve, glm::vec3(0.0f, 0.0f, 0.0f));
		modeloRelieve = glm::rotate(modeloRelieve, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modeloRelieve = glm::scale(modeloRelieve, glm::vec3(0.3f, 0.3f, 0.3f));

		bumpShader->setMat4("model", modeloRelieve);

		terrenoLunar->Draw(*bumpShader);

	}
	{
		emissiveShader->use();

		emissiveShader->setMat4("projection", projection);
		emissiveShader->setMat4("view", view);

		glm::mat4 modeloLamps = glm::mat4(1.0f);
		modeloLamps = glm::translate(modeloLamps, glm::vec3(0.0f, 0.0f, 0.0f));
		modeloLamps = glm::rotate(modeloLamps, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modeloLamps = glm::scale(modeloLamps, glm::vec3(0.3f, 0.3f, 0.3f));

		emissiveShader->setMat4("model", modeloLamps);
		emissiveShader->setVec3("glowColor", glm::vec3(1.0f, 0.9f, 0.7f));

		// Fade suave de brillo para lámparas externas (sincronizado con L)
		float targetLamps = lightsEnabled ? 2.5f : 0.5f * sunIntensity;
		lampsBrightness += (targetLamps - lampsBrightness) * lightsFadeSpeed * deltaTime;

		emissiveShader->setFloat("intensity", lampsBrightness);
		lamps->Draw(*emissiveShader);

		glm::mat4 modeloLucesInternas = glm::mat4(1.0f);
		modeloLucesInternas = glm::translate(modeloLucesInternas, glm::vec3(0.0f, 0.0f, 0.0f));
		modeloLucesInternas = glm::rotate(modeloLucesInternas, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modeloLucesInternas = glm::scale(modeloLucesInternas, glm::vec3(0.3f, 0.3f, 0.3f));

		emissiveShader->setMat4("model", modeloLucesInternas);
		emissiveShader->setVec3("glowColor", glm::vec3(1.0f, 0.9f, 0.7f));

		// Fade suave de brillo para lámparas internas (sincronizado con K)
		float targetInternas = insideLightsEnabled ? 3.5f : 0.5f * sunIntensity;
		insideLampsBrightness += (targetInternas - insideLampsBrightness) * lightsFadeSpeed * deltaTime;

		emissiveShader->setFloat("intensity", insideLampsBrightness);
		lucesinternas->Draw(*emissiveShader);

		glm::mat4 modeloElectronicos = glm::mat4(1.0f);
		modeloElectronicos = glm::translate(modeloElectronicos, glm::vec3(0.0f, 0.0f, 0.0f));
		modeloElectronicos = glm::rotate(modeloElectronicos, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modeloElectronicos = glm::scale(modeloElectronicos, glm::vec3(0.3f, 0.3f, 0.3f));

		emissiveShader->setMat4("model", modeloElectronicos);

		// Configuramos el brillo controlable
		emissiveShader->setVec3("glowColor", glm::vec3(0.32f, 0.35f, 0.32f));
		emissiveShader->setFloat("intensity", electronicsBrightness); // Usa la variable global

		electronicos->Draw(*emissiveShader);
	}

	glUseProgram(0);


    // Personaje
	{
		if (isCharacterMoving)
			character01->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		// Mandamos el sol dinámico al shader de todos los modelos animados
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));


		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);

		// Dibujamos el modelo
		character01->Draw(*dynamicShader);
	}

	{
		characterAnim01->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		// Mandamos el sol dinámico al shader de todos los modelos animados
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 5.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0022f, 0.0022f, 0.0022f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim01->gBones);

		// Dibujamos el modelo
		characterAnim01->Draw(*dynamicShader);

	}

	{
		characterAnim02->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.16f, 0.0f, 4.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
       model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.002f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim02->gBones);

		// Dibujamos el modelo
		characterAnim02->Draw(*dynamicShader);
	}

	{
		characterAnim03->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(14.4f, 0.0f,2.8f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim03->gBones);

		// Dibujamos el modelo
		characterAnim03->Draw(*dynamicShader);
	}
	{
		characterAnim04->UpdateAnimation(deltaTime);

		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Activación del shader del personaje
		dynamicShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(11.5f, 7.5f, 9.84f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
       model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim04->gBones);

		// Dibujamos el modelo
		characterAnim04->Draw(*dynamicShader);
	}
	{
		characterAnim05->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(11.5f, 7.5f, -25.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
       model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim05->gBones);

		// Dibujamos el modelo
		characterAnim05->Draw(*dynamicShader);
	}
	{
		characterAnim06->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-11.34f, 7.5f, -8.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
       model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim06->gBones);

		// Dibujamos el modelo
		characterAnim06->Draw(*dynamicShader);
	}
	{
		characterAnim07->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.33f, 0.0f, -5.5f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim07->gBones);

		// Dibujamos el modelo
		characterAnim07->Draw(*dynamicShader);
	}
	
	{
		characterAnim08->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));


		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(11.0f, 8.0f, -8.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim08->gBones);

		// Dibujamos el modelo
		characterAnim08->Draw(*dynamicShader);
	}

	{
		characterAnim09->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();
		dynamicShader->setVec3("lightPos", gLights[0].Position);
		dynamicShader->setVec3("lightColor", glm::vec3(gLights[0].Color));

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(11.1, 0.5, -5.4)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.0123f, 0.0123f, 0.0123f));

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, characterAnim09->gBones);

		// Dibujamos el modelo
		characterAnim09->Draw(*dynamicShader);
	}
	

	glUseProgram(0); 

	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	isCharacterMoving = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position = position + scaleV * forwardView;
		isCharacterMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position = position - scaleV * forwardView;
		isCharacterMoving = true;
	}
	glm::vec3 rightView = glm::normalize(glm::cross(forwardView, glm::vec3(0.0f, 1.0f, 0.0f)));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position = position - scaleV * rightView;
		isCharacterMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position = position + scaleV * rightView;
		isCharacterMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// Character movement (teclas alternas)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position = position + scaleV * forwardView;
		isCharacterMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position = position - scaleV * forwardView;
		isCharacterMoving = true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!doorTogglePressed) {
			doorTargetScale = (doorTargetScale > 0.0f) ? 0.0f : 0.22f;
		}
		doorTogglePressed = true;
	}
	else {
		doorTogglePressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		rotateA += 0.5f;
		UpdateCharacterDirection();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rotateA -= 0.5f;
		UpdateCharacterDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		if (!viewTogglePressed)
			activeCamera = !activeCamera;
		viewTogglePressed = true;
	}
	else {
		viewTogglePressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (!lightsTogglePressed)
			lightsEnabled = !lightsEnabled;
		lightsTogglePressed = true;
	}
	else {
		lightsTogglePressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (!insideLightsTogglePressed)
			insideLightsEnabled = !insideLightsEnabled;
		insideLightsTogglePressed = true;
	}
	else {
		insideLightsTogglePressed = false;
	}

	// --- CONTROL DE CÁMARA DE ROVERS POR PROXIMIDAD ---

	// 1. Definimos dónde está el panel de control (Ajusta la X y Z si es necesario)
	glm::vec3 posPanelControl(11.0f, position.y, -7.0f); // Ignoramos la altura (Y) para no complicarlo

	// 2. Calculamos la distancia entre el jugador y el panel
	float distanciaAlPanel = glm::distance(position, posPanelControl);

	// 3. Creamos nuestra condición: ¿Está a menos de 1 metro?
	bool cercaDelPanel = (distanciaAlPanel < 2.0f);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (!roverCameraTogglePressed) {
			if (activeRoverCamera == 0) {
				activeRoverCamera = -1; // Siempre te permite "apagar" la cámara
			}
			else if (cercaDelPanel) {
				activeRoverCamera = 0;  // Solo enciende si estás cerca
			}
			else {
				std::cout << "Señal debil: Acercate al panel de control para usar el Rover 1." << std::endl;
			}
			roverCameraTogglePressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (!roverCameraTogglePressed) {
			if (activeRoverCamera == 1) {
				activeRoverCamera = -1;
			}
			else if (cercaDelPanel) {
				activeRoverCamera = 1;
			}
			else {
				std::cout << "Señal debil: Acercate al panel de control para usar el Rover 2." << std::endl;
			}
			roverCameraTogglePressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (!roverCameraTogglePressed) {
			if (activeRoverCamera == 2) {
				activeRoverCamera = -1;
			}
			else if (cercaDelPanel) {
				activeRoverCamera = 2;
			}
			else {
				std::cout << "Señal debil: Acercate al panel de control para usar el Rover 3." << std::endl;
			}
			roverCameraTogglePressed = true;
		}
	}
	else {
		roverCameraTogglePressed = false;
	}

	// Control de brillo de electrónicos con tecla F
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (!fKeyPressed) {
			electronicsBrightness += 0.05f;
			if (electronicsBrightness >= 6.95f)
				electronicsBrightness = 0.05f;
			fKeyPressed = true;
		}
	}
	else {
		fKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		if (!gKeyPressed) {
			electronicsBrightness -= 0.05f;
			if (electronicsBrightness <= 0.05f)
				electronicsBrightness = 6.95f;
			gKeyPressed = true;
		}
	}
	else {
		gKeyPressed = false;
	}

	// ============ CONTROL DE TIEMPO (TECLA T) ============
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!timeAccelPressed) {
			// Ciclar entre velocidades: 1x -> 4x -> 8x -> 1x
			if (timeMultiplier < 2.0f) {
				timeMultiplier = 4.0f;
				std::cout << "Velocidad del tiempo: 4x" << std::endl;
			}
			else if (timeMultiplier < 6.0f) {
				timeMultiplier = 8.0f;
				std::cout << "Velocidad del tiempo: 8x" << std::endl;
			}
			else {
				timeMultiplier = 1.0f;
				std::cout << "Velocidad del tiempo: 1x (Normal)" << std::endl;
			}
			timeAccelPressed = true;
		}
	}
	else {
		timeAccelPressed = false;
	}

	// --- INTERACCIÓN DE COMIDA (COMER Y RELLENAR) ---
	glm::vec3 posMesaComida(-14.5f, position.y, -8.0f); // Centro de la mesa

	// Si estás a menos de 3.0 metros de la mesa
	if (glm::distance(position, posMesaComida) < 3.0f) {

		// TECLA C: Comer
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !isEating) {
			// Buscamos cuál es la primera comida que sobra en la mesa
			if (isFood1Visible) { activeFood = 1; isEating = true; eatingTimer = 0.0f; }
			else if (isFood2Visible) { activeFood = 2; isEating = true; eatingTimer = 0.0f; }
			else if (isFood3Visible) { activeFood = 3; isEating = true; eatingTimer = 0.0f; }
			else if (isFood4Visible) { activeFood = 4; isEating = true; eatingTimer = 0.0f; }
			if (activeFood != 0) {
				// Usamos el ../ para que salga de la carpeta bin y busque en sound
				// El "false" significa que NO se va a repetir en bucle infinito
				SoundEngine->play2D("../sound/comer.mp3", false);
			}
		}

		// TECLA R: Rellenar
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			isFood1Visible = true;
			isFood2Visible = true;
			isFood3Visible = true;
			isFood4Visible = true;
			isEating = false;
			activeFood = 0;
		}
	}
}

// glfw: Actualizamos el puerto de vista si hay cambios del tamaño
// de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Callback del movimiento y eventos del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

    float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; 
	float mouseSensitivity = 0.5f;
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	lastX = (float)xpos;
	lastY = (float)ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
 forwardView = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
	rotateCharacter = glm::degrees(std::atan2(forwardView.x, forwardView.z));
	UpdateCharacterCameras();
}

// glfw: Complemento para el movimiento y eventos del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}
