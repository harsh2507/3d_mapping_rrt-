// planner.cpp
#include "raylib.h"
#include "raymath.h"   // For vector math
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>


bool planningComplete = false;

// External variables (defined elsewhere in your environment.cpp or main.cpp)
extern float tileSize;
extern float obstacleRadius;
extern float obstacleHeight;
float robotRadius = 1.0f;  // Define and set in your environment or main file


// Obstacle structure with position and radius
struct Obstacle {
    Vector3 position;
    float radius;
};

// Your global obstacles list, defined externally (extern)
extern std::vector<Obstacle> obstacles;

// Node for RRT tree
struct Node {
    Vector3 position;
    int parent;  // index of parent node (-1 for root)
};

static std::vector<Node> tree;
static Vector3 startPoint;
static Vector3 goalPoint;
static std::vector<Vector3> pathPoints;  // final path points (smooth)
static int currentPathIndex = 0;

// Distance helper
static float distance(Vector3 a, Vector3 b) {
    return Vector3Length(Vector3Subtract(a, b));
}

// Check if a point is in collision with any obstacle
bool isPointInCollision(Vector3 point) {
     for (int i = 0; i < obstacles.size(); i++){
        Obstacle obs = obstacles[i]; 
        float inflatedRadius = obs.radius + robotRadius;
        if (distance(point, obs.position) < inflatedRadius) {
            return true;
        }
    }
    return false;
}

// Check if the line segment from a to b intersects any obstacle (simple discretized check)
bool isSegmentInCollision(Vector3 a, Vector3 b) {
    const int steps = 10;
    for (int i = 0; i <= steps; i++) {
        float t = i / (float)steps;
        Vector3 p = Vector3Lerp(a, b, t);
        if (isPointInCollision(p)) return true;
    }
    return false;
}

// Initialize planner
void initializePlanner(Vector3 start, Vector3 goal) {
    tree.clear();
    pathPoints.clear();
    currentPathIndex = 0;

    startPoint = start;
    goalPoint = goal;

    tree.push_back({ start, -1 });
    pathPoints.push_back(start);
}

// Sample a random point within some bounds (modify bounds as per your environment)
Vector3 sampleRandomPoint() {
    float range = 10.0f; // Example: environment from -10 to +10 on X and Z
    return {
        (float)GetRandomValue(-1000, 1000) / 100.0f * tileSize,  // X
        obstacleHeight / 2.0f,                                    // Y fixed to obstacle height mid-level
        (float)GetRandomValue(-1000, 1000) / 100.0f * tileSize   // Z
    };
}

// Find nearest node index in tree to given point
int nearestNodeIndex(const Vector3& point) {
    int nearest = 0;
    float minDist = distance(tree[0].position, point);
    for (int i = 1; i < (int)tree.size(); i++) {
        float dist = distance(tree[i].position, point);
        if (dist < minDist) {
            minDist = dist;
            nearest = i;
        }
    }
    return nearest;
}

// Smooth cubic spline interpolation helpers
// Cubic spline using Catmull-Rom spline (simple and common for path smoothing)

// Interpolate between 4 points at t [0,1]
Vector3 catmullRomSpline(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    // Catmull-Rom spline formula
    float t2 = t * t;
    float t3 = t2 * t;

    Vector3 result = {
        0.5f * ((2.0f * p1.x) +
            (-p0.x + p2.x) * t +
            (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
            (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3),

        0.5f * ((2.0f * p1.y) +
            (-p0.y + p2.y) * t +
            (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
            (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3),

        0.5f * ((2.0f * p1.z) +
            (-p0.z + p2.z) * t +
            (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 +
            (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3)
    };
    return result;
}

// Generate smooth path points from raw path using Catmull-Rom spline
std::vector<Vector3> generateSmoothPath(const std::vector<Vector3>& rawPath, int subdivisions = 10) {
    std::vector<Vector3> smoothPath;

    int n = (int)rawPath.size();
    if (n < 2) return rawPath;

    for (int i = 0; i < n - 1; i++) {
        Vector3 p0 = i == 0 ? rawPath[i] : rawPath[i - 1];
        Vector3 p1 = rawPath[i];
        Vector3 p2 = rawPath[i + 1];
        Vector3 p3 = (i + 2 < n) ? rawPath[i + 2] : rawPath[i + 1];

        for (int j = 0; j < subdivisions; j++) {
            float t = j / (float)subdivisions;
            smoothPath.push_back(catmullRomSpline(p0, p1, p2, p3, t));
        }
    }
    smoothPath.push_back(rawPath.back());

    return smoothPath;
}

// Plan one step of RRT* algorithm with obstacle collision checking
void planStep() {
    if (planningComplete) {
        std::cout << "Planning already complete.\n";
        return;
    }
    if (tree.empty()) {
        std::cout << "Tree is empty.\n";
        return;
    }

    Vector3 sample = sampleRandomPoint();
    std::cout << "Sampled point: " << sample.x << ", " << sample.y << ", " << sample.z << "\n";

    int nearest = nearestNodeIndex(sample);
    Vector3 nearestPos = tree[nearest].position;
    std::cout << "Nearest node: " << nearestPos.x << ", " << nearestPos.y << ", " << nearestPos.z << "\n";

    Vector3 dir = Vector3Subtract(sample, nearestPos);
    float dirLen = Vector3Length(dir);
    std::cout << "Direction length: " << dirLen << "\n";
    if (dirLen == 0) return;

    dir = Vector3Normalize(dir);
    float stepSize = tileSize * 0.9f;
    Vector3 newPoint = Vector3Add(nearestPos, Vector3Scale(dir, stepSize));
    std::cout << "New point: " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << "\n";

    if (isPointInCollision(newPoint)) {
        std::cout << "Collision at new point.\n";
        return;
    }

    if (isSegmentInCollision(nearestPos, newPoint)) {
        std::cout << "Collision along segment.\n";
        return;
    }

    tree.push_back({ newPoint, nearest });
    std::cout << "Added new node. Tree size: " << tree.size() << "\n";

   if (distance(newPoint, goalPoint) < stepSize && 
    !isPointInCollision(goalPoint) && 
    !isSegmentInCollision(newPoint, goalPoint)) {

        tree.push_back({ goalPoint, (int)tree.size() - 1 });
        std::cout << "Goal reached!\n";

        std::vector<Vector3> rawPath;
        int current = (int)tree.size() - 1;
        while (current != -1) {
            rawPath.push_back(tree[current].position);
            current = tree[current].parent;
        }
        std::reverse(rawPath.begin(), rawPath.end());

        pathPoints = generateSmoothPath(rawPath, 15);
        currentPathIndex = 0;

        planningComplete = true;
    }
}



// Get next point on path for robot to follow
Vector3 getNextPathPoint() {
    if (pathPoints.empty()) return startPoint;

    if (currentPathIndex >= (int)pathPoints.size())
        return pathPoints.back();

    return pathPoints[currentPathIndex++];
}

// Draw planner elements
void DrawPlanner() {
    // Draw tree edges
    for (int i = 1; i < (int)tree.size(); i++) {
        Vector3 from = tree[i].position;
        Vector3 to = tree[tree[i].parent].position;
        DrawLine3D(from, to, DARKGRAY);
    }

    // Draw path points
    for (int i = 0; i < (int)pathPoints.size(); i++) {
        DrawSphere(pathPoints[i], 0.1f, RED);
        if (i + 1 < (int)pathPoints.size()) {
            DrawLine3D(pathPoints[i], pathPoints[i + 1], RED);
        }
    }
}

// Return full smooth path (for debugging or other use)
std::vector<Vector3> getFullPath() {
    return pathPoints;
}