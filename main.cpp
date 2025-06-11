#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <iostream>

// Planner function declarations
void initializePlanner(Vector3 start, Vector3 goal);
void planStep();
void DrawPlanner();
Vector3 getNextPathPoint();
std::vector<Vector3> getFullPath();

void DrawEnvironment();

int main()
{
    InitWindow(800, 600, "RRT* 3D Navigation");
    SetTargetFPS(60);
    InitAudioDevice();

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 20.0f, 20.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    Vector3 startPos = { 5.0f, 1.0f, -3.0f };
    Vector3 goalPos = { -8.0f, 1.0f, -5.0f };
    Vector3 spherePos = startPos;
    Vector3 targetPoint = startPos;
    float sphereRadius = 1.0f;

    initializePlanner(startPos, goalPos);

    // Get the full path from planner
    std::vector<Vector3> pathPoints = getFullPath();
    int currentPathIndex = 0;

    Vector3 robotPosition = { 0, 0, 0 };
    float reachThreshold = 0.5f;

    if (!pathPoints.empty())
        targetPoint = pathPoints[0];

    while (!WindowShouldClose())
{
    float deltaTime = GetFrameTime();
    float wheel = GetMouseWheelMove();

    if (wheel != 0.0f)
    {
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        camera.position = Vector3Add(camera.position, Vector3Scale(forward, wheel * 2.0f));
    }

    //  Expand RRT each frame
    planStep();

    //  Get updated path from the planner
    pathPoints = getFullPath();

    //  Update robot target and move along path
    if (!pathPoints.empty() && currentPathIndex < pathPoints.size())
    {
        targetPoint = pathPoints[currentPathIndex];

        Vector3 toTarget = Vector3Subtract(targetPoint, robotPosition);
        float distToTarget = Vector3Length(toTarget);

        if (distToTarget < reachThreshold && currentPathIndex < pathPoints.size() - 1)
        {
            currentPathIndex++;  // move to next point
        }
        else
        {
            Vector3 dir = Vector3Normalize(toTarget);
            float speed = 2.0f;
            robotPosition = Vector3Add(robotPosition, Vector3Scale(dir, speed * deltaTime));
        }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawEnvironment();
    DrawPlane((Vector3){5.0f, 0.0f, 5.0f}, (Vector2){20.0f, 20.0f}, LIGHTGRAY);
    DrawPlanner();
    DrawSphere(robotPosition, sphereRadius, BLUE);  //  this reflects the moving robot
    DrawSphere(goalPos, 0.2f, GREEN);
    EndMode3D();

    DrawText("RRT* path planning in 3D with collision avoidance", 10, 10, 20, DARKGRAY);
    EndDrawing();
}
    std::cout << "Path size: " << pathPoints.size() << std::endl;
    CloseWindow();
    return 0;
}
