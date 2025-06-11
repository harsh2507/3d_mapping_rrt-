// controller.cpp
#include "raylib.h"
#include "raymath.h"

Vector3 spherePos = { 5.0f, 1.0f, -3.0f };
Vector3 sphereVelocity = { 0.0f, 0.0f, 0.0f };
Vector3 sphereAcceleration = { 0.0f, 0.0f, 0.0f };
Vector3 sphereRotation = { 0.0f, 0.0f, 0.0f };
Vector3 sphereAngularVelocity = { 0.0f, 0.0f, 0.0f };

const float maxAcceleration = 2.0f;
const float maxVelocity = 5.0f;
const float maxAngularVelocity = 0.5f;
float sphereRadius = 1.0f;

void UpdateController(float deltaTime)
{
    Vector3 inputAccel = { 0.0f, 0.0f, 0.0f };

    if (IsKeyDown(KEY_W)) inputAccel.z -= 1.0f;
    if (IsKeyDown(KEY_S)) inputAccel.z += 1.0f;
    if (IsKeyDown(KEY_A)) inputAccel.x -= 1.0f;
    if (IsKeyDown(KEY_D)) inputAccel.x += 1.0f;
    if (IsKeyDown(KEY_E)) inputAccel.y += 1.0f;
    if (IsKeyDown(KEY_Q)) inputAccel.y -= 1.0f;

    if (Vector3Length(inputAccel) > 0)
    {
        inputAccel = Vector3Normalize(inputAccel);
        inputAccel = Vector3Scale(inputAccel, maxAcceleration);
    }

    sphereAcceleration = inputAccel;
    sphereVelocity = Vector3Add(sphereVelocity, Vector3Scale(sphereAcceleration, deltaTime));

    if (Vector3Length(sphereVelocity) > maxVelocity)
        sphereVelocity = Vector3Scale(Vector3Normalize(sphereVelocity), maxVelocity);

    spherePos = Vector3Add(spherePos, Vector3Scale(sphereVelocity, deltaTime));

    Vector3 inputAngVel = { 0.0f, 0.0f, 0.0f };

    if (IsKeyDown(KEY_UP))    inputAngVel.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN))  inputAngVel.x += 1.0f;
    if (IsKeyDown(KEY_LEFT))  inputAngVel.y -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) inputAngVel.y += 1.0f;
    if (IsKeyDown(KEY_Z))     inputAngVel.z -= 1.0f;
    if (IsKeyDown(KEY_X))     inputAngVel.z += 1.0f;

    if (Vector3Length(inputAngVel) > 0)
    {
        inputAngVel = Vector3Normalize(inputAngVel);
        inputAngVel = Vector3Scale(inputAngVel, maxAngularVelocity);
    }

    sphereAngularVelocity = inputAngVel;

    sphereRotation.x += sphereAngularVelocity.x * deltaTime;
    sphereRotation.y += sphereAngularVelocity.y * deltaTime;
    sphereRotation.z += sphereAngularVelocity.z * deltaTime;
}

void DrawSphere()
{
    DrawSphereEx(spherePos, sphereRadius, 16, 16, BLUE);
}
