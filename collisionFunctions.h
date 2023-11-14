#pragma once
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
#include "structs.h"

enum ECollisionAxis { xAxis, zAxis, none };

const int kFrontCollisionScore = 10;
const int kSideCollisionScore = 15;
const float kScaleEnemyDown = 0.8f;

// check player car to enemy car collision using bounding box
bool CheckPlayerToEnemyCollision(PlayerStruct &player, EnemyStruct &enemy, float xBoundary, float zBoundary)
{
	bool collision = false;
	// get player positions
	float playerXPos = player.model->GetX();
	float playerZPos = player.model->GetZ();

	// get enemy car min and max using current position and bounding box values
	enemy.minX = enemy.model->GetX() - xBoundary;
	enemy.maxX = enemy.model->GetX() + xBoundary;

	enemy.minZ = enemy.model->GetZ() - zBoundary;
	enemy.maxZ = enemy.model->GetZ() + zBoundary;

	// check player position in relation to enemy min and max and player radius
	if (playerXPos > enemy.minX - player.kPlayerRadius && playerXPos < enemy.maxX + player.kPlayerRadius &&
		playerZPos > enemy.minZ - player.kPlayerRadius && playerZPos < enemy.maxZ + player.kPlayerRadius)
	{
		collision = true;
	}

	player.oldMomentum = player.momentum; // reset momentum so car can move

	return collision;
}

void CollisionResolution(Vector3 old, PlayerStruct &player, EnemyStruct &enemy, float frameTime) 
{
	player.model->SetLocalPosition(old.x, old.y, old.z); // reset car position
	player.oldMomentum = { 0,0,0 }; // reset old momentum
	//player.model->Move(player.momentum.x * frameTime, player.momentum.y * frameTime, player.momentum.z * frameTime);
	enemy.ball->SetSkin("redball.jpg"); // change ball colour
	enemy.carSpeed = 0; // stop moving cars
	enemy.ballSpeed = 0; // stop moving balls
}

void CheckStaticCollision(PlayerStruct &player, EnemyStruct enemy) // use vectors to find out if there's been a side or front collision for static cars
{
	// get enemy to player x,y and x vectors
	float vectorX = enemy.model->GetX() - player.model->GetX();
	float vectorY = enemy.model->GetY() - player.model->GetY();
	float vectorZ = enemy.model->GetZ() - player.model->GetZ();

	float frontRearCollision = sqrt(vectorX * vectorX + vectorY * vectorY);
	float sideCollision = sqrt(vectorY * vectorY + vectorZ * vectorZ);

	if (frontRearCollision < player.kPlayerRadius)
	{
  		player.initialGameScore += kFrontCollisionScore; // increment score by 10
		ScalingEnemiesZ(enemy, kScaleEnemyDown); // scale down by 0.8
 	}

	else if (sideCollision < player.kPlayerRadius)
	{
		player.initialGameScore += kSideCollisionScore;
		ScalingEnemiesX(enemy, kScaleEnemyDown); // scale down by 0.8
 	}
}

void CheckMovingCollision(PlayerStruct &player, EnemyStruct &enemy)  // use vectors to find out if there's been a side or front collision for moving cars
{
 	float vectorX = enemy.model->GetX() - player.model->GetX();
	float vectorY = enemy.model->GetY() - player.model->GetY();
	float vectorZ = enemy.model->GetZ() - player.model->GetZ();

	float sideCollision = sqrt(vectorX * vectorX + vectorY * vectorY);
	float frontRearCollision = sqrt(vectorY * vectorY + vectorZ * vectorZ);

	if (sideCollision < player.kPlayerRadius)
	{
		player.initialGameScore += kSideCollisionScore; // increment score by 10
		player.momentum = player.oldMomentum;
 	}

	else if (frontRearCollision < player.kPlayerRadius)
	{
		player.initialGameScore += kFrontCollisionScore;
		player.momentum = player.oldMomentum;
	}

}

void BounceBack(EnemyStruct enemy, PlayerStruct &player) // bounce back based on front/side collisoin
{
	if (enemy.health == 0)
	{
		player.momentum.x *= enemy.kMinusSpeed; // change x momentum if it hits the z axis
	}

	if (enemy.health == 0)
	{
		player.momentum.z *= enemy.kMinusSpeed; // change z momentum if it hits the x axis
	}
}

bool CheckPlayerToTreesCollision(PlayerStruct player, TreesStruct trees) // check collision using vector between trees and enemy
{
	float vectorX = trees.model->GetX() - player.model->GetX();
	float vectorZ = trees.model->GetZ() - player.model->GetZ();

	float collision = sqrt(vectorX * vectorX + vectorZ * vectorZ);

	return (collision < player.kPlayerRadius); // return true if the vector is less than the player car radius

}

