#pragma once
#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
using namespace tle;

constexpr float kCameraYPos = 15.f, kCameraZPos = -60.f, kCameraRotateX = 15.f;
constexpr float kChaseCameraY = 4.f, kChaseCameraZ = -8.f;
constexpr float kFirstCameraY = 3.f, kFirstCameraZ = -2.5f;

void DefaultCamera(ICamera* camera)
{
	camera->ResetOrientation(); // reset camera
	camera->DetachFromParent(); // detach it from the car
	// set the camera positions back to default
	camera->SetPosition(0, kCameraYPos, kCameraZPos);
	camera->RotateX(kCameraRotateX);
}

void ChaseCameraView(ICamera* camera, IModel* player)
{
	camera->ResetOrientation();
	camera->SetLocalY(kChaseCameraY); camera->SetLocalZ(kChaseCameraZ); // set the camera above and behind player car
	camera->AttachToParent(player); // a
}

void FirstPersonView(ICamera* camera, IModel* player)
{
	camera->ResetOrientation();
	camera->SetLocalY(kFirstCameraY); camera->SetLocalZ(kFirstCameraZ); // move camera to a first person view
	camera->AttachToParent(player);
}
