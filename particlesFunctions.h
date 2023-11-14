#pragma once

constexpr float kParticlesXBoundary = 5.f, kParticlesYBoundary = 1.5f, kParticlesZBoundary = 8.f;
constexpr float kParticlesYPos = -5.f;
constexpr int kParticlesMinBoundary = -20, kParticlesMaxBoundary = 20;
constexpr float kInitialSpeed = 20.f;
constexpr float kScaleDownParticles = 0.07f;
constexpr float kGravity = -10.f;
constexpr float kDecelerateValue = 0.02f;

float GenerateRandomInt(int rangeMin, int rangeMax) // generate random numbers for particles' positions
{
	float result = (float)rand() / (float)(RAND_MAX + 1);
	result *= (float)(rangeMax - rangeMin);
	result += rangeMin;

	return result;
}


void EmitParticle(Particle& particles, PlayerStruct &player, IMesh* particlesMesh)
{
	particles.model = particlesMesh->CreateModel(0, kParticlesYPos, 0); // create particles under the player car
	particles.model->AttachToParent(player.model);
	particles.movementVector.x = GenerateRandomInt(kParticlesMinBoundary, kParticlesMaxBoundary); 
	particles.previousSpeed = kInitialSpeed; particles.movementVector.y = kParticlesMaxBoundary; particles.movementVector.z = kParticlesMinBoundary;
	particles.model->Scale(kScaleDownParticles); // scale down by 0.7
}

void DecelerateDust(Particle& particles, float frameTime)
{
	particles.movementVector.y = particles.previousSpeed + (kGravity * frameTime); // allow gravity to pull the particles down
	particles.movementVector.x -= kDecelerateValue * frameTime; // decelerate by 0.02
	particles.movementVector.z -= kDecelerateValue * frameTime;
	particles.movementVector.y -= kDecelerateValue * frameTime;

	// move particles by new momentum
	particles.model->Move(particles.movementVector.x * frameTime, 
	particles.movementVector.y * frameTime, particles.movementVector.z);
	particles.previousSpeed = particles.movementVector.y;
}