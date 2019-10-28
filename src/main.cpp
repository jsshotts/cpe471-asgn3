/*
ZJ Wood CPE 471 Lab 3 base code
*/
#define _USE_MATH_DEFINES
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = glm::vec3(0, 0, -15);
		rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double frametime)
	{
		double ftime = frametime;
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{
public:
	int kn = 0;
	float globeRot = 0;
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, sphereProg, globeprog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID = 0;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID = 0, VertexColorIDBox = 0, IndexBufferIDBox = 0;

	Shape shape, globe;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;

		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			//mycam.a = 1;
			globeRot = 0.02;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
			globeRot = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			//mycam.d = 1;
			globeRot = -0.02;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
			globeRot = 0;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS) kn = 1;
		if (key == GLFW_KEY_N && action == GLFW_RELEASE) kn = 0;
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	void mkClinder(shared_ptr<vector<GLfloat>> &verticies, shared_ptr<vector<GLfloat>> &colors, int numFaces)
	{
		float x, x2, y, y2, r = 1;
		vec3 faceClr = { 1.0, 0, 0 };
		vec3 sideClr = { 0, 0, 1.0 };
		verticies = make_shared<vector<GLfloat>>(); // x, y, z make a Point
		colors = make_shared<vector<GLfloat>>();    // r, g, b for one Point
		
		for (double t = 0, t2 = -(2 * M_PI) / numFaces; t >= -2 * M_PI; t=t2, t2 -= (2 * M_PI) / numFaces)
		{
			x = r * cos(t); y = r * sin(t);
			x2 = r * cos(t2); y2 = r * sin(t2);
			//front
			verticies->push_back(0), verticies->push_back(0), verticies->push_back(1);
			verticies->push_back(x2), verticies->push_back(y2), verticies->push_back(1);
			verticies->push_back(x), verticies->push_back(y), verticies->push_back(1);
			clr(colors, faceClr);
			//sides
			verticies->push_back(x2), verticies->push_back(y2), verticies->push_back(1);
			verticies->push_back(x), verticies->push_back(y), verticies->push_back(-1);
			verticies->push_back(x), verticies->push_back(y), verticies->push_back(1);
			clr(colors, sideClr);
			verticies->push_back(x2), verticies->push_back(y2), verticies->push_back(-1);
			verticies->push_back(x), verticies->push_back(y), verticies->push_back(-1);
			verticies->push_back(x2), verticies->push_back(y2), verticies->push_back(1);
			clr(colors, sideClr);
			//back
			verticies->push_back(x), verticies->push_back(y), verticies->push_back(-1);
			verticies->push_back(x2), verticies->push_back(y2), verticies->push_back(-1);
			verticies->push_back(0), verticies->push_back(0), verticies->push_back(-1);
			clr(colors, faceClr);
		}
	}
	void clr(shared_ptr<vector<GLfloat>>& colors, vec3 &clr) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				colors->push_back(clr[j]);
		}
	}

	/* just for debugging */
	void printBuffers(shared_ptr<vector<GLfloat>>& verticies,
		shared_ptr<vector<GLfloat>>& colors)
	{
		int v = 1, t = 1;
		for (auto itr = verticies->begin(); itr != verticies->end(); itr++, v++)
		{
			cout << "(" << *itr << ", ";
			itr++;
			cout << *itr << ", ";
			itr++;
			cout << *itr << ")";
			if (v % 3 == 0)
			{
				t++;
				cout << endl;
			}
			else
				cout << ", ";
			if (t % 5 == 0)
			{
				t = 1;
				cout << endl;
			}
		}
	}

	void initClyinder() {
		shared_ptr<vector<GLfloat>> verticies, colors;
		mkClinder(verticies, colors, 80);
		//VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		
		//verticies
		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticies->size(), verticies->data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		glGenBuffers(1, &VertexColorIDBox);
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors->size(), colors->data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		initClyinder();

		//init sphere
		string resourceDirectory = "../resources";
		shape.loadMesh(resourceDirectory + "/sphere.obj");
		shape.resize();
		shape.init();

		//init globe
		globe.loadMesh(resourceDirectory + "/highResSphere.obj");
		globe.resize();
		globe.init();
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		// Enable blending/transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		// cylinders.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertColor");
		// sphere.
		sphereProg = std::make_shared<Program>();
		sphereProg->setVerbose(true);
		sphereProg->setShaderNames(resourceDirectory + "/shape_vertex.glsl", resourceDirectory + "/shape_fragment.glsl");
		if (!sphereProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
		}
		sphereProg->addUniform("P");
		sphereProg->addUniform("V");
		sphereProg->addUniform("M");
		sphereProg->addAttribute("vertPos");
		sphereProg->addAttribute("vertNor");
		sphereProg->addAttribute("vertTex");

		// globe.
		globeprog = std::make_shared<Program>();
		globeprog->setVerbose(true);
		globeprog->setShaderNames(resourceDirectory + "/globe_vertex.glsl", resourceDirectory + "/globe_fragment.glsl");
		if (!globeprog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		globeprog->addUniform("P");
		globeprog->addUniform("V");
		globeprog->addUniform("M");
		globeprog->addUniform("camPos");
		globeprog->addAttribute("vertPos");
		globeprog->addAttribute("vertNor");
		globeprog->addAttribute("vertTex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	float approach(float a, float n)
	{
		if (abs(a - n) < 0.00001)
			return n;
		if (a < n)
			a += 0.01;
		else if (a > n)
			a -= 0.01;
		return a;
	}

	void render()
	{
		double frametime = get_last_elapsed_time();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		// Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		static float rot = 0;
		rot += globeRot;
		mat4 GR = rotate(mat4(1.0f), rot, vec3(0, 1, 0));
		static float w = 0, rt = w, lt = w, h = w;
		w += 0.01;
		rt = approach(rt, asin(sin(w)));
		lt = approach(lt, (M_PI-0.8)/2);
		h = approach(h, M_PI / 2.5);
		
		// spheres
		sphereProg->bind();
		glUniformMatrix4fv(sphereProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(sphereProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		//base
		mat4 S1 = scale(mat4(1.0f), vec3(1, 1, 1));
		mat4 TP1 = translate(mat4(1.0f), vec3(0, -1, 0));
		mat4 M1 = TP1 * GR;
		M = M1 * S1;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//body
		mat4 S2 = scale(mat4(1.0f), vec3(0.8, 0.8, 0.8));
		mat4 TP2 = translate(mat4(1.0f), vec3(0, 1, 0));
		mat4 M2 = M1 * TP2;
		M = M2 * S2;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//leftShoulder
		mat4 S6 = scale(mat4(1.0f), vec3(0.5, 0.3, 0.3));
		mat4 TP6 = translate(mat4(1.0f), vec3(-0.6, 0.2, 0));
		mat4 M6 = M2 * TP6;
		M = M6 * S6;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//rightShoulder
		mat4 S9 = scale(mat4(1.0f), vec3(0.5, 0.3, 0.3));
		mat4 TP9 = translate(mat4(1.0f), vec3(0.6, 0.2, 0));
		mat4 M9 = M2 * TP9;
		M = M9 * S9;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//head
		mat4 S3 = scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
		mat4 R3 = rotate(mat4(1.0f), cos(h), glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 TP3 = translate(mat4(1.0f), vec3(0, 1, 0));
		mat4 M3 = M2 * TP3 * R3;
		M = M3 * S3;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//leftEye
		mat4 S4 = scale(mat4(1.0f), vec3(0.1, 0.1, 0.1));
		mat4 TP4 = translate(mat4(1.0f), vec3(-0.2, 0.1, 0.4));
		mat4 M4 = M3 * TP4;
		M = M4 * S4;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//rightEye
		mat4 S5 = scale(mat4(1.0f), vec3(0.1, 0.1, 0.1));
		mat4 TP5 = translate(mat4(1.0f), vec3(0.2, 0.1, 0.4));
		mat4 M5 = M3 * TP5;
		M = M5 * S5;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//leftarm
		mat4 S7 = scale(mat4(1.0f), vec3(0.15, 0.15, 0.5));
		mat4 R7 = rotate(mat4(1.0f), (float)M_PI / 2, vec3(0, 1, 0));
		mat4 TR7 = translate(mat4(1.0f), vec3(-0.5, 0, 0));
		mat4 R7P = rotate(mat4(1.0f), lt, vec3(0, 0, 1));
		mat4 TP7 = translate(mat4(1.0f), vec3(-0.3, 0, 0));
		mat4 M7 = M6 * TP7 * R7P * TR7 * R7;
		//lefthand
		mat4 S8 = scale(mat4(1.0f), vec3(0.2, 0.2, 0.2));
		mat4 TP8 = translate(mat4(1.0f), vec3(0, 0, -0.6));
		mat4 M8 = M7 * TP8;
		M = M8 * S8;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		//rightarm
		mat4 S10 = scale(mat4(1.0f), vec3(0.15, 0.15, 0.5));
		mat4 R10 = rotate(mat4(1.0f), (float)M_PI / 2, vec3(0, 1, 0));
		mat4 TR10 = translate(mat4(1.0f), vec3(0.5, 0, 0));
		mat4 R10P = rotate(mat4(1.0f), cos(rt), vec3(0, 0, 1));
		mat4 TP10 = translate(mat4(1.0f), vec3(0.3, 0, 0));
		mat4 M10 = M9 * TP10 * R10P * TR10 * R10;
		//righthand
		mat4 S11 = scale(mat4(1.0f), vec3(0.2, 0.2, 0.2));
		mat4 TP11 = translate(mat4(1.0f), vec3(0, 0, 0.6));
		mat4 M11 = M10 * TP11;
		M = M11 * S11;
		glUniformMatrix4fv(sphereProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(sphereProg);
		sphereProg->unbind();

		//cylinders
		prog->bind();
		glBindVertexArray(VertexArrayID);
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		//leftarm
		M = M7 * S7;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 10000);
		//rightarm
		M = M10 * S10;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 10000);

		glBindVertexArray(0);
		prog->unbind();

		// globe
		globeprog->bind();
		glUniformMatrix4fv(globeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(globeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform3f(globeprog->getUniform("camPos"), -mycam.pos.x, -mycam.pos.y, -mycam.pos.z);

		mat4 SSphere = scale(mat4(1.0f), vec3(5, 5, 5));
		M = GR * SSphere;
		glUniformMatrix4fv(globeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(globeprog); //swap to globe.draw(globeprog) for high res globe
		globeprog->unbind();
	}
};

//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
		resourceDir = argv[1];

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1400, 800);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
