#include <iostream>
#include <Windows.h>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <GL/glu.h>

#include "main.h"

GLuint tex;
char title[] = "3D Model Loader Sample";

int WIDTH = 1280;
int HEIGHT = 720;

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

wallPositions gameBounds{ -30, 30, -40, 40 };

MoveMode moveMode = PLAYER;

//camera vectors
Vector freeCamPosition;
Vector Eye;
Vector At;
Vector Up(0, 1, 0);
VIEWS currentView = VIEWS::FIRST;

//player
Vector playerPosition(-5, 0, 15);
DIRECTION playerEnumDirection = FRONT;
Vector playerVectorDirection(0, 0, -1);

// Global variables for lighting animations
float sunlightIntensity = 1.0f; // Intensity of the main sunlight
float bioluminescentPhase = 0.0f; // Animation phase for bioluminescent light
float sunlightAngle = 0.0f; // Angle to animate sunlight movement

Model_3DS tree_model;
GLTexture tex_ground;
Model_3DS model_fish;
Model_3DS model_bottle;
Model_3DS model_player;
Model_3DS model_collectable;

void setupSunlight() {
	// Sunlight properties
	GLfloat ambientLight[] = { 0.1f * sunlightIntensity, 0.2f * sunlightIntensity, 0.3f * sunlightIntensity, 1.0f };
	GLfloat diffuseLight[] = { 0.7f * sunlightIntensity, 0.8f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f };
	GLfloat specularLight[] = { 1.0f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f };
	GLfloat lightDirection[] = { -0.5f, -1.0f, -0.3f, 0.0f }; // Directional light

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
}

void setupBioluminescentLight() {
	// Oscillating bioluminescent light
	GLfloat bioluminescentColor[] = {
		0.3f + 0.2f * sin(bioluminescentPhase),
		0.7f + 0.1f * cos(bioluminescentPhase),
		1.0f,
		1.0f
	};
	GLfloat bioluminescentPosition[] = {
		1.0f * cos(bioluminescentPhase),
		0.5f,
		1.0f * sin(bioluminescentPhase),
		1.0f
	};

	glLightfv(GL_LIGHT1, GL_DIFFUSE, bioluminescentColor);
	glLightfv(GL_LIGHT1, GL_POSITION, bioluminescentPosition);
}

void setupAmbientLighting() {
	GLfloat globalAmbient[] = { 0.05f, 0.1f, 0.2f, 1.0f }; // Dim blue for underwater feel
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void updateLightingAnimation() {

	// Simulate sunlight fading with depth
	sunlightIntensity = 0.5f + 0.5f * cos(sunlightAngle);
	sunlightAngle += 0.05f;  // Reduced from 0.002f to 0.0005f
	if (sunlightAngle > 2 * 3.14159f) sunlightAngle -= 2 * 3.14159f;

	// Update bioluminescent light phase
	bioluminescentPhase += 0.002f;  // Reduced from 0.01f to 0.002f
	if (bioluminescentPhase > 2 * 3.14159f) bioluminescentPhase -= 2 * 3.14159f;

	glutPostRedisplay(); // Request redisplay for smooth animation
}

void setupLighting() {
	setupAmbientLighting();
	setupSunlight();
	setupBioluminescentLight();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); // Sunlight
	glEnable(GL_LIGHT1); // Bioluminescent light
}

void myInit(void)
{
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

bool isPlayerPositionValid(Vector newPosition) {
	if (newPosition.x < gameBounds.negativeX || newPosition.x > gameBounds.positiveX) return false;
	if (newPosition.z < gameBounds.negativeZ || newPosition.z > gameBounds.positiveZ) return false;

	return true;
}

void drawSkyBox() {
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();
}

void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glScaled(2, 0, 4);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void drawWalls() {
	// Draw Tree Model

	glPushMatrix();
	glTranslatef(-30, 0, 0);

	for (int i = 0; i < 10; i++) {
		glPushMatrix();
		glTranslatef(0, 0, 40 + i * -8);
		tree_model.Draw();
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 0, 0);

	for (int i = 0; i < 10; i++) {
		glPushMatrix();
		glTranslatef(0, 0, 40 + i * -8);
		tree_model.Draw();
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -40);

	for (int i = 0; i <= 10; i++) {
		glPushMatrix();
		glTranslatef(-25 + i * 5, 0, 0);
		tree_model.Draw();
		glPopMatrix();
	}
	glPopMatrix();
}

void movePlayer(char button) {
	Vector addVector(0, 0, 0);

	switch (button) {
	case 'w':
		addVector.z -= 4;
		playerEnumDirection = FRONT;
		playerVectorDirection.set(0, 0, -1);
		break;
	case 's':
		addVector.z += 4;
		playerEnumDirection = BACK;
		playerVectorDirection.set(0, 0, 1);
		break;
	case 'a':
		addVector.x -= 4;
		playerEnumDirection = LEFT;
		playerVectorDirection.set(-1, 0, 0);
		break;
	case 'd':
		addVector.x += 4;
		playerEnumDirection = RIGHT;
		playerVectorDirection.set(1, 0, 0);
		break;
	case 'q':
		addVector.y += 4;
		break;
	case 'e':
		addVector.y -= 4;
		break;
	}

	Vector newPosition = playerPosition + addVector;

	if (!isPlayerPositionValid(newPosition)) return;

	playerPosition.set(newPosition);
}

void moveCamera(char button) {
	Vector addVector(0, 0, 0);

	switch (button) {
	case 'w':
		addVector.z -= 4;
		break;
	case 's':
		addVector.z += 4;
		break;
	case 'a':
		addVector.x -= 4;
		break;
	case 'd':
		addVector.x += 4;
		break;
	case 'q':
		addVector.y += 4;
		break;
	case 'e':
		addVector.y -= 4;
		break;
	}

	freeCamPosition += addVector;
}

void drawAxes() {
	glPushAttrib(GL_CURRENT_BIT);

	glLineWidth(5);

	//y axis
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 7, 0);
	glEnd();

	//z axis
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 7);
	glEnd();

	//x axis
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(7, 0, 0);
	glEnd();

	glPopAttrib();
}

void drawCharacter() {

	glPushMatrix();
	glTranslated(playerPosition.x, playerPosition.y, playerPosition.z);
	glutWireCube(2);
	glPopMatrix();
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupLighting();

	glPushMatrix();

	drawAxes();

	RenderGround();

	drawWalls();

	drawCharacter();

	// Draw fish Model
	glPushMatrix();
	glTranslatef(0, 5, 0);
	glScalef(70.0, 70.0, 70);
	//model_fish.Draw(); // works
	glPopMatrix();

	// Draw bottle Model
	glPushMatrix();
	glTranslatef(0, 5, 0);
	glScalef(1, 1, 1);
	//model_bottle.Draw(); //works
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 5, 0);
	glScalef(10, 10, 10);
	//glRotatef(180.f, 0, 1, 0);
	//glRotatef(45.f, 0, 1, 0);
	model_player.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 0);
	glScaled(5, 5, 5);
	//model_collectable.Draw(); //works
	glPopMatrix();

	drawSkyBox();

	glPopMatrix();

	glutSwapBuffers();
}

void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
	case 's':
	case 'a':
	case 'd':
	case 'q':
	case 'e':
		moveMode == PLAYER ? movePlayer(button) : moveCamera(button);
		handleView();
		break;
	case 'y':
		moveMode == PLAYER ? moveMode = CAM : moveMode = PLAYER;
		break;
	case 'r':
		freeCamPosition.set(0, 10, 0);
		handleView();
		break;
	case 't':
		freeCamPosition.set(playerPosition);
		freeCamPosition.y += 15;
		freeCamPosition += playerVectorDirection * -25; //move camera behind player
		handleView();
		break;
	case 'i': //front multi-view
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-80, 80, -80, 80, -50, 50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
		break;
	case 'o': //top view
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-80, 80, -80, 80, -50, 50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 1, 0, 0, 0, 0, 0, 0, -1);
		break;
	case 'p': //side view
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-80, 80, -80, 80, -50, 50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(-1, 0, 0, 0, 0, 0, 0, 1, 0);
		break;
	case '/':
		myInit();
		break;
	case '[':
		Eye.printVector("Eye position");
		break;
	case ']':
		playerPosition.printVector("Player Position");
		break;
	case '1':
		updateView(FIRST);
		break;
	case '3':
		updateView(THIRD);
		break;
	case '2':
		updateView(FREE);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

void LoadAssets()
{
	tree_model.Load("Models/tree/Tree1.3ds");

	model_fish.Load("Models/fish2/fish2/Fish N130416.3ds");
	model_bottle.Load("Models/bottle/chembottle.3ds");
	model_player.Load("Models/ben10uncle/man.3ds");
	model_collectable.Load("Models/coin/uploads_files_233898_50ct.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

void updateView(VIEWS view) {
	currentView = view;
	handleView();
}

void handleView() {
	Vector cameraPosition(playerPosition);
	Vector cameraCenter(playerVectorDirection);

	if (currentView == FIRST) {
		cameraPosition.y += 10;
		cameraPosition += playerVectorDirection * 5; //move camera infront of player
		cameraCenter += cameraPosition; //set center infront of camera
	}
	else if (currentView == THIRD) {
		cameraPosition.y += 15;
		cameraPosition += playerVectorDirection * -25; //move camera behind player
		cameraCenter *= 5; //set center infront of camera
		cameraCenter.y -= 1;
		cameraCenter += cameraPosition;
	}
	else if (currentView == FREE) {
		cameraPosition.set(freeCamPosition);
		cameraCenter.set(playerPosition);
	}

	Eye = cameraPosition;
	At = cameraCenter;

	myInit();
}

void gameLoop(int value) {
	myDisplay();
	glutTimerFunc(16, gameLoop, 0);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // Deep ocean blue background

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);
	glutIdleFunc(updateLightingAnimation); // Idle function for light animation
	glutKeyboardFunc(myKeyboard);
	glutReshapeFunc(myReshape);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	//first time calls
	handleView();

	glutTimerFunc(16, gameLoop, 0);
	glutMainLoop();
}
