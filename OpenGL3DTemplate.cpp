#include <Windows.h>
#include <glut.h>
#include <GL/glu.h>
#include <cmath>

// Global variables for animations
float sunlightIntensity = 1.0f; // Intensity of the main sunlight
float bioluminescentPhase = 0.0f; // Animation phase for bioluminescent light
float sunlightAngle = 0.0f; // Angle to animate sunlight movement

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
    sunlightAngle += 0.0005f;  // Reduced from 0.002f to 0.0005f
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

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 640 / 480, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1.5, 1.5, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up lights and camera
    setupLighting();
    setupCamera();

    // Sample scene objects
    glPushMatrix();
    glTranslated(0.4, 0.0, 0.4);
    glutSolidTeapot(0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.4, 0.0, -0.4);
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.0, 0.0, 0.0);
    glRotated(45, 1, 1, 0);
    glutSolidCube(0.5);
    glPopMatrix();

    glFlush();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Treasure Hunt: Under The Sea");
    glutDisplayFunc(Display);
    glutIdleFunc(updateLightingAnimation); // Idle function for light animation

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // Deep ocean blue background

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
