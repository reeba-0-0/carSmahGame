// GamesAssignmentTwo.cpp: A program using the TL-Engine

#include "cameraControls.h"
#include "structs.h"
#include "gameFunction.h"
#include "collisionFunctions.h"
#include "particlesFunctions.h"

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );
	myEngine->AddMediaFolder(".\\Media");

	constexpr float kGravity = -10.f;
	constexpr int kDustParticlesSize = 300;
	constexpr int kHealthFontSize = 640;

	// camera consts
	constexpr float kCameraXPos = 0.0f, kCameraYPos = 15.0f, kCameraZPos = 60.0f, kCameraRotateX = 15.0f;

	// posistion consts
	constexpr int kNumEnemyCars = 4;
	constexpr float kSkyboxX = 0.0f, kSkyboxY = 960.0f, kSkyboxZ = 0.0f;
	constexpr float kEnemyNewXPos[kNumEnemyCars] = { 30.f, -30.f, -30.f, 30.f };
	constexpr float kBallsXPos = 0.f, kBallsYPos = 2.f, kBallsZPos = 0.f, kBallSpeed = 1.f;

	// key consts
	constexpr EKeyCode kRestartKey = Key_R, kPauseKey = Key_P, kDefaultCamKey = Key_1, kChaseCamKey = Key_2, kFirstCamKey = Key_3;

	// font sizes
	constexpr float kCentreFontX = 640.f, kCentreFontY = 320.f, kRightFontX = 1280.f, kFontSize = 30.f;
	constexpr int kTreesNum = 160;

	// enemy car positions
	constexpr float kStaticEnemyCarX[kNumEnemyCars] = { -20.f, 20.f, -20.f, 20.f }, kStaticEnemyCarY[4] = { 0.0f, 0.0f, 0.f, 0.f }, kStaticEnemyCarZ[4] = { 20.0f, 20.0f, 0.f, 0.f };
	constexpr float kMovingEnemyCarX[kNumEnemyCars] = { -30.f, 30.f, 30.f, -30.f }, kMovingEnemyCarY[4] = { 0.f, 0.f, 0.f, 0.f }, kMovingEnemyCarZ[4] = { 15.f, -15.f, 30.f, -30.f };

	// matrices
	constexpr int kXMatricesComponent = 0, kYMatricesComponent = 1, kZMatricesComponent = 2;

	// particles consts
	constexpr float kParticlesXBoundary = 5.f, kParticlesYBoundary = 1.5f, kParticlesZBoundary = 8.f;
	constexpr float kParticlesYPos = -5.f;
	constexpr int kParticlesMinBoundary = -20, kParticlesMaxBoundary = 20;

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	DefaultCamera(myCamera);
	enum EGameStates { playing, paused, gameWon, gameOver }; // track the state of the game

	//load meshes
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IMesh* skyBoxMesh = myEngine->LoadMesh("Skybox01.x");
	IMesh* playerCarMesh = myEngine->LoadMesh("4x4jeep.x");
	IMesh* staticCarMesh = myEngine->LoadMesh("audi.x");
	IMesh* movingCarMesh = myEngine->LoadMesh("estate.x");
	IMesh* ballMesh = myEngine->LoadMesh("ball.x");
	IMesh* treesMesh = myEngine->LoadMesh("tree.x");
	IMesh* particlesMesh = myEngine->LoadMesh(("particle.x"));

	PlayerStruct playerCar;
	EnemyStruct staticEnemies[kNumEnemyCars], movingEnemies[kNumEnemyCars]/*, movingBalls[kNumEnemyCars]*/;
	TreesStruct treesArray[kTreesNum];
	Particle dustParticles[kDustParticlesSize];
	playerCar.model = playerCarMesh->CreateModel(playerCar.kOriginX, playerCar.kOriginY, playerCar.kOriginZ); // position at origin

	IModel* skyBox = skyBoxMesh->CreateModel(kSkyboxX, kSkyboxY, kSkyboxZ); // position at (0, -960, 0)
	IModel* ground = groundMesh->CreateModel(); // position at origin
	ISceneNode* wheelNode1 = playerCar.model->GetNode(5); 	// access wheel nodes to spin
	ISceneNode* wheelNode2 = playerCar.model->GetNode(6);
	IFont* myFont = myEngine->LoadFont("Calibri", kFontSize);

	// use a for loop to loop through the array of static enemy car's and balls to create the models
	for (int i = 0; i < kNumEnemyCars; i++)
	{
		staticEnemies[i].model = staticCarMesh->CreateModel(kStaticEnemyCarX[i], kStaticEnemyCarY[i], kStaticEnemyCarZ[i]);
		staticEnemies[i].ball = ballMesh->CreateModel(kBallsXPos, kBallsYPos, kBallsZPos); // position at (0,2,0) - so the y position is just above cars
		staticEnemies[i].ball->AttachToParent(staticEnemies[i].model); // attach balls to cars
	}

	// loop through the array of moving enemy car's and balls to create the models
	for (int i = 0; i < kNumEnemyCars; i++)
	{
		movingEnemies[i].model = movingCarMesh->CreateModel(kMovingEnemyCarX[i], kMovingEnemyCarY[i], kMovingEnemyCarZ[i]);
		movingEnemies[i].ball = ballMesh->CreateModel(kBallsXPos, kBallsYPos, kBallsZPos);
		movingEnemies[i].ball->AttachToParent(movingEnemies[i].model);
	}

	RotateCars(movingEnemies);

	for (int i = 0; i < kTreesNum; i++) 	// create trees array
	{
		treesArray[i].model = treesMesh->CreateModel(i * treesArray->kCreateCircle, 0, 0);
	}

	for (int i = 0; i < kDustParticlesSize; i++) // create particles array
	{
		EmitParticle(dustParticles[i], playerCar, particlesMesh);
	}

	float treesDegrees = 0; // set the degrees of the tree's position to 0
		for (int i = 0; i < kTreesNum; i++) // use trees array to create a circle around the cars
	{
		CreateTrees(treesArray[i], treesDegrees);
	}

	float elapsedTime = 0; // elapsed time for moving enemy cars

	EGameStates gameState = playing; // set it to playing as this is the initial state
	myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		float frameTime = myEngine->Timer();
		/**** Update your scene each frame here ****/
		Vector3 oldCarPos = { playerCar.model->GetX(),playerCar.model->GetY(), playerCar.model->GetZ() }; // previous player car position
		//Vector3 oldWheelsPos = { wheelNode2->GetX(), wheelNode2->GetY(), wheelNode2->GetZ() };

		float playerMatrix[4][4]; // access player matrix for realistic movement
		playerCar.model->GetMatrix(&playerMatrix[0][0]);
		Vector3 localZ = { playerMatrix[kZMatricesComponent][kXMatricesComponent], playerMatrix[kZMatricesComponent][kYMatricesComponent], playerMatrix[kZMatricesComponent][kZMatricesComponent] }; // z component of matrices

		stringstream scoreText; // use stringstream to create dynamic text for the score
		scoreText << "Score: " << playerCar.initialGameScore;
		myFont->Draw(scoreText.str(), 0, 0, kWhite);

		stringstream healthText; // use stringstream to create dynamic text for the score
		healthText << "Health: " << playerCar.health;
		myFont->Draw(healthText.str(), kHealthFontSize, 0, kWhite);

		if (gameState == playing)
		{

			for (int i = 0; i < kNumEnemyCars; i++)
			{
				// move cars based on WASD keys
				MoveCars(playerCar, localZ, frameTime, wheelNode1, wheelNode2, myEngine);
				RealisticMovement(playerCar, frameTime); 

				if (CheckPlayerToEnemyCollision(playerCar, staticEnemies[i], staticEnemies[i].kStaticXBoundary, staticEnemies[i].kStaticZBoundary))
				{
					CollisionResolution(oldCarPos, playerCar, staticEnemies[i], frameTime); // don't allow car to drive through

					if (staticEnemies[i].health == staticEnemies[i].kInitialHealth) // check if car has been hit
					{
						staticEnemies[i].health--; // decrement health
						playerCar.carsHit++; 
						CheckStaticCollision(playerCar, staticEnemies[i]); // check for front/rear or side collision
					}

					else
					{
						BounceBack(staticEnemies[i], playerCar); // bounce back when car is hit
					}
				}

				if (CheckPlayerToEnemyCollision(playerCar, movingEnemies[i], movingEnemies[i].kMovingXBoundary, movingEnemies[i].kMovingZBoundary))
				{
					CollisionResolution(oldCarPos, playerCar, movingEnemies[i], frameTime);

					if (movingEnemies[i].health == movingEnemies[i].kInitialHealth)
					{
						movingEnemies[i].health--;
						playerCar.carsHit++;
						movingEnemies[i].timer = true;
						CheckMovingCollision(playerCar, movingEnemies[i]);
					}
					playerCar.momentum = { 0,0,0 };
					playerCar.thrust = { 0,0,0 };
				}

				MoveEnemiesAcrossX(movingEnemies[i], frameTime); // move enemies horizontally
				MoveBallsVertically(movingEnemies[i], frameTime); // move balls up and down

				if (playerCar.isAccelerating == true) 
				{
					for (int j = 0; j < kDustParticlesSize; j++) // emit particles when car is moving forward or rotating
					{
						// check if particle's x, y and z positions are less than a certain point
						if (dustParticles[j].model->GetX() < (playerCar.model->GetX() + kParticlesXBoundary) && dustParticles[j].model->GetX() > (playerCar.model->GetX() - kParticlesXBoundary) &&
							dustParticles[j].model->GetY() < kParticlesYBoundary && dustParticles[j].model->GetZ() < kParticlesZBoundary)
						{
							// if they are then move the particles by the momentum vector values
							dustParticles[j].model->Move(dustParticles[j].movementVector.x * frameTime,
								dustParticles[j].movementVector.y * frameTime,
								dustParticles[j].movementVector.z * frameTime);
						}

						else // let gravity take effect and set the position back
						{
							dustParticles[j].movementVector.y = dustParticles->previousSpeed + (kGravity * frameTime); //update the speed with the effect of gravity
 							dustParticles[j].model->SetLocalPosition(wheelNode2->GetLocalX() + 1, wheelNode2->GetLocalY(), wheelNode2->GetLocalZ());
							dustParticles->previousSpeed = dustParticles[j].movementVector.y; 
						}

						if (dustParticles[j].model->GetY() < kParticlesYPos) // regenerate particles
						{
							dustParticles[j].movementVector.y = GenerateRandomInt(kParticlesMinBoundary, kParticlesMaxBoundary);
							dustParticles[j].model->SetLocalPosition(wheelNode2->GetLocalX() + 1, wheelNode2->GetLocalY(), wheelNode2->GetLocalZ());
						}

						if (!myEngine->KeyHeld(Key_W) && !myEngine->KeyHeld(Key_A) && !myEngine->KeyHeld(Key_D))
						{
							DecelerateDust(dustParticles[j], frameTime); // bring the particles back to the original position
						}
					}

				}

				if (movingEnemies[i].elapsedTime <= movingEnemies[i].kCarTimer && movingEnemies[i].timer == true)
				{
					movingEnemies[i].elapsedTime += frameTime; // set 15 second timer after a car has been hit
				}

				if (movingEnemies[i].elapsedTime >= movingEnemies[i].kCarTimer && movingEnemies[i].timer == true && movingEnemies[i].health == 0)
				{
					MoveAfter15(playerCar, movingEnemies[i]); 
				}

			}

			if (myEngine->KeyHit(kDefaultCamKey)) // if 1 key is hit
			{
				DefaultCamera(myCamera); 
			}

			if (myEngine->KeyHit(kChaseCamKey)) // if 2 key is hit
			{
				ChaseCameraView(myCamera, playerCar.model);
			}

			if (myEngine->KeyHit(kFirstCamKey)) // if 3 key is hit
			{
				FirstPersonView(myCamera, playerCar.model);
			}

			if (myEngine->KeyHit(kPauseKey)) // pause game if P key is hit
			{
				gameState = paused;
			}
		}

		else if (gameState == paused) // display paused text
		{
			myFont->Draw("Paused", kCentreFontX, kCentreFontY, kWhite, kCentre, kVCentre); // display paused text
			if (myEngine->KeyHit(kPauseKey))
			{
				gameState = playing; // set it back to playing if P key is hit again
			}
		}

		if (playerCar.carsHit == playerCar.kMaxCarsHit) 
		{
			gameState = gameWon;
		}

		if (gameState == gameWon)
		{
			// display winning text
			myFont->Draw("You Win! :) Click 'R' to restart", kCentreFontX, kCentreFontY, kWhite, kCentre, kVCentre);
		}

		for (int i = 0; i < treesArray->kTreesNum; i++) // check if car has collided with the trees
		{
			if (CheckPlayerToTreesCollision(playerCar, treesArray[i]))
			{
				playerCar.model->SetLocalPosition(oldCarPos.x, oldCarPos.y, oldCarPos.z); // set car's position back
				playerCar.health --; // decrement car's health
			}

			if (playerCar.health == 0)
			{
				gameState = gameOver;
			}
		}

		if (gameState == gameOver) // display losing text
		{
 			myFont->Draw("You Lose! :( Click 'R' to restart", kCentreFontX, kCentreFontY, kWhite, kCentre, kVCentre);
 		}

		if (gameState == gameWon || gameState == gameOver) // restart when game is lost or won
		{
			if (myEngine->KeyHit(kRestartKey)) // if R key is hit
			{
				for (int car = 0; car < kNumEnemyCars; car++)
				{
					// call restart function
					RestartGame(playerCar, staticEnemies[car], movingEnemies[car], myCamera);
					movingEnemies[car].model->SetPosition(kMovingEnemyCarX[car], kMovingEnemyCarY[car], kMovingEnemyCarZ[car]);
					gameState = playing; // allow playing again
				}
			}
		}
			
		if (myEngine->KeyHit(kExitKey)) // exit if ESC key is hit
		{
			myEngine->Stop();
		}
	}

	myEngine->Delete();
}
