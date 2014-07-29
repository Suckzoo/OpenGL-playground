#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>
#include "EventHandler.h"
#include "Object.h"
using namespace glm;



KeyEventHandler keyEvents;
MouseEventHandler mouseEvents;

void testFunction() {
	printf("J key pressed, yah!\n");
}
void testMouseButton() {
	printf("Mouse onClick()\n");
}
void testDrag(double oX, double oY, double X, double Y) {
	printf("Mouse onDrag() : origin at (%lf, %lf), now at(%lf, %lf)\n",oX,oY,X,Y);

}
void testRelease() {
	printf("Mouse onRelease()\n");
}
void doKeyFunction(GLFWwindow* window, int key, int scancode, int action, int mod) {
	keyEvents(key);
}
void doClickFunction(GLFWwindow* window, int button, int action, int mods) {
	mouseEvents(action);
}
void doDragFunction(GLFWwindow* window, double xpos,double ypos) {
	if(mouseEvents.isClicked()) {
		mouseEvents(xpos,ypos);
	}
}
void ScreenPosToWorldRay(
	int mouseX, int mouseY,
	int screenWidth, int screenHeight,
	glm::mat4 ViewMatrix,
	glm::mat4 ProjectionMatrix,
	glm::vec3& out_origin,
	glm::vec3& out_direction
) {

}
int main( void )
{
	//srand(time(0));
	//initialise bullet
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "nodap", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("VertexShader.glsl","FragShader.glsl");

	Object* cubeObject = new Object("suzanne.obj",2);
	dynamicsWorld->addRigidBody(cubeObject->getRigidBody());
	cubeObject->setProgram(programID);

	GLuint Texture = loadDDS("uvmap.DDS");


	glm::mat4 Projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0,0,-20),glm::vec3(0,0,0),glm::vec3(0,1,0));

	keyEvents.registerKeyboardFunction((int)'J',testFunction);
	mouseEvents.registerClick(testMouseButton);
	mouseEvents.registerDrag(testDrag);
	mouseEvents.registerRelease(testRelease);
	glfwSetKeyCallback(window,doKeyFunction);
	glfwSetMouseButtonCallback(window,doClickFunction);
	glfwSetCursorPosCallback(window,doDragFunction);
	glfwSetScrollCallback(window,doDragFunction);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	GLuint VPMatrixID = glGetUniformLocation(programID, "VP");
	GLuint ViewMatrix = glGetUniformLocation(programID, "V");
	GLuint lightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	do{
		dynamicsWorld->stepSimulation(1/60.f, 10);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		
		glm::mat4 VP = Projection*View;
		glUniformMatrix4fv(VPMatrixID, 1, GL_FALSE, &VP[0][0]);
		glUniformMatrix4fv(ViewMatrix, 1, GL_FALSE, &View[0][0]);
		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
		cubeObject->draw();

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

