/*
 * Program 4 example with diffuse and spline camera PRESS 'g'
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn (spline D. McGirr)
 */

#include <iostream>
#include <windows.h>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include "Camera.h"
#include <chrono>
#include "Geometry.h"
#include <map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927
#define GROUND_SIZE 25.0
#define EMERALD 0
#define JADE 1
#define RUBY 2
#define PEARL 3
#define OBSIDIAN 4
#define CHROME 5
#define GOLD 6

// Dummy Parts
#define RIGHT_FOOT 0
#define RIGHT_ANKLE 1
#define RIGHT_LOWER_LEG 2
#define RIGHT_KNEE 3
#define RIGHT_UPPER_LEG 4
#define RIGHT_PELVIS 5
#define LEFT_FOOT 6
#define LEFT_ANKLE 7
#define LEFT_LOWER_LEG 8
#define LEFT_KNEE 9
#define LEFT_UPPER_LEG 10
#define LEFT_PELVIS 11
#define HIPS 12
#define BELLY 13
#define TORSO 14
#define RIGHT_SHOULDER 15
#define RIGHT_BICEP 16
#define RIGHT_ELBOW 17
#define RIGHT_FOREARM 18
#define RIGHT_WRIST 19
#define RIGHT_HAND 20
#define LEFT_SHOULDER 21
#define LEFT_BICEP 22
#define LEFT_ELBOW 23
#define LEFT_FOREARM 24
#define LEFT_WRIST 25
#define LEFT_HAND 26
#define NECK 27
#define HEAD 28

// Dummy Keyframe Data
#define NUM_KEYFRAMES 3
#define NUM_FLOATS 12
#define URLR1 0
#define URLR2 1
#define LRLR1 2
#define ULLR1 3
#define ULLR2 4
#define LLLR1 5
#define URAR1 6
#define URAR2 7
#define LRAR1 8
#define ULAR1 9
#define ULAR2 10
#define LLAR1 11

// Coins Data
#define NUM_COINS 10
#define MAX_COIN_HEIGHT 2

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;
using namespace chrono;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong has diffuse
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> instanceProg;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	// shader program for skybox
	std::shared_ptr<Program> cubeProg;

	//Camera
	Camera camera;

	//cast of geometry
	Geometry palmtree01;
	Geometry ramp01;
	Geometry ramp02;
	Geometry ramp03;
	Geometry ramp04;
	Geometry ramp05;
	Geometry ramp06;
	Geometry skate_left;
	Geometry skate_right;
	Geometry dummy;
	Geometry coin01;
	Geometry coin02;
	Geometry coin03;
	Geometry coin04;
	Geometry coin05;
	Geometry coin06;
	Geometry coin07;
	Geometry coin08;
	Geometry coin09;
	Geometry coin10;



	//Game object collection
	std::map<int, Geometry> coins;

	shared_ptr<Shape> mesh;

	shared_ptr<Shape> cube;

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj = 0; 
	GLuint GrndNorBuffObj = 0;
	GLuint GrndTexBuffObj = 0;
	GLuint GIndxBuffObj = 0;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;
	GLuint palmVAO;

	//the image to use as a texture (ground)
	shared_ptr<Texture> texture1;	
	shared_ptr<Texture> texture2;

	unsigned int cubeMapTexture = 0;

	float kf[NUM_KEYFRAMES][NUM_FLOATS] = {
		// middle 
		{0, 0, 0, 0, 0, 0, glm::half_pi<float>(), 0, 0, -glm::half_pi<float>(), 0, 0},
		// skate right
		{glm::quarter_pi<float>()*0.25, glm::quarter_pi<float>()*1.75, glm::quarter_pi<float>()*0.25, 0, 0, 0, -glm::half_pi<float>(), -glm::half_pi<float>(), glm::half_pi<float>(), glm::quarter_pi<float>()*1.5, glm::half_pi<float>(), 0},
		// skate left
		{0, 0, 0, -glm::quarter_pi<float>()*0.25, glm::quarter_pi<float>() * 1.75, glm::quarter_pi<float>() * 0.25, glm::quarter_pi<float>()*1.5, glm::half_pi<float>(), 0, -glm::half_pi<float>(), -glm::half_pi<float>(), -glm::half_pi<float>()}
	};

	// skybox
	vector<std::string> faces{
		"space.jpg",
		"space.jpg",
		"space.jpg",
		"space.jpg",
		"space02.jpg",
		"space.jpg",
	};

	// palmtrees
	float offsets[168];

	//animation data
	float speed = 0.135;
	float t = 0.0f;
	float interpolate = 0.055;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	int g_count = 0;
	float radius = 2.5f;
	float model_height = -0.1f;

	//camera
	float cRad = 3.0;
	double g_phi = 0;
	double g_theta = -glm::half_pi<float>();
	vec3 deltaLookAt = camera.getLookAt();
	vec3 end = vec3(0, 0, 0);

	//light
	float light1_x = -25;
	float light1_y = 1.5;
	float light1_z = 25;

	float light2_x = 25;
	float light2_y = 1.5;
	float light2_z = -25;

	float light3_x = -25;
	float light3_y = 1.5;
	float light3_z = -25;

	float light4_x = 25;
	float light4_y = 1.5;
	float light4_z = 25;

	Spline splinepath[2];
	bool goCamera = false;

	void resize_obj(std::vector<tinyobj::shape_t>& shapes) {
		float minX, minY, minZ;
		float maxX, maxY, maxZ;
		float scaleX, scaleY, scaleZ;
		float shiftX, shiftY, shiftZ;
		float epsilon = 0.001;

		minX = minY = minZ = 1.1754E+38F;
		maxX = maxY = maxZ = -1.1754E+38F;

		//Go through all vertices to determine min and max of each dimension
		for (size_t i = 0; i < shapes.size(); i++) {
			for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
				if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
				if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

				if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
				if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

				if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
				if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
			}
		}

		//From min and max compute necessary scale and shift for each dimension
		float maxExtent, xExtent, yExtent, zExtent;
		xExtent = maxX - minX;
		yExtent = maxY - minY;
		zExtent = maxZ - minZ;
		if (xExtent >= yExtent && xExtent >= zExtent) {
			maxExtent = xExtent;
		}
		if (yExtent >= xExtent && yExtent >= zExtent) {
			maxExtent = yExtent;
		}
		if (zExtent >= xExtent && zExtent >= yExtent) {
			maxExtent = zExtent;
		}
		scaleX = 2.0 / maxExtent;
		shiftX = minX + (xExtent / 2.0);
		scaleY = 2.0 / maxExtent;
		shiftY = minY + (yExtent / 2.0);
		scaleZ = 2.0 / maxExtent;
		shiftZ = minZ + (zExtent) / 2.0;

		//Go through all verticies shift and scale them
		for (size_t i = 0; i < shapes.size(); i++) {
			for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
				shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
				assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
				assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
				shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
				assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
				assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
				shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
				assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
				assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
			}
		}
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
			goCamera = !goCamera;
			g_phi = 0;
			g_theta = -glm::pi<float>() / 2;
			camera.setEye(end);
			camera.setLookAt(0, 0, -1);
			deltaLookAt.x = 0;
			deltaLookAt.y = 0;
			deltaLookAt.z = 0;
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS) {
			goCamera = false;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			forward = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			forward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			backward = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			backward = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			left = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			left = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			right = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			right = false;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}


	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {
		if (goCamera == false) {
			/*cout << "xDel + yDel " << deltaX << " " << deltaY << endl;*/
			//fill in for game camera
			/*cout << phi << endl;*/
			/*if (g_phi > 1.396) {
				g_phi = 1.396;
			}
			else if (g_phi < -1.396) {
				g_phi = -1.396;
			}
			else {
				g_phi += ((glm::pi<float>()) / glm::radians(180.0)) * deltaY;
			}*/
			g_phi = 0;
			g_theta += ((2 * glm::pi<float>()) / glm::radians(180.0)) * deltaX;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		g_theta = -PI/2.0;

		// Initialize the GLSL program that we will use for local shading
		instanceProg = make_shared<Program>();
		instanceProg->setVerbose(true);
		instanceProg->setShaderNames(resourceDirectory + "/palm_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		instanceProg->init();
		instanceProg->addUniform("P");
		instanceProg->addUniform("V");
		instanceProg->addUniform("M");
		instanceProg->addUniform("MatAmb");
		instanceProg->addUniform("MatDif");
		instanceProg->addUniform("MatSpec");
		instanceProg->addUniform("MatShine");
		instanceProg->addUniform("lightPos");
		instanceProg->addUniform("lightPos2");
		instanceProg->addUniform("lightPos3");
		instanceProg->addUniform("lightPos4");
		instanceProg->addUniform("discoColor");
		instanceProg->addUniform("offsets");
		instanceProg->addAttribute("vertPos");
		instanceProg->addAttribute("vertNor");
		instanceProg->addAttribute("vertTex");

		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addUniform("lightPos2");
		prog->addUniform("lightPos3");
		prog->addUniform("lightPos4");
		prog->addUniform("discoColor");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex"); //silence error


		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatAmb");
		texProg->addUniform("MatDif");
		texProg->addUniform("MatSpec");
		texProg->addUniform("MatShine");
		texProg->addUniform("lightPos");
		texProg->addUniform("lightPos2");
		texProg->addUniform("lightPos3");
		texProg->addUniform("lightPos4");
		texProg->addUniform("discoColor");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		// skybox
		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addUniform("skybox");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");
		cubeProg->addAttribute("vertTex");

		//read in a load the texture

  		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/skyBox/back.jpg");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/cartoonWood.jpg");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		// init splines up and down
       splinepath[0] = Spline(glm::vec3(-20,15,-15), glm::vec3(5,-5, 5), glm::vec3(3, -5, 5), glm::vec3(6, 8, 2), 5);
       splinepath[1] = Spline(glm::vec3(6,8,2), glm::vec3(3,-5,5), glm::vec3(7, 4, 5), end, 5);
	}

	Geometry createEntity(const std::string& resourceDirectory, string filename)
	{
		Geometry entity;
		float gxmin, gxmax, gymin, gymax, gzmin, gzmax;
		gxmin = gymin = gzmin = 1.1754E+38F;
		gxmax = gymax = gzmax = -1.1754E+38F;

		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;

		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/" + filename).c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			resize_obj(TOshapes);
			// loop through shapes, create indiv shapes, and add to Geometry
			for (long unsigned int i = 0; i < TOshapes.size(); i++) {
				mesh = make_shared<Shape>();
				mesh->createShape(TOshapes[i]);
				mesh->measure();
				mesh->init();
				// check shape mins and maxs for global Bbox
				if (mesh->min.x < gxmin) gxmin = mesh->min.x;
				if (mesh->max.x > gxmax) gxmax = mesh->max.x;
				if (mesh->min.y < gymin) gymin = mesh->min.y;
				if (mesh->max.y > gymax) gymax = mesh->max.y;
				if (mesh->min.z < gzmin) gzmin = mesh->min.z;
				if (mesh->max.z > gzmax) gzmax = mesh->max.z;
				entity.shapes.push_back(mesh);
			}
		}
		entity.gMin.x = gxmin;
		entity.gMax.x = gxmax;
		entity.gMin.y = gymin;
		entity.gMax.y = gymax;
		entity.gMin.z = gzmin;
		entity.gMax.z = gzmax;
		entity.calculateOriginTranslate();

		return entity;
	}

	void initOffsets(float* offsets, int row_count) {
		float x = 0;
		float x_2 = 0;
		float y = 0;
		float z = 0;
		for (int i = 0; i < 56; i++) {
			if (i <= 13) {
				offsets[3 * i] = x;
				offsets[3 * i + 1] = -0.1;
				offsets[3 * i + 2] = 0;
				x += 0.75;
			}
			else if (i <= 27) {
				offsets[3 * i] = 0.1;
				offsets[3 * i + 1] = z;
				offsets[3 * i + 2] = 0;
				z -= 0.75;
			}
			else if (i <= 41) {
				offsets[3 * i] = x_2;
				offsets[3 * i + 1] = -0.75 * 13.3;
				offsets[3 * i + 2] = 0;
				x_2 += 0.75;
			}
			else {
				offsets[3 * i] = 0.75 * 13.3;;
				offsets[3 * i + 1] = y;
				offsets[3 * i + 2] = 0;
				y -= 0.75;
			}
		}
	}

	void setBoundingBoxRamp01(Geometry* ramp01) {
		float rotAng = glm::pi<float>() * 1.2;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(16, -0.9, -6));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(2));
		ramp01->gMax = trans * rot * scale * vec4(ramp01->gMax, 1.0f);
		ramp01->gMin = trans * rot * scale * vec4(ramp01->gMin, 1.0f);
	}

	void setBoundingBoxRamp02(Geometry* ramp02) {
		float rotAng = glm::pi<float>() * 1.65;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(-8, -0.9, -16));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(2));
		ramp02->gMax = trans * rot * scale * vec4(ramp02->gMax, 1.0f);
		ramp02->gMin = trans * rot * scale * vec4(ramp02->gMin, 1.0f);
	}

	void setBoundingBoxRamp03(Geometry* ramp03) {
		float rotAng = glm::quarter_pi<float>();
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(-16, -0.9, 3));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(2));
		ramp03->gMax = trans * rot * scale * vec4(ramp03->gMax, 1.0f);
		ramp03->gMin = trans * rot * scale * vec4(ramp03->gMin, 1.0f);
	}

	void setBoundingBoxRamp04(Geometry* ramp04) {
		float rotAng = 3 * glm::half_pi<float>();
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(0, -0.7, 1));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(8));
		ramp04->gMax = trans * scale * rot * vec4(ramp04->gMax, 1.0f);
		ramp04->gMin = trans * scale * rot  * vec4(ramp04->gMin, 1.0f);
	}

	void setBoundingBoxRamp05(Geometry* ramp05) {
		float rotAng = glm::half_pi<float>() * 0.7;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(11, -0.8, 17));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(5));
		ramp05->gMax = trans * scale * rot * vec4(ramp05->gMax, 1.0f);
		ramp05->gMin = trans * scale * rot * vec4(ramp05->gMin, 1.0f);
	}

	void setBoundingBoxRamp06(Geometry* ramp06) {
		float rotAng = glm::half_pi<float>() * 0.8;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), vec3(5, -0.8, -18));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAng, vec3(0.0, 1.0, 0.0));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), vec3(5));
		ramp06->gMax = trans * scale * rot * vec4(ramp06->gMax, 1.0f);
		ramp06->gMin = trans * scale * rot * vec4(ramp06->gMin, 1.0f);
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry

		// cube
		string errStr;
		vector<tinyobj::shape_t> TOshapesC;
		vector<tinyobj::material_t> objMaterialsC;
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapesC, objMaterialsC, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			cube = make_shared<Shape>();
			cube->createShape(TOshapesC[0]);
			cube->measure();
			cube->init();
		}

		palmtree01 = createEntity(resourceDirectory, "palmtree01.obj");
		ramp01 = createEntity(resourceDirectory, "ramp01.obj");
		setBoundingBoxRamp01(&ramp01);
		ramp02 = createEntity(resourceDirectory, "ramp01.obj");
		setBoundingBoxRamp02(&ramp02);
		ramp03 = createEntity(resourceDirectory, "ramp01.obj");
		setBoundingBoxRamp03(&ramp03);
		ramp04 = createEntity(resourceDirectory, "ramp02.obj");
		setBoundingBoxRamp04(&ramp04);
		ramp05 = createEntity(resourceDirectory, "ramp05.obj");
		setBoundingBoxRamp05(&ramp05);
		ramp06 = createEntity(resourceDirectory, "ramp03.obj");
		setBoundingBoxRamp06(&ramp06);
		skate_left = createEntity(resourceDirectory, "rollerSkateLeftFoot.obj");
		skate_right = createEntity(resourceDirectory, "rollerSkateRightFoot.obj");
		dummy = createEntity(resourceDirectory, "dummy.obj");

		coins[1] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[1].pos = glm::vec3(-13, 0.5, 19);
		coins[1].orientation = glm::vec3(0, 1, 0);
		coins[1].velocity = glm::vec3(0, 0.03, 0);

		coins[2] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[2].pos = glm::vec3(-10, 0.5, -16);
		coins[2].orientation = glm::vec3(0, 1, 0);
		coins[2].velocity = glm::vec3(0, 0.03, 0);

		coins[3] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[3].pos = glm::vec3(-16, 0.5, 5);
		coins[3].orientation = glm::vec3(0, 1, 0);
		coins[3].velocity = glm::vec3(0, 0.03, 0);

		coins[4] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[4].pos = glm::vec3(16, 0.5, -8);
		coins[4].orientation = glm::vec3(0, 1, 0);
		coins[4].velocity = glm::vec3(0, 0.03, 0);

		coins[5] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[5].pos = glm::vec3(13, 0.5, 17);
		coins[5].orientation = glm::vec3(0, 1, 0);
		coins[5].velocity = glm::vec3(0, 0.03, 0);

		coins[6] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[6].pos = glm::vec3(5, 0.5, 5);
		coins[6].orientation = glm::vec3(0, 1, 0);
		coins[6].velocity = glm::vec3(0, 0.03, 0);

		coins[7] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[7].pos = glm::vec3(-5, 0.5, 8);
		coins[7].orientation = glm::vec3(-1, 0, 0);
		coins[7].velocity = glm::vec3(-0.05, 0, 0);

		coins[8] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[8].pos = glm::vec3(5, 0.5, -8);
		coins[8].orientation = glm::vec3(-1, 0, 0);
		coins[8].velocity = glm::vec3(-0.05, 0, 0);

		coins[9] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[9].pos = glm::vec3(5, 0.5, -15);
		coins[9].orientation = glm::vec3(0, 0, 1);
		coins[9].velocity = glm::vec3(0, 0, 0.05); 

		coins[10] = createEntity(resourceDirectory, "dogeCoin.obj");
		coins[10].pos = glm::vec3(0, 0.5, -6);
		coins[10].orientation = glm::vec3(0, 0, -1);
		coins[10].velocity = glm::vec3(0, 0, 0.05);

		//camera
		camera.setEye(vec3(0, 0, 0));
		camera.setLookAt(0, 0, -4);
		dummy.pos = vec3(0, 0, -4);

		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = GROUND_SIZE;
		float g_groundY = -0.25;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	texture2->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
  		SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

	 // skybox
	 unsigned int createSky(string dir, vector<string> faces) {
		 unsigned int textureID;
		 glGenTextures(1, &textureID);
		 glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		 int width, height, nrChannels;
		 stbi_set_flip_vertically_on_load(false);
		 for (GLuint i = 0; i < faces.size(); i++) {
			 unsigned char* data = stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			 if (data) {
				 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			 }
			 else {
				 cout << "failed to load: " << (dir + faces[i]).c_str() << endl;
			 }
		 }
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		 cout << " creating cube map any errors: " << glGetError() << endl;
		 return textureID;
	 }

     //helper function to pass material data to the GPU
	void SetMaterial(shared_ptr<Program> curS, int i) {
		switch (i) {
		case EMERALD: //
			glUniform3f(curS->getUniform("MatAmb"), 0.0215f, 0.1745f, 0.0215f);
			glUniform3f(curS->getUniform("MatDif"), 0.07568f, 0.61424f, 0.07568f);
			glUniform3f(curS->getUniform("MatSpec"), 0.633f, 0.727811f, 0.633f);
			glUniform1f(curS->getUniform("MatShine"), 0.6f);
			break;
		case JADE: // 
			glUniform3f(curS->getUniform("MatAmb"), 0.135f, 0.2225f, 0.1575f);
			glUniform3f(curS->getUniform("MatDif"), 0.54f, 0.89f, 0.63f);
			glUniform3f(curS->getUniform("MatSpec"), 0.316228f, 0.316228f, 0.316228f);
			glUniform1f(curS->getUniform("MatShine"), 0.1f);
			break;
		case RUBY: //
			glUniform3f(curS->getUniform("MatAmb"), 0.1745f, 0.01175f, 0.01175f);
			glUniform3f(curS->getUniform("MatDif"), 0.61424f, 0.04136f, 0.04136f);
			glUniform3f(curS->getUniform("MatSpec"), 0.727811f, 0.626959f, 0.626959f);
			glUniform1f(curS->getUniform("MatShine"), 0.6f);
			break;
		case PEARL: //
			glUniform3f(curS->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
			glUniform3f(curS->getUniform("MatDif"), 1, 0.829f, 0.829f);
			glUniform3f(curS->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
			glUniform1f(curS->getUniform("MatShine"), 0.088f);
			break;
		case OBSIDIAN:
			glUniform3f(curS->getUniform("MatAmb"), 0.05375f, 0.05f, 0.06625f);
			glUniform3f(curS->getUniform("MatDif"), 0.18275f, 0.17f, 0.22525f);
			glUniform3f(curS->getUniform("MatSpec"), 0.332741f, 0.328634f, 0.346435f);
			glUniform1f(curS->getUniform("MatShine"), 0.3f);
			break;
		case CHROME:
			glUniform3f(curS->getUniform("MatAmb"), 0.25f, 0.25f, 0.25f);
			glUniform3f(curS->getUniform("MatDif"), 0.4f, 0.4f, 0.4f);
			glUniform3f(curS->getUniform("MatSpec"), 0.774597f, 0.774597f, 0.774597f);
			glUniform1f(curS->getUniform("MatShine"), 0.6f);
			break;
		case GOLD:
			glUniform3f(curS->getUniform("MatAmb"), 0.24725f, 0.1995f, 0.0745f);
			glUniform3f(curS->getUniform("MatDif"), 0.75164f, 0.60648f, 0.22648f);
			glUniform3f(curS->getUniform("MatSpec"), 0.628281f, 0.555802f, 0.366065f);
			glUniform1f(curS->getUniform("MatShine"), 0.4f);
			break;
		}
	}

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

   	/* camera controls - do not change */
	void SetView(shared_ptr<Program>  shader) {
		glm::mat4 Cam = glm::lookAt(camera.getEye(), camera.getLookAt(), camera.getUp());
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}

	void drawSkyBox(shared_ptr<MatrixStack> ident, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> V) {
		cubeProg->bind();
		ident->pushMatrix();
		ident->loadIdentity();
		ident->scale(40);
		//to draw the sky box bind the right shader 
		//set the projection matrix - can use the same one 
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE,
			value_ptr(Projection->topMatrix()));
		//set the depth function to always draw the box! 
		glDepthFunc(GL_LEQUAL);
		//set up view matrix to include your view transforms 
		//(your code likely will be different depending 
		glUniformMatrix4fv(cubeProg->getUniform("V"), 1,
			GL_FALSE, value_ptr(V->topMatrix()));
		//set and send model transforms - likely want a bigger cube 
		glUniformMatrix4fv(cubeProg->getUniform("M"), 1,
			GL_FALSE, value_ptr(ident->topMatrix()));
		//bind the cube map texture 
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		//draw the actual cube 
		cube->draw(cubeProg);
		//set the depth test back to normal! 
		glDepthFunc(GL_LESS);
		ident->popMatrix();
		//unbind the shader for the skybox 
		cubeProg->unbind();
	}

	/* code to draw waving hierarchical model */
	void drawSkateLeft(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> Model, shared_ptr<Program> prog, float t, float model_height) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		auto newV = make_shared<MatrixStack>();
		newV->pushMatrix();
		newV->loadIdentity();
		newV->translate(vec3(0, 0, -4));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(newV->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f));
		//draw the waving HM
		SetMaterial(prog, JADE);
		// simplified for releaes code

		// Draw Hips
		Model->pushMatrix();
		Model->loadIdentity();
		Model->translate(vec3(0, model_height, 0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 1.0, 0.0));
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[HIPS]->draw(prog);

		//Draw Right Upper Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_PELVIS]->draw(prog);
		dummy.shapes[RIGHT_UPPER_LEG]->draw(prog);

		// Draw Right Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_KNEE]->draw(prog);
		dummy.shapes[RIGHT_LOWER_LEG]->draw(prog);

		// Draw right skate
		Model->pushMatrix();
		// z, x, y
		Model->translate(vec3(0, -0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();


		// Draw Left Upper Leg
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.1, 0));
		Model->rotate((1 - t) * kf[0][ULLR1] + (t)*kf[2][ULLR1], vec3(0, 0, 1));
		Model->rotate((1 - t) * kf[0][ULLR2] + (t)*kf[2][ULLR2], vec3(0, 1, 0));
		Model->translate(vec3(0, -0.1, 0));
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[LEFT_PELVIS]->draw(prog);
		dummy.shapes[LEFT_UPPER_LEG]->draw(prog);

		// Draw Left Lower Leg
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.1, 0.5));
		Model->rotate((1 - t) * kf[0][LLLR1] + (t)*kf[2][LLLR1], vec3(0, 1, 0));
		Model->translate(vec3(0, -0.1, 0.5));
		setModel(prog, Model);
		dummy.shapes[LEFT_KNEE]->draw(prog);
		dummy.shapes[LEFT_LOWER_LEG]->draw(prog);

		// Draw Left Skate
		Model->pushMatrix();
		Model->translate(vec3(0, 0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Head and Neck
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[HEAD]->draw(prog);
		dummy.shapes[NECK]->draw(prog);
		Model->popMatrix();

		// Draw Torso
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[TORSO]->draw(prog);
		dummy.shapes[BELLY]->draw(prog);

		//Draw Right Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_SHOULDER]->draw(prog);

		// Draw Right Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.23, -0.56));
		Model->rotate((1 - t)* kf[0][URAR2] + (t)*kf[2][URAR2], vec3(0, 1, 0));
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, 0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[RIGHT_BICEP]->draw(prog);

		// Draw Right Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_ELBOW]->draw(prog);

		// Draw Right Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_FOREARM]->draw(prog);
		dummy.shapes[RIGHT_WRIST]->draw(prog);
		dummy.shapes[RIGHT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Left Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_SHOULDER]->draw(prog);

		// Draw Left Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.23, -0.56));
		Model->rotate((1 - t)* kf[0][ULAR2] + (t)*kf[2][ULAR2], vec3(0, 1, 0));
		Model->rotate((1 - t)* kf[0][ULAR1] + (t)*kf[2][ULAR1], vec3(1, 0, 0));
		Model->translate(vec3(0, -0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[LEFT_BICEP]->draw(prog);

		// Draw Left Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_ELBOW]->draw(prog);

		// Draw Left Lower Arm and Hand
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.5, -0.56));
		Model->rotate((1 - t)* kf[0][LLAR1] + (t)*kf[2][LLAR1], vec3(0, 0, 1));
		Model->translate(vec3(0, -0.5, -0.56));
		setModel(prog, Model);
		dummy.shapes[LEFT_FOREARM]->draw(prog);
		dummy.shapes[LEFT_WRIST]->draw(prog);
		dummy.shapes[LEFT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Torso POP
		Model->popMatrix();

		// Hips POP
		Model->popMatrix();

		newV->popMatrix();
		prog->unbind();
	}

	/* code to draw waving hierarchical model */
	void drawSkateRight(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> Model, shared_ptr<Program> prog, float t, float model_height) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		auto newV = make_shared<MatrixStack>();
		newV->pushMatrix();
		newV->loadIdentity();
		newV->translate(vec3(0, 0, -4));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(newV->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x , light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f));
		//draw the waving HM
		SetMaterial(prog, JADE);
		// simplified for releaes code

		// Draw Hips
		Model->pushMatrix();
		Model->loadIdentity();
		Model->translate(vec3(0, model_height, 0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 1.0, 0.0));
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[HIPS]->draw(prog);

		//Draw Right Upper Leg
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.1, 0));
		Model->rotate((1 - t) * kf[0][URLR1] + (t)*kf[1][URLR1], vec3(0, 0, 1));
		Model->rotate((1 - t) * kf[0][URLR2] + (t)*kf[1][URLR2], vec3(0, 1, 0));
		Model->translate(vec3(0, 0.1, 0));
		setModel(prog, Model);
		dummy.shapes[RIGHT_PELVIS]->draw(prog);
		dummy.shapes[RIGHT_UPPER_LEG]->draw(prog);

		// Draw Right Lower Leg
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.1, 0.5));
		Model->rotate((1 - t) * kf[0][LRLR1] + (t)*kf[1][LRLR1], vec3(0, 1, 0));
		Model->translate(vec3(0, 0.1, 0.5));
		setModel(prog, Model);
		dummy.shapes[RIGHT_KNEE]->draw(prog);
		dummy.shapes[RIGHT_LOWER_LEG]->draw(prog);

		// Draw right skate
		Model->pushMatrix();
		// z, x, y
		Model->translate(vec3(0, -0.09, -1.01)); // 0, -0.09, -1.01
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();


		// Draw Left Upper Leg
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[LEFT_PELVIS]->draw(prog);
		dummy.shapes[LEFT_UPPER_LEG]->draw(prog);

		// Draw Left Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_KNEE]->draw(prog);
		dummy.shapes[LEFT_LOWER_LEG]->draw(prog);

		// Draw Left Skate
		Model->pushMatrix();
		Model->translate(vec3(0, 0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Head and Neck
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[HEAD]->draw(prog);
		dummy.shapes[NECK]->draw(prog);
		Model->popMatrix();

		// Draw Torso
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[TORSO]->draw(prog);
		dummy.shapes[BELLY]->draw(prog);

		//Draw Right Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_SHOULDER]->draw(prog);

		// Draw Right Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.23, -0.56));
		Model->rotate((1 - t)* kf[0][URAR2] + (t)*kf[1][URAR2], vec3(0, 1, 0));
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, 0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[RIGHT_BICEP]->draw(prog);

		// Draw Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_ELBOW]->draw(prog);

		// Draw Right Lower Arm and Hand
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.5, -0.56));
		Model->rotate((1-t)*kf[0][LRAR1] + (t)*kf[1][LRAR1], vec3(0, 0, 1));
		Model->translate(vec3(0, 0.5, -0.56));
		setModel(prog, Model);
		dummy.shapes[RIGHT_FOREARM]->draw(prog);
		dummy.shapes[RIGHT_WRIST]->draw(prog);
		dummy.shapes[RIGHT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Left Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_SHOULDER]->draw(prog);

		// Draw Left Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.23, -0.56));
		Model->rotate((1 - t)* kf[0][ULAR2] + (t)*kf[1][ULAR2], vec3(0, 1, 0));
		Model->rotate(-glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, -0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[LEFT_BICEP]->draw(prog);

		// Draw Left Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_ELBOW]->draw(prog);

		// Draw Left Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_FOREARM]->draw(prog);
		dummy.shapes[LEFT_WRIST]->draw(prog);
		dummy.shapes[LEFT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Torso POP
		Model->popMatrix();

		// Hips POP
		Model->popMatrix();

		newV->popMatrix();
		prog->unbind();
	}

	/* code to draw waving hierarchical model */
	void drawSkateBackward(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> Model, shared_ptr<Program> prog, float t, float model_height) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		auto newV = make_shared<MatrixStack>();
		newV->pushMatrix();
		newV->loadIdentity();
		newV->translate(vec3(0, 0, -4));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(newV->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f));
		//draw the waving HM
		SetMaterial(prog, JADE);
		// simplified for releaes code

		// Draw Hips
		Model->pushMatrix();
		Model->loadIdentity();
		Model->rotate((1-t)*glm::quarter_pi<float>() * 0.5 + (t)*-glm::quarter_pi<float>()*0.5, vec3(0, 1, 0));
		Model->translate(vec3(0, model_height, 0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 1.0, 0.0));
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[HIPS]->draw(prog);

		//Draw Right Upper Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_PELVIS]->draw(prog);
		dummy.shapes[RIGHT_UPPER_LEG]->draw(prog);

		// Draw Right Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_KNEE]->draw(prog);
		dummy.shapes[RIGHT_LOWER_LEG]->draw(prog);

		// Draw right skate
		Model->pushMatrix();
		// z, x, y
		Model->translate(vec3(0, -0.09, -1.01));
		Model->scale(0.20f);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();


		// Draw Left Upper Leg
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[LEFT_PELVIS]->draw(prog);
		dummy.shapes[LEFT_UPPER_LEG]->draw(prog);

		// Draw Left Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_KNEE]->draw(prog);
		dummy.shapes[LEFT_LOWER_LEG]->draw(prog);

		// Draw Left Skate
		Model->pushMatrix();
		Model->translate(vec3(0, 0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Head and Neck
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[HEAD]->draw(prog);
		dummy.shapes[NECK]->draw(prog);
		Model->popMatrix();

		// Draw Torso
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[TORSO]->draw(prog);
		dummy.shapes[BELLY]->draw(prog);

		//Draw Right Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_SHOULDER]->draw(prog);

		// Draw Right Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.23, -0.56));
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, 0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[RIGHT_BICEP]->draw(prog);

		// Draw Right Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_ELBOW]->draw(prog);

		// Draw Right Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_FOREARM]->draw(prog);
		dummy.shapes[RIGHT_WRIST]->draw(prog);
		dummy.shapes[RIGHT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Left Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_SHOULDER]->draw(prog);

		// Draw Left Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.23, -0.56));
		Model->rotate(-glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, -0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[LEFT_BICEP]->draw(prog);

		// Draw Left Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_ELBOW]->draw(prog);

		// Draw Left Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_FOREARM]->draw(prog);
		dummy.shapes[LEFT_WRIST]->draw(prog);
		dummy.shapes[LEFT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Torso POP
		Model->popMatrix();

		// Hips POP
		Model->popMatrix();

		newV->popMatrix();
		prog->unbind();
	}

	/* code to draw waving hierarchical model */
	void drawDummy(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> Model, shared_ptr<Program> prog, float model_height) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		auto newV = make_shared<MatrixStack>();
		newV->pushMatrix();
		newV->loadIdentity();
		newV->translate(vec3(0, 0, -4));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(newV->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f));
		//draw the waving HM
		SetMaterial(prog, JADE);
		// simplified for releaes code

		// Draw Hips
		Model->pushMatrix();
		Model->loadIdentity();
		Model->translate(vec3(0, model_height, 0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0));
		Model->rotate(glm::half_pi<float>(), glm::vec3(0.0, 1.0, 0.0));
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[HIPS]->draw(prog);

		//Draw Right Upper Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_PELVIS]->draw(prog);
		dummy.shapes[RIGHT_UPPER_LEG]->draw(prog);

		// Draw Right Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_KNEE]->draw(prog);
		dummy.shapes[RIGHT_LOWER_LEG]->draw(prog);

		// Draw right skate
		Model->pushMatrix();
		// z, x, y
		Model->translate(vec3(0, -0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();


		// Draw Left Upper Leg
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[LEFT_PELVIS]->draw(prog);
		dummy.shapes[LEFT_UPPER_LEG]->draw(prog);

		// Draw Left Lower Leg
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_KNEE]->draw(prog);
		dummy.shapes[LEFT_LOWER_LEG]->draw(prog);

		// Draw Left Skate
		Model->pushMatrix();
		Model->translate(vec3(0, 0.09, -1.01));
		Model->scale(0.20);
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		skate_right.drawShapes(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Head and Neck
		Model->pushMatrix();
		SetMaterial(prog, JADE);
		setModel(prog, Model);
		dummy.shapes[HEAD]->draw(prog);
		dummy.shapes[NECK]->draw(prog);
		Model->popMatrix();

		// Draw Torso
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[TORSO]->draw(prog);
		dummy.shapes[BELLY]->draw(prog);

		//Draw Right Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_SHOULDER]->draw(prog);

		// Draw Right Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, 0.23, -0.56));
		Model->rotate(glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, 0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[RIGHT_BICEP]->draw(prog);

		// Draw Right Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_ELBOW]->draw(prog);

		// Draw Right Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[RIGHT_FOREARM]->draw(prog);
		dummy.shapes[RIGHT_WRIST]->draw(prog);
		dummy.shapes[RIGHT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Draw Left Shoulder
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_SHOULDER]->draw(prog);

		// Draw Left Upper Arm
		Model->pushMatrix();
		Model->translate(-vec3(0, -0.23, -0.56));
		Model->rotate(-glm::half_pi<float>(), vec3(1, 0, 0));
		Model->translate(vec3(0, -0.23, -0.56));
		setModel(prog, Model);
		dummy.shapes[LEFT_BICEP]->draw(prog);

		// Draw Left Elbow
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_ELBOW]->draw(prog);

		// Draw Left Lower Arm and Hand
		Model->pushMatrix();
		setModel(prog, Model);
		dummy.shapes[LEFT_FOREARM]->draw(prog);
		dummy.shapes[LEFT_WRIST]->draw(prog);
		dummy.shapes[LEFT_HAND]->draw(prog);

		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();
		Model->popMatrix();

		// Torso POP
		Model->popMatrix();

		// Hips POP
		Model->popMatrix();

		newV->popMatrix();
		prog->unbind();
	}

	void drawPalmTrees(shared_ptr<Program> curS, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> V) {
		curS->bind();
		glBindVertexArray(palmVAO);
		glUniformMatrix4fv(instanceProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(instanceProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
		glUniform3f(instanceProg->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(instanceProg->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(instanceProg->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(instanceProg->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(instanceProg->getUniform("discoColor"), sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f));
		glUniform3fv(instanceProg->getUniform("offsets"), 168, offsets);
		initOffsets(offsets, 1);
		SetModel(vec3(-GROUND_SIZE, 3.3, -GROUND_SIZE), 0, -glm::half_pi<float>(), 5, curS);
		palmtree01.drawInstanced(curS);
		curS->unbind();
	}

	void drawRink(shared_ptr<Program> curS, shared_ptr<MatrixStack> Projection) {
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(texProg);
		glUniform3f(texProg->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(texProg->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(texProg->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(texProg->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(texProg->getUniform("discoColor"), sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f));
		glUniform1f(texProg->getUniform("MatShine"), 27.9);
		glUniform1i(texProg->getUniform("flip"), 1);
		glUniform1i(texProg->getUniform("flip"), 1);
		drawGround(texProg);

		texProg->unbind();
	}

	void drawRamps(shared_ptr<Program> curS, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> V, shared_ptr<MatrixStack> Model) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f));

		// mini ramps
		// 1
		Model->pushMatrix();
		Model->translate(vec3(16, -0.9, -6));
		Model->rotate(glm::pi<float>()*1.2, vec3(0.0, 1.0, 0.0));
		Model->scale(2);
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp01.drawShapes(prog);
		Model->popMatrix();

		//2
		Model->pushMatrix();
		Model->translate(vec3(-8, -0.9, -16));
		Model->rotate(glm::pi<float>() * 1.65, vec3(0.0, 1.0, 0.0));
		Model->scale(2);
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp02.drawShapes(prog);
		Model->popMatrix();

		//3
		Model->pushMatrix();
		Model->translate(vec3(-16, -0.9, 3));
		Model->rotate(glm::quarter_pi<float>(), vec3(0.0, 1.0, 0.0));
		Model->scale(2);
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp03.drawShapes(prog);
		Model->popMatrix();
		
		// Center piece
		//4
		Model->pushMatrix();
		Model->translate(vec3(0, -0.7, 1)); // 0, -0.8, 3
		Model->scale(8);
		Model->rotate(3 * glm::half_pi<float>(), vec3(0.0, 1.0, 0.0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp04.drawShapes(prog);
		Model->popMatrix();

		//Back ramp
		//5
		Model->pushMatrix();
		Model->translate(vec3(11, -0.8, 17));
		Model->scale(5);
		Model->rotate(glm::half_pi<float>() * 0.7, vec3(0.0, 1.0, 0.0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp05.drawShapes(prog);
		Model->popMatrix();

		// Top ramp
		//6
		Model->pushMatrix();
		Model->translate(vec3(5, -0.8, -18));
		Model->scale(5);
		Model->rotate(glm::half_pi<float>() *0.8, vec3(0.0, 1.0, 0.0));
		SetMaterial(prog, OBSIDIAN);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		ramp06.drawShapes(prog);
		Model->popMatrix();

		prog->unbind();
	}

	Geometry updateCoinPosHorizontal(Geometry coin)
	{
		Geometry new_coin = coin;

		float move = glm::dot(coin.orientation, coin.velocity);
		if ((new_coin.pos.x > GROUND_SIZE || new_coin.pos.x < -GROUND_SIZE) || (new_coin.pos.z > GROUND_SIZE || new_coin.pos.z < -GROUND_SIZE))
		{
			new_coin.direction_flag *= -1;
			new_coin.pos.x += new_coin.direction_flag * move;
			new_coin.pos.z += new_coin.direction_flag * move;
		}
		else
		{
			new_coin.pos.x += new_coin.direction_flag * move;
			new_coin.pos.z += new_coin.direction_flag * move;
		}
		
		return new_coin;
	}

	Geometry updateCoinPosVertical(Geometry coin)
	{
		Geometry new_coin = coin;

		float move = glm::dot(coin.orientation, coin.velocity);
		if (new_coin.pos.y < 0.5 || new_coin.pos.y > MAX_COIN_HEIGHT)
		{
			new_coin.direction_flag *= -1;
			new_coin.pos.y += new_coin.direction_flag * move;
		}
		else
		{
			new_coin.pos.y += new_coin.direction_flag * move;
		}

		return new_coin;
	}

	bool isCoinCollision(Geometry* dummy, Geometry* coin)
	{
		return (dummy->pos.x - 1.5 < coin->gMin.x && dummy->pos.x + 1.5 > coin->gMax.x) && (dummy->pos.z - 1.5 < coin->gMin.z && dummy->pos.z + 1.5 > coin->gMax.z);
	}

	void drawCoins(shared_ptr<Program> curS, shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> V, shared_ptr<MatrixStack> Model)
	{
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), light1_x, light1_y, light1_z);
		glUniform3f(prog->getUniform("lightPos2"), light2_x, light2_y, light2_z);
		glUniform3f(prog->getUniform("lightPos3"), light3_x, light3_y, light3_z);
		glUniform3f(prog->getUniform("lightPos4"), light4_x, light4_y, light4_z);
		glUniform3f(prog->getUniform("discoColor"), sin(glfwGetTime() * 1.3f), sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f));

		for (int i = 1; i <= 10; i++)
		{
			if (isCoinCollision(&dummy, &coins[i]))
			{
				bool played_coin = PlaySound("coinSound.wav", NULL, SND_ASYNC);
				coins.erase(i);
			}
		}

		for (int i = 1; i <= 10; i++)
		{
			if (coins.count(i) > 0 && (i >= 1 && i <= 6))
			{
				Model->pushMatrix();
				//cout << coins[1].pos.x << " " << coins[1].pos.y << " " << coins[1].pos.z << endl;
				coins[i] = updateCoinPosVertical(coins[i]);
				Model->translate(coins[i].pos);
				Model->rotate(glm::pi<float>() / 2, vec3(0.0, 1.0, 0.0));
				Model->scale(0.5);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				coins[i].drawShapes(prog);
				Model->popMatrix();
			}
			else if (coins.count(i) > 0 && (i >= 7 && i <= 10))
			{
				Model->pushMatrix();
				//cout << coins[1].pos.x << " " << coins[1].pos.y << " " << coins[1].pos.z << endl;
				coins[i] = updateCoinPosHorizontal(coins[i]);
				Model->translate(coins[i].pos);
				//Model->rotate(glm::pi<float>() / 2, vec3(0.0, 1.0, 0.0));
				Model->scale(0.5);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				coins[i].drawShapes(prog);
				Model->popMatrix();
			}
		}

		prog->unbind();
	}

   	void updateUsingCameraPath(float frametime)  {
   	  if (goCamera && g_count <= 0) {
       if(!splinepath[0].isDone()){
       		splinepath[0].update(frametime);
			camera.setEye(splinepath[0].getPosition());
        } else {
            splinepath[1].update(frametime);
			camera.setEye(splinepath[1].getPosition());
        }
      }
	  if (splinepath[1].getPosition() == end) {
		  goCamera = false;
		  g_count = 1;
		  /*g_phi = 0;
		  g_theta = -glm::pi<float>() / 2;*/
	  }
		  
   	}

	bool isRamp01Collision(Geometry* dummy, Geometry* ramp01) {
		return (dummy->pos.x - 1.5 < ramp01->gMin.x && dummy->pos.x + 1.5 > ramp01->gMax.x) && (dummy->pos.z - 1.5 < ramp01->gMin.z && dummy->pos.z + 1.5 > ramp01->gMax.z);
	}

	bool isRamp02Collision(Geometry* dummy, Geometry* ramp02) {
		return (dummy->pos.x - 1.5 < ramp02->gMin.x && dummy->pos.x + 1.5 > ramp02->gMax.x) && (dummy->pos.z + 1.5 > ramp02->gMin.z && dummy->pos.z - 1.5 < ramp02->gMax.z);
	}

	bool isRamp03Collision(Geometry* dummy, Geometry* ramp03) {
		return (dummy->pos.x - 1.5 < ramp03->gMax.x && dummy->pos.x + 1.5 > ramp03->gMin.x) && (dummy->pos.z - 1.5 < ramp03->gMax.z && dummy->pos.z + 1.5 > ramp03->gMin.z);
	}

	bool isInMiddleofRamp04(Geometry* dummy, Geometry* ramp04) {
		return (dummy->pos.x < ramp04->gMin.x && dummy->pos.x > ramp04->gMax.x) && (dummy->pos.z < 4.55 && dummy->pos.z > -1.2);
	}

	bool onLedgesofRamp04(Geometry* dummy, Geometry* ramp04) {
		return ((dummy->pos.x < 8 && dummy->pos.x > 3.3) && (dummy->pos.z < -1.2 && dummy->pos.z > -4.5)) || ((dummy->pos.x < 8 && dummy->pos.x > 3.3) && (dummy->pos.z > 4.55 && dummy->pos.z < 7))
			|| ((dummy->pos.x < -2.5 && dummy->pos.x > -8) && (dummy->pos.z < -1.2 && dummy->pos.z > -4.5)) || ((dummy->pos.x < -2.5 && dummy->pos.x > -8) && (dummy->pos.z > 4.55 && dummy->pos.z < 7));
	}

	bool isInMiddleofRamp05(Geometry* dummy, Geometry* ramp05) {
		return (dummy->pos.x < 15.75 && dummy->pos.x > 9.3) && (dummy->pos.z < 20 && dummy->pos.z > 12.8);
	}

	bool onLedgesofRamp05(Geometry* dummy, Geometry* ramp05) {
		return ((dummy->pos.x < 10.8 && dummy->pos.x > 7.25) && (dummy->pos.z < 20 && dummy->pos.z > 16.3)) 
			|| ((dummy->pos.x < 15 && dummy->pos.x > 13.4) && (dummy->pos.z < 17 && dummy->pos.z > 12.8));
	}

	bool isInMiddleofRamp06(Geometry* dummy, Geometry* ramp06) {
		return (dummy->pos.x < 7.96 && dummy->pos.x > 3.3) && (dummy->pos.z <-16.3 && dummy->pos.z > ramp06->gMax.z - 1);
	}

	bool onLedgesofRamp06(Geometry* dummy, Geometry* ramp06) {
		return ((dummy->pos.x < 3.3 && dummy->pos.x > 0.83) && (dummy->pos.z < -16.3 && dummy->pos.z > ramp06->gMax.z - 1))
			|| ((dummy->pos.x < 10.55 && dummy->pos.x > 8.5) && (dummy->pos.z < -16.3 && dummy->pos.z > ramp06->gMax.z - 1))
			|| ((dummy->pos.x < 8.5 && dummy->pos.x > 3.3) && (dummy->pos.z < -14.3 && dummy->pos.z > -16.3));
	}

	bool isBackwardCollision(Camera* camera) {
		return (camera->getEye().z - radius < -GROUND_SIZE + 2 || camera->getEye().z + radius > GROUND_SIZE - 2) || (camera->getEye().x - radius < -GROUND_SIZE + 2 || camera->getEye().x + radius > GROUND_SIZE - 2);
	}
	bool isEdgeCollision(Geometry* dummy) {
		return (dummy->pos.z - radius < -GROUND_SIZE + 2.3 || dummy->pos.z + radius> GROUND_SIZE - 2.3) || (dummy->pos.x - radius < -GROUND_SIZE + 2.4 || dummy->pos.x + radius> GROUND_SIZE - 2.4);
	}

	void updateCamera(shared_ptr<MatrixStack>& V, shared_ptr<MatrixStack>& Model, shared_ptr<Program> prog) {
		prog->bind();
		// Camera
		vec3 gaze = normalize(camera.getLookAt() - camera.getEye());
		vec3 w = -gaze / glm::length(gaze);
		vec3 u = glm::cross(camera.getUp(), w);
		vec3 v = glm::cross(w, u);
		
		if ((left || right) && !isEdgeCollision(&dummy)) {
			camera.incrementEye(-speed * w);
			camera.incrementLookAt(-speed * w);
			deltaLookAt += -speed * w;
		}

		if (backward && !isBackwardCollision(&camera)) {
			camera.incrementEye(speed * w);
			camera.incrementLookAt(speed * w);
			deltaLookAt += speed * w;
		}

		float x = cos(g_phi) * cRad * cos(g_theta) + deltaLookAt.x;
		float y = sin(g_phi) + deltaLookAt.y;
		float z = cos(g_phi) * cRad * cos((glm::pi<float>() / 2.0) - g_theta) + deltaLookAt.z;

		camera.setLookAt(x, y, z);
		dummy.pos = vec3(x, y, z);
		dummy.gMin = vec3(x, y, z);
		dummy.gMax = vec3(x, y, z);

		auto View = glm::lookAt(camera.getEye(), camera.getLookAt(), camera.getUp());
		V->lookAt(camera.getEye(), camera.getLookAt(), camera.getUp());

		prog->unbind();
	}

	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		auto ident = make_shared<MatrixStack>();

		//update the camera position
		updateUsingCameraPath(frametime);

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// move
		updateCamera(V, Model, prog);

		// check Ramp Collisions
		if (isRamp01Collision(&dummy, &ramp01)) {
			model_height = std::max(0.8, abs((sin(t)) * 1.5));
		}
		else if (isRamp02Collision(&dummy, &ramp02)) {
			model_height = std::max(0.8, abs((sin(t)) * 1.5));
		}
		else if (isRamp03Collision(&dummy, &ramp03)) {
			model_height = std::max(0.8, abs((sin(t)) * 1.5));
		}
		else if (onLedgesofRamp04(&dummy, &ramp04)) {
			model_height = std::max(0.3, abs((sin(t)) * 0.9)); // 0.6
		}
		else if (isInMiddleofRamp04(&dummy, &ramp04)) {
			model_height = 0.88;
		}
		else if (onLedgesofRamp05(&dummy, &ramp05)) {
			model_height = std::max(0.5, abs((sin(t)) * 0.83));
		}
		else if (isInMiddleofRamp05(&dummy, &ramp05)) {
			model_height = 0.73;
		}
		else if (onLedgesofRamp06(&dummy, &ramp06)) {
			model_height = std::max(0.65, abs((sin(t)) * 0.9));
		}
		else if (isInMiddleofRamp06(&dummy, &ramp06)) {
			model_height = 0.73;
		}
		else {
			model_height = -0.1f;
		}

		// skating animation
		if (left) {
			t += interpolate;
			drawSkateLeft(Projection, Model, prog, abs(sin(t)), model_height);
		}
		else if (right) {
			t += interpolate;
			drawSkateRight(Projection, Model, prog, abs(sin(t)), model_height);
		}
		else if (backward) {
			t += interpolate;
			drawSkateBackward(Projection, Model, prog, abs(sin(t)), model_height);
		}
		else {
			t = 0;
			drawDummy(Projection, Model, prog, model_height);
		}

		// draw skybox
		drawSkyBox(ident, Projection, V);

		// draw palm trees using instancing
		drawPalmTrees(instanceProg, Projection, V);

		// Draw Ramps
		drawRamps(prog, Projection, V, Model);

		// Draw the rink floor
		drawRink(texProg, Projection);

		// Draw coins
		drawCoins(prog, Projection, V, Model);

		// Pop matrix stacks.
		Projection->popMatrix();
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	// This is to play sound. Commented out so application can run on iOS
	//bool played = PlaySound("CanYouFeelTheForce.wav", NULL, SND_ASYNC);

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);
	application->cubeMapTexture = application->createSky("../resources/cracks/", application->faces);

	auto lastTime = chrono::high_resolution_clock::now();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
