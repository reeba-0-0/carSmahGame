#pragma once
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
#include "structs.h"

constexpr float kDragFactor = -0.001f;
constexpr EKeyCode kForwardKey = Key_W, kBackwardKey = Key_S, kClockwiseKey = Key_D, kAntiClockwiseKey = Key_A, kExitKey = Key_Escape;
constexpr float kRotateCars = 90.f;
constexpr float kTurnCars = 180.f;
constexpr int kNumEnemyCars = 4;
constexpr float kResetCarSpeed = 8.f;
constexpr float kResetBallSpeed = 1.f;
constexpr int kResetEnemyHealth = 1;
constexpr int kResetPlayerHealth = 100;

void MoveEnemiesAcrossX(EnemyStruct &enemy, float frameTime) // move enemy cars horizontally
{
	// move the cars vertically
	enemy.model->MoveLocalZ(enemy.carSpeed * frameTime);

	// set the speed to minus if the enemy car's x is more than 30 or less than -30
	if (enemy.model->GetLocalX() > enemy.kMovingBoundary || enemy.model->GetLocalX() < -enemy.kMovingBoundary)
	{
		enemy.carSpeed *= enemy.kMinusSpeed; // reverse the car
		enemy.isMoving = true; // set isAccelerating to true to track if the car has reached to the other end

		if (enemy.isMoving)
		{
			enemy.model->RotateLocalY(kTurnCars); // if it has then rotate the car by 180
			enemy.carSpeed *= enemy.kMinusSpeed; // reverse the car
		}
	}
}

void MoveBallsVertically(EnemyStruct &enemy, float frameTime) // make balls move up and down
{
	enemy.ball->MoveLocalY(enemy.ballSpeed * frameTime); // if it is then move the balls up and down
	
	if (enemy.ball->GetLocalY() > enemy.maxBallY || enemy.ball->GetLocalY() < enemy.minBallY) // move ball vertically between two points
	{
		enemy.ballSpeed *= enemy.kMinusSpeed; // reverse the ball
	}
}

void CreateTrees(TreesStruct& trees, float& treesDegrees)
{

	treesDegrees += trees.kIncrementDegrees; // increment degrees by 2.25 in each instance of the game loop
	float radians = trees.kPi / trees.kHalfCircleDegrees * treesDegrees; // convert degrees to radians by dividing pi by 180 * degrees
	float treesX = trees.kTreesRadius * cos(radians); // derive x coordinates using cos
	float treesZ = trees.kTreesRadius * sin(radians); // derive z coordinates using sin

	if (treesDegrees >= trees.kFullCircleDegrees)
	{
		treesDegrees = trees.kOriginalTreesDegrees; // set degrees back to 0 once it hits 360 (full circle)
	}
	trees.model->SetPosition(treesX, 0, treesZ); // update position of trees using derived x and z coords

}

// function to scale static enemy car down
void ScalingEnemiesX(EnemyStruct enemy, float scaleFactor) // pass in enemy model struct and scale factor as parameters
{
	enemy.model->ScaleX(scaleFactor);
}

// function to scale static enemy car down
void ScalingEnemiesZ(EnemyStruct enemy, float scaleFactor) // pass in enemy model struct and scale factor as parameters
{
	enemy.model->ScaleZ(scaleFactor);
}

void RotateWheels(ISceneNode* wheels, ISceneNode* wheels2, float frameTime, float rotationSpeed) // rotate wheel nodes depending on direction
{
	wheels->RotateLocalX(rotationSpeed * frameTime);
	wheels2->RotateLocalX(rotationSpeed * frameTime);
}

void RotateCars(EnemyStruct enemy[kNumEnemyCars]) // rotate cars by 90 degrees depending on their position
{
	enemy[0].model->RotateLocalY(kRotateCars);
	enemy[3].model->RotateLocalY(kRotateCars);
	enemy[1].model->RotateLocalY(-kRotateCars);
	enemy[2].model->RotateLocalY(-kRotateCars);
}

void MoveAfter15(PlayerStruct& player, EnemyStruct& enemy) // move enemy car 15 seconds after it's been hit
{
	enemy.elapsedTime = 0; // reset timer
	enemy.timer = false; // set timer to false
	enemy.ball->SetSkin("white.png"); 
	enemy.carSpeed = kResetCarSpeed; // make the car move again
	enemy.ballSpeed = kResetBallSpeed; // make the ball move again
	enemy.health = kResetEnemyHealth; // reset moving enemy car's health
	player.carsHit--; // reset how many cars have been hit

}

// function to restart game after game state = gameWon or gameOver
// pass in everything that needs to be reset as parameters
void RestartGame(PlayerStruct& player, EnemyStruct& staticEnemies, EnemyStruct& movingEnemies, ICamera* myCamera)
{
	player.model->SetLocalPosition(player.kOriginX, player.kOriginY, player.kOriginZ); // reset the player car's position


	staticEnemies.health = 1; // reset static enemy car's health
	staticEnemies.ball->SetSkin("white.png"); // static balls are set back to white
	staticEnemies.model->ResetScale();

	movingEnemies.health = 1; // reset moving enemy car's health
	movingEnemies.ball->SetSkin("white.png"); // moving balls are set back to white
	movingEnemies.carSpeed = kResetCarSpeed;
	movingEnemies.ballSpeed = kResetBallSpeed;

	player.initialGameScore = 0; // reset score
	player.carsHit = 0;
	player.health = kResetPlayerHealth;
	player.momentum = { 0,0,0 };
	player.drag = { 0,0,0 };
	player.thrust = { 0,0,0 };
	player.oldMomentum = { 0,0,0 };
	player.model->ResetOrientation();
	myCamera->DetachFromParent(); 
	myCamera->ResetOrientation();
	DefaultCamera(myCamera); // camera is set back to default
}

void MoveCars(PlayerStruct& player, Vector3 localZ, float frameTime, ISceneNode* wheelNode1, ISceneNode* wheelNode2, I3DEngine* myEngine)
{

	if (myEngine->KeyHeld(kForwardKey)) // move up
	{
		player.isAccelerating = true;
		player.thrust = { localZ.x * player.speed * frameTime,localZ.y * player.speed * frameTime,localZ.z * player.speed * frameTime }; // mpve z component of matrices

		RotateWheels(wheelNode1, wheelNode2, player.kRotationSpeed, frameTime);

	}
	else if (myEngine->KeyHeld(kBackwardKey)) // move down
	{
		player.isAccelerating = false;
		player.thrust = { localZ.x * -player.speed * frameTime,localZ.y * -player.speed * frameTime,localZ.z * -player.speed * frameTime };
		RotateWheels(wheelNode1, wheelNode2, -player.kRotationSpeed, frameTime);
	}

	else
	{
		player.thrust = { 0, 0, 0 };
	}

	if (myEngine->KeyHeld(kClockwiseKey)) // if D is held
	{
		player.isAccelerating = true;
		player.model->RotateLocalY(player.kRotationSpeed * frameTime); // rotate the car clockwise
	}

	else if (myEngine->KeyHeld(kAntiClockwiseKey)) // if A is held
	{
		player.isAccelerating = true;
		player.model->RotateLocalY(-player.kRotationSpeed * frameTime); // rotate the car anti-clockwise
	}

}

void RealisticMovement(PlayerStruct& player, float frameTime) // incorporate drag thrust and momentum for realistic movement
{
	player.drag = { player.oldMomentum.x * kDragFactor,player.oldMomentum.y * kDragFactor,player.oldMomentum.z * kDragFactor };

	player.momentum = { player.thrust.x + player.oldMomentum.x + player.drag.x,
		player.thrust.y + player.oldMomentum.y + player.drag.y,
		player.thrust.z + player.oldMomentum.z + player.drag.z };

	player.model->Move(player.momentum.x * frameTime, player.momentum.y * frameTime, player.momentum.z * frameTime);
}