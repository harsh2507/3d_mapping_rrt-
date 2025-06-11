// environment.cpp
#include "raylib.h"
#include <vector>

// Constants
const int screenWidth = 1000;
const int screenHeight = 800;

int gridSize = 10;
float tileSize = 2.0f;

float obstacleRadius = 0.5f;
float obstacleHeight = 2.0f;

float envMinX = -10.0f;
float envMaxX = 10.0f;
float envMinZ = -10.0f;
float envMaxZ = 10.0f;

// Define the Obstacle class
class Obstacle {
public:
    Vector3 position;
    float radius;

    Obstacle(Vector3 pos, float r) {
        position = pos;
        radius = r;
    }
};

// Create a vector of Obstacle objects
std::vector<Obstacle> obstacles = {
    Obstacle({  2 * tileSize, obstacleHeight / 2.0f,  3 * tileSize }, obstacleRadius),
    Obstacle({ -1 * tileSize, obstacleHeight / 2.0f, -4 * tileSize }, obstacleRadius),
    Obstacle({  0 * tileSize, obstacleHeight / 2.0f,  0 * tileSize }, obstacleRadius),
    Obstacle({  4 * tileSize, obstacleHeight / 2.0f, -2 * tileSize }, obstacleRadius),
    Obstacle({ -3 * tileSize, obstacleHeight / 2.0f,  1 * tileSize }, obstacleRadius),
    Obstacle({  1 * tileSize, obstacleHeight / 2.0f, -6 * tileSize }, obstacleRadius),
    Obstacle({ -5 * tileSize, obstacleHeight / 2.0f, -1 * tileSize }, obstacleRadius),
    Obstacle({  3 * tileSize, obstacleHeight / 2.0f,  4 * tileSize }, obstacleRadius),
    Obstacle({ -8 * tileSize, obstacleHeight / 2.0f,  4 * tileSize }, obstacleRadius),
    Obstacle({  8 * tileSize, obstacleHeight / 2.0f,  5 * tileSize }, obstacleRadius),
    Obstacle({  3 * tileSize, obstacleHeight / 2.0f, -9 * tileSize }, obstacleRadius),
    Obstacle({  7 * tileSize, obstacleHeight / 2.0f, -7 * tileSize }, obstacleRadius),
    Obstacle({  6 * tileSize, obstacleHeight / 2.0f,  6 * tileSize }, obstacleRadius)
};

void DrawEnvironment()
{
    // Draw the checkerboard grid
    for (int x = -gridSize; x < gridSize; x++)
    {
        for (int z = -gridSize; z < gridSize; z++)
        {
            Color tileColor = ((x + z) % 2 == 0) ? LIGHTGRAY : GRAY;
            Vector3 pos = { x * tileSize, 0.0f, z * tileSize };
            DrawCube(pos, tileSize, 0.1f, tileSize, tileColor);
        }
    }

    // Draw each obstacle from the vector
    for (int i = 0; i < obstacles.size(); i++) {
        Obstacle obs = obstacles[i];
        DrawCylinder(obs.position, obs.radius, obs.radius, obstacleHeight, 16, RED);
    }
}
