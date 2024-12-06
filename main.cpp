#include <iostream>

#include <Windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <GL/glu.h>
#include <chrono>

#include "main.h"

GLuint tex;
char title[] = "3D Model Loader Sample";

int WIDTH = 1280;
int HEIGHT = 720;

//level number
LEVELS currentLevel = COLLECT;

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;

wallPositions gameBounds{ -30, 30, -40, 40 };

MoveMode moveMode = PLAYER;

//camera vectors
Vector freeCamPosition(-1, 55, 80);
Vector Eye;
Vector At;
Vector Up(0, 1, 0);
VIEWS currentView = VIEWS::FREE;

//player
Vector playerPosition(-5, 5, 15);
DIRECTION playerEnumDirection = FRONT;
Vector playerVectorDirection(0, 0, -1);

// Global variables for lighting animations
float sunlightIntensity = 1.0f; // Intensity of the main sunlight
float bioluminescentPhase = 0.0f; // Animation phase for bioluminescent light

//game status
float score = 0.0f;
float elapsedTime = 0.0f;
int lastUpdateTime = 0;

Model_3DS model_pearl;
Model_3DS tree_model;
GLTexture tex_ground;
Model_3DS model_fish;
Model_3DS model_bottle;
Model_3DS model_player;
Model_3DS model_collectable;
Model_3DS model_seaweed;
Model_3DS model_anchor;
Model_3DS model_pathA;
Model_3DS model_shell;
Model_3DS model_rock;
Model_3DS model_diver;
Model_3DS model_star;
Model_3DS model_chest;
Model_3DS model_coral;

//mohamed
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.z, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f unit() {
		float mag = sqrt(x * x + y * y + z * z);
		return Vector3f(x / mag, y / mag, z / mag);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

Vector3f bottlePosition(-25, 5 ,25); // Example position of the bottle
float bottleRadius = 0.1f; // Define a radius for the bottle
float collisionRadius = 0.15f; // Define a collision radius for the player
bool bottleCollected = false; // Tracks if the bottle has been collected

float tolerance = 5;
bool checkCollisionWithBottle() {
	float dx = playerPosition.x - bottlePosition.x;
	float dz = playerPosition.z - bottlePosition.z;
	float dy = playerPosition.y - bottlePosition.y;
	float distance = sqrt(dx * dx + dz * dz + dy * dy);

	// Check if the distance is less than the sum of the radii
	return distance <= (collisionRadius + bottleRadius + tolerance);

}

Vector3f treasurePosition(0, 2, -60); // Example position of the bottle
float treasureRadius = 0.1f; // Define a radius for the bottle
bool treasureCollected = false; // Tracks if the bottle has been collected


bool checkCollisionWithtreasure() {
	float dx = playerPosition.x - treasurePosition.x;
	float dz = playerPosition.z - treasurePosition.z;
	float dy = playerPosition.y - treasurePosition.y;
	float distance = sqrt(dx * dx + dz * dz + dy * dy);

	// Check if the distance is less than the sum of the radii
	return distance <= (collisionRadius + treasureRadius + tolerance);

}


Vector3f CoinPosition(-25, 5, -25); // Example position of the bottle
float CoinRadius = 0.1f; // Define a radius for the bottle
bool CoinCollected = false; // Tracks if the bottle has been collected


bool checkCollisionWithCoin() {
	float dx = playerPosition.x - CoinPosition.x;
	float dz = playerPosition.z - CoinPosition.z;
	float dy = playerPosition.y - CoinPosition.y;
	float distance = sqrt(dx * dx + dz * dz + dy * dy);

	// Check if the distance is less than the sum of the radii
	return distance <= (collisionRadius + CoinRadius + tolerance);

}

Vector3f FishPosition(25, 5, 25); // Example position of the bottle
float FishRadius = 0.1f; // Define a radius for the bottle
bool FishCollected = false; // Tracks if the bottle has been collected


bool checkCollisionWithFish() {
	float dx = playerPosition.x - FishPosition.x;
	float dz = playerPosition.z - FishPosition.z;
	float dy = playerPosition.y - FishPosition.y;
	float distance = sqrt(dx * dx + dz * dz + dy * dy);

	// Check if the distance is less than the sum of the radii
	return distance <= (collisionRadius + FishRadius + tolerance);

}

//mohamed
void setupSunlight() {
	// Sunlight properties
	GLfloat ambientLight[] = { 0.1f * sunlightIntensity, 0.2f * sunlightIntensity, 0.3f * sunlightIntensity, 1.0f };
	GLfloat diffuseLight[] = { 0.7f * sunlightIntensity, 0.8f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f };
	GLfloat specularLight[] = { 1.0f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f * sunlightIntensity, 1.0f };
	GLfloat lightDirection[] = { 0, 1, 0, 0.0f }; // Directional light
	GLfloat spotDirection[] = {0, -1, 0};
	GLfloat spotCutOff[] = { 10.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutOff);
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
	sunlightIntensity = 0.05f + 0.5f * cos((2 * M_PI * elapsedTime) / 10);

	// Update bioluminescent light phase
	bioluminescentPhase = (2 * M_PI * elapsedTime / 2);  // Reduced from 0.01f to 0.002f
}

void setupLighting() {
	setupSunlight();
	setupBioluminescentLight();
}

void renderBitmapString(float x, float y, void* font, const char* string) {
	const char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void updateGameState() {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;

	// Update elapsed time
	elapsedTime += deltaTime;

	// Optional: Add game logic here for score updates
	// For example:
	// score += someGameLogic();

	lastUpdateTime = currentTime;
}

void renderTextOverlay() {
	// Save current matrix states
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Set up orthographic projection
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluOrtho2D(0, viewport[2], 0, viewport[3]);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Disable depth testing and lighting for text
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// Set text color to white
	glColor3f(1.0f, 1.0f, 1.0f);

	// Prepare text buffers
	char scoreText[50], timeText[50];
	sprintf(scoreText, "Score: %.2f", score);
	sprintf(timeText, "Time: %.2f seconds", elapsedTime);

	// Render text
	renderBitmapString(10, viewport[3] - 30, GLUT_BITMAP_HELVETICA_18, scoreText);
	renderBitmapString(10, viewport[3] - 60, GLUT_BITMAP_HELVETICA_18, timeText);

	// Restore OpenGL states
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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
	return true;
}

void drawSkyBox() {
	glPushMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glRotated(90, 1, 0, 1);
	glScaled(2, 2, 2);
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
	glScaled(5, 5, 5);
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

	playSound("Swim-shortened");
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

void faceDirection() {
	GLdouble angle = 0;
	Vector direction(0, 1, 0);

	switch (playerEnumDirection) {
	case FRONT:
		break;
	case LEFT:
		angle = 90;
		break;
	case RIGHT:
		angle = -90;
		break;
	case BACK:
		angle = 180;
		break;
	}

	glRotated(angle, direction.x, direction.y, direction.z);
}

void drawCharacter() {

	glPushMatrix();
	glTranslated(playerPosition.x, playerPosition.y, playerPosition.z);
	glRotated(180, 0, 1, 0);
	faceDirection();
	glScaled(0.02, 0.02, 0.02);
	model_player.Draw();
	glPopMatrix();
}

float fishPositions[][3] = {
	{4, 4, 0},    // Fish 1
	{6, 4.5, -2}, // Fish 2
	{3, 3.5, 2},  // Fish 3
	{5, 4, 3},    // Fish 4
	{7, 5, -1}    // Fish 5
};
float fishScales[] = { 70.0, 65.0, 75.0, 72.0, 68.0 }; // Slightly different sizes
float fishSpeeds[] = { 0.01f, 0.015f, 0.02f, 0.013f, 0.018f }; // Different swim speeds
float fishAngles[] = { 0, 0, 0, 0, 0 }; // Keep track of each fish's angle

double getTime() {
	static auto start = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration<double>(now - start).count();
}

void drawSchoolOfFish() {
	for (int i = 0; i < 5; ++i) {
		glPushMatrix();

		// Fish-specific movement: add unique animation offsets
		float waveOffset = sin(getTime() * fishSpeeds[i]) * 0.5f;

		glTranslatef(fishPositions[i][0] + waveOffset, fishPositions[i][1], fishPositions[i][2]);
		glRotatef(fishAngles[i], 0, 1, 0); // Rotate fish to "swim"
		glScalef(fishScales[i], fishScales[i], fishScales[i]);

		// Update angles for continuous swimming motion
		fishAngles[i] += fishSpeeds[i] * 50.0f;
		if (fishAngles[i] > 360.0f) fishAngles[i] -= 360.0f;

		// Draw each fish
		model_fish.Draw();

		glPopMatrix();
	}
}

float grassPositions[][3] = {
	{-28, 0, 6},    // Fish 1
	{-29, 0, 10}, // Fish 2
	{-26, 0, 8},  // Fish 3
	{-30, 0, 27},    // Fish 4
	{-33, 0, 25}    // Fish 5
};

void drawSeaWeeds() { 
	for (int i = 0; i < 5; ++i) {
		glPushMatrix();

		glScalef(0.5, 0.5, 0.5);
		glTranslatef(grassPositions[i][0], grassPositions[i][1], grassPositions[i][2]);

		// Calculate bioluminescence intensity
		float time = getTime(); // Replace with getTime() if not using GLFW
		float intensity = (sin(time * 2.0f) + 1.0f) * 0.5f; // Oscillates between 0.0 and 1.0

		// Set emissive material properties for bioluminescent effect
		GLfloat emissionColor[] = { 0.0f, 0.8f * intensity, 0.2f * intensity, 1.0f }; // Green glow
		glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);

		// Draw the seaweed model
		model_seaweed.Draw();

		// Reset emission to default (no glow)
		GLfloat defaultEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmission);

		glPopMatrix();
	}
}

//level2 rock pathway, can have some coins placed in between
//optional tbh, if not used place the coins in the cave instead
void drawRocksWithGap() {
	// Draw the first rock
	glPushMatrix();
	glTranslatef(0, 1, -50); // Position for the first anchor
	glRotatef(180.0f, 0, 0, 1); // Orientation for the anchor
	glScalef(0.2, 0.2, 0.2); // Scale for the anchor
	model_pathA.Draw();
	glPopMatrix();

	// Draw the second rock
	glPushMatrix();
	glTranslatef(0, 1, -70); // Position for the second anchor (relative gap)
	glRotatef(180.0f, 0, 0, 1); // Orientation for the anchor
	glScalef(0.2, 0.2, 0.2); // Scale for the anchor
	model_pathA.Draw();
	glPopMatrix();
}

//scaled down for level 1, collectible, scale it up to be a big treasure chest for level 2
void drawTreasureChest(){
	glPushMatrix();
	glTranslatef(0, 2, -60);
	glScalef(2, 2, 2);
	model_chest.Draw();
	glPopMatrix();
}

//shell "clam" collectible placed randomly for level1
//model currently not working "dive.3ds"
void drawShell(){

	glPushMatrix();
	glTranslatef(0, 0, 16); // Shell 
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(17, 0, 18); 
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(11, 0, 0);
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(2, 0, 6); 
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-5, 0, -4); 
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix(); 
	
	glPushMatrix();
	glTranslatef(-9, 0, 2); 
	glScalef(20, 20, 20);
	model_shell.Draw();
	glPopMatrix(); 
}

//coral "tree" obstacle in level1
void drawCoral(double x, double y, double z){
	glPushMatrix();
	glTranslatef(x, y, z); 
	glRotatef(90.0f, 0, 1, 0);
	glScalef(0.8, 0.8, 0.8);
	model_coral.Draw();
	glPopMatrix();
}

//golden star, golden relic for level 2, collectible
void drawStar(){
	glPushMatrix();
	glTranslatef(4, 1, 0); 
	glRotatef(90.0f, 1, 0, 0);
	glScalef(0.003, 0.003, 0.003);
	model_star.Draw();
	glPopMatrix();
}

//two rocks in different sizes to appear like a cave, seaweed is placed at the entrance (already positioned in the drawseaweed method)
void drawCave(){
	glPushMatrix();
	glTranslatef(-17, 0, 4); // Seaweed 10
	glRotatef(90.0f, 0, 1, 0);
	glScalef(0.15, 0.15, 0.15);
	model_rock.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-17, 0, 4); // Seaweed 10
	glRotatef(90.0f, 0, 1, 0);
	glScalef(0.2, 0.2, 0.2);
	model_rock.Draw();
	glPopMatrix();
}

//level 1: 
//obstacles: coral tree
// fish for appearance
//collectible: shell, can draw a white sphere on top of shell to look like a pearl
//collectible: small treasure chest
//collectible: coins

//level 2:
//obstacles: cave, rocky pathway optional
//seaweed placed at cave entrance for appearance (and can be scattered everywhere too)
//collectible: treasure chest scaled up
//collectible: golder star
//collectible: gemstones (havent added it yet)

//To work: 
//	- sound format (.wav) 
//	- is inside of Sounds folder!
//Behavior:
// - Only plays once
// - main thread should not end to finish playing (Played in Async mode)
void playSound(const char *soundFileName, bool force) {
	std::string soundFilePath;
	
	soundFilePath += "Sounds/";
	soundFilePath += soundFileName;
	soundFilePath += ".wav";

	if (force) {
		PlaySound(soundFilePath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySound(soundFilePath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
	}
}

void displayStart() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glutSwapBuffers();
}

void displayCollect()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupLighting();

	glPushMatrix();

	drawAxes();

	RenderGround();

	drawRocksWithGap();

	
	//add collision to pearl (shaklaha zay el beda fyl le3ba)
	glPushMatrix();
	glTranslated(25, 1, -25);
	glScaled(10, 10, 10);
	model_pearl.Draw();
	glPopMatrix();

	drawCoral(CoinPosition.x, CoinPosition.y, CoinPosition.z); //working

	//drawTreasureChest(); //works

	drawCharacter();

	//bottle
	if (!bottleCollected && checkCollisionWithBottle()) {
		playSound("Pick-up", 1);
		bottleCollected = true; // Mark as collected
		score += 10.0f; // Increment score
	}


	// Draw bottle only if it hasn't been collected
	if (!bottleCollected) {
		glPushMatrix();
		glTranslatef(-25, 5, 25);
		glScalef(1, 1, 1);
		model_bottle.Draw();
		glPopMatrix();
	}

	//treasure
	if (!treasureCollected) {
		drawTreasureChest();
	}
	if (!treasureCollected && checkCollisionWithtreasure()) {
		playSound("Pick-up", 1);
		treasureCollected = true; // Mark as collected
		score += 10.0f; // Increment score
	}


	// Draw treasure only if it hasn't been collected
	if (!treasureCollected) {
		glPushMatrix();
		glTranslatef(0, 2, -60);
		glScaled(20, 20, 20);
		drawTreasureChest();//works
		glPopMatrix();
	}


	//coin
	if (!CoinCollected && checkCollisionWithCoin()) {
		playSound("Pick-up", 1);
		CoinCollected = true; // Mark as collected
		score += 10.0f; // Increment score
	}


	// Draw bottle only if it hasn't been collected
	if (!CoinCollected) {
		glPushMatrix();
		glTranslatef(-25, 5, -25);
		glScaled(20, 20, 20);
		model_collectable.Draw(); //works
		glPopMatrix();
	}

	
	//fish
	// Fish collision and rendering
	if (!FishCollected && checkCollisionWithFish()) {
		playSound("Pick-up", 1);
		FishCollected = true; // Mark as collected
		score += 10.0f; // Increment score
		std::cout << "Fish collected! Score: " << score << std::endl; // Debug message
	}

	if (!FishCollected) {
		// Draw fish Model
		glPushMatrix();
		glTranslatef(25, 5, 25);
		glScalef(70.0, 70.0, 70);
		model_fish.Draw(); // works
		glPopMatrix();
	}
	
	drawSeaWeeds();

	glPushMatrix();
	glTranslated(5, 1, 0);
	drawSchoolOfFish();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-15, 1, 0);
	drawSchoolOfFish();
	glPopMatrix();

	drawSkyBox();
	
	renderTextOverlay();
	
	glPopMatrix();

	glutSwapBuffers();
}

void displayHunt() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupLighting();

	glPushMatrix();

	drawAxes();

	RenderGround();

	drawCharacter();

	drawSkyBox();

	glPopMatrix();
	glutSwapBuffers();
}

void displayEnd() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 1, 0, 0);
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
	//**Switch Levels**
	case 'c':
		currentLevel = COLLECT;
		break;
	case 'h':
		currentLevel = HUNT;
		break;
	case 'C':
		currentLevel = START;
		break;
	case 'H': 
		currentLevel = END;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
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
	model_fish.Load("Models/fish2/fish2/Fish N130416.3ds");
	model_bottle.Load("Models/bottle/chembottle.3ds");
	model_player.Load("Models/Players/Diver_Scuba.3ds");
	model_collectable.Load("Models/coin/uploads_files_233898_50ct.3ds");
	model_seaweed.Load("Models/seaweed/Grass 2 N180822.3ds");
	model_pathA.Load("Models/rockpath/RockWalkway01.3ds");
	model_shell.Load("Models/fishies/dive.3ds");
	model_coral.Load("Models/fan/RS04v1.3ds");
	model_rock.Load("Models/rock/ArchTripple.3ds");
	model_chest.Load("Models/chest/chest.3ds");
	model_star.Load("Models/star/Star big.3ds");
	model_pearl.Load("Models/pearls/pearl.3ds");
	
	// Loading texture files
	tex_ground.Load("Textures/sand.bmp");
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

void setDisplayFunc() {
	switch (currentLevel) {
		case START:
			glutDisplayFunc(displayStart);
			break;
		case COLLECT:
			glutDisplayFunc(displayCollect);
			break;
		case HUNT:
			glutDisplayFunc(displayHunt);
			break;
		case END:
			glutDisplayFunc(displayEnd);
			break;
		default:
			glutDisplayFunc(displayStart);
	}
}

void gameLoop(int value) {
	updateGameState();
	setDisplayFunc();
	glutPostRedisplay();
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

	setDisplayFunc();
	glutIdleFunc(updateLightingAnimation); // Idle function for light animation
	glutKeyboardFunc(myKeyboard);
	glutReshapeFunc(myReshape);

	lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
	
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	setupAmbientLighting();
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
