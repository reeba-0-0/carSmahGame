#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
using namespace tle;

struct Vector2
{
	float x;
	float y;
};

struct Vector3
{
	float x = 0;
	float y = 0;
	float z = 0;
};

struct PlayerStruct
{
	IModel* model;
	const float kCarSpeedMax = 15, kCarSpeedMin = 0.f, kRotationSpeed = 35;
	const float kOriginX = 0.0f, kOriginY = 0.0, kOriginZ = 0.0f;
	float speed = 4.0f; // player car speed for moving cars
	const float kPlayerRadius = 2.21361f;
	int initialGameScore = 0;
	int health = 100;
	int carsHit = 0;
	const int kMaxCarsHit = 8;
	Vector3 thrust, drag, momentum, oldMomentum;
	bool xAxis = false;
	bool zAxis = false;
	bool none = false;
	bool isAccelerating = false;
};

struct EnemyStruct
{
	IModel* model;
	IModel* ball;
	const int kInitialHealth = 1;
	const float kMinusSpeed = -1;
	const int kCarTimer = 15;
	int health = 1; // enemy car health to detect if a car has already been hit
	float minX, maxX, minY, maxY, minZ, maxZ; // model's bounding box
	float carSpeed = 8.f; // enemy car speed for moving cars
	float ballSpeed = 1.f;
	float minBallY = 2.f;
	float maxBallY = 3.f;
	const float kMovingBoundary = 30.f;
	bool isMoving = false;
	bool ballTimer = false;
	bool timer = false;
	float elapsedTime = 0;
	bool sideCollision = false;
	bool frontCollision = false;

	// bounding box values
	const float kStaticXBoundary = 0.946118f, kStaticZBoundary = 1.97237f;
	const float kMovingXBoundary = 1.05776, kMovingZBoundary = 2.13928;

};

struct TreesStruct
{
	IModel* model;
	// consts for trees 
	const int kTreesNum = 160;
	const float kPi = 3.1415926f;
	const float kTreesRadius = 50.f;
	const float kIncrementDegrees = 2.25f;
	const float kFullCircleDegrees = 360.f, kHalfCircleDegrees = 180.f, kOriginalTreesDegrees = 0.f;
	const float kCreateCircle = 0.2f;
};

struct Particle
{
	IModel* model;
	Vector3 movementVector;
	bool isEmitted = false;
	float previousSpeed = 0.0f;
};
