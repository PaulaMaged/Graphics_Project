#pragma once

#include <iostream>
#include <string>
#include "glut.h"

enum MoveMode {
	CAM,
	PLAYER
};

enum DIRECTION {
	LEFT,
	RIGHT,
	FRONT,
	BACK,
};

enum VIEWS {
	FIRST,
	THIRD,
	FREE
};

class Vector
{
public:
	GLdouble x, y, z;
	Vector() : x(0), y(0), z(0) {}
	Vector(const Vector& vector) {
		this->x = vector.x;
		this->y = vector.y;
		this->z = vector.z;
	}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}

	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
	void operator +=(Vector vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;
	}
	void operator *=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
	}
	void operator *=(Vector vector) {
		x *= vector.x;
		y *= vector.y;
		z *= vector.z;
	}
	Vector operator +(Vector vector) {
		Vector newVector;
		newVector.x = x + vector.x;
		newVector.y = y + vector.y;
		newVector.z = z + vector.z;
		return newVector;
	}
	Vector operator *(Vector vector) {
		Vector newVector;
		newVector.x = x * vector.x;
		newVector.y = y * vector.y;
		newVector.z = z * vector.z;
		return newVector;
	}
	Vector operator *(float value) {
		Vector newVector;
		newVector.x = value * x;
		newVector.y = value * y;
		newVector.z = value * z;
		return newVector;
	}
	
	void set(Vector vector) {
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}
	void set(GLdouble x, GLdouble y, GLdouble z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void addVector(Vector vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;
	}
	void add(GLdouble x, GLdouble y, GLdouble z) {
		this->x += x;
		this->y += y;
		this->z += z;
	}
	void printVector(std::string label) {
		std::cout << label << "{\n"
			<< "X: " << x
			<< "\nY: " << y
			<< "\nZ: " << z
			<< "\n}" << std::endl;
	}
};

struct wallPositions {
	double negativeX;
	double positiveX;
	double negativeZ;
	double positiveZ;
};

extern wallPositions gameBounds;

extern int WIDTH;
extern int HEIGHT;


// 3D Projection Options
extern GLdouble fovy;
extern GLdouble aspectRatio;
extern GLdouble zNear;
extern GLdouble zFar;

// Camera Movement
extern GLfloat cameraX;
extern GLfloat cameraY;
extern GLfloat cameraZ;

//player state
extern Vector playerPosition;
extern DIRECTION playerDirection;

bool isPlayerPositionValid(Vector newPosition);
void handleView();
void updateView(VIEWS view);
void playSound(const char* soundFileName, bool force = 0);
