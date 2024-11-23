#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LANE_COUNT 5
#define MAX_ROBOTS 10
#define MAX_PROJECTILES 50
#define EASY_TIME 60   // Easy level time (in seconds)
#define HARD_TIME 30   // Hard level time (in seconds)
#define EASY_ROBOT_SPEED 1.0f  // Slow robots for easy level
#define HARD_ROBOT_SPEED 3.0f // Fast robots for hard level
#define BASE_HEALTH 5         // Base health

// Struct Definitions
typedef struct Plant {
    Vector2 position;
    bool active;
    int shootTimer;  // Timer to track shooting intervals
} Plant;



typedef struct Robot {
    Vector2 position;
    bool active;
    int health;
    float speed;  // Robot speed for difficulty level
} Robot;

typedef struct Projectile {
    Vector2 position;
    bool active;
    int lane;
} Projectile;

// Global Variables
Plant plants[LANE_COUNT];
Robot robots[MAX_ROBOTS];
Projectile projectiles[MAX_PROJECTILES];
bool gameRunning = false;
bool gameOver = false;
int score = 0;
int baseHealth = BASE_HEALTH;  // Base health starts at 5
int gameTime = EASY_TIME;  // Default to easy time
float remainingTime;
float timeSinceLastUpdate = 0.0f;
float secondsPerUpdate = 1.0f;  // Set to 1 second for normal time update
float robotSpeed = EASY_ROBOT_SPEED; // Default to easy speed
char endMessage[50];
int plantCount = 0;  // Tracks the number of active plants

// Function Declarations
void InitGame();
void UpdateGame();
void DrawGame();
void SpawnRobot();
void PlacePlant(Vector2 position);  // Updated to take mouse position
void CheckCollisions();
void ShootProjectile(int lane);
void UpdateProjectiles();
void EndGame(const char* message);
void DifficultyScreen();  // New function for difficulty selection

int main(void) {
    // Initialize Window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Roots vs Robots");
    SetTargetFPS(60);

    // Main Game Loop
    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (!gameRunning) {
                // Home Screen
                BeginDrawing();
                ClearBackground(PURPLE);
                DrawText("ROOTS VS ROBOTS", 200, 150, 40, DARKGRAY);
                DrawText("Press ENTER to Start", 250, 300, 20, DARKGRAY);
                DrawText("How to Play:", 50, 400, 20, DARKGRAY);
                DrawText("- Click to place plants at any position", 50, 430, 20, DARKGRAY);
                DrawText("- Prevent robots from reaching the left!", 50, 460, 20, DARKGRAY);

                if (IsKeyPressed(KEY_ENTER)) {
                    DifficultyScreen();  // Show difficulty selection screen
                }
                EndDrawing();
            } 
            else {
                UpdateGame();
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawGame();
                EndDrawing();
            }
        } 
        else {
            // Game Over Screen
            BeginDrawing();
            ClearBackground(SKYBLUE);
            DrawText(endMessage, 250, 200, 40, DARKGRAY);
            DrawText("Press ENTER to Restart", 250, 300, 20, DARKGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;  // Restart game
                InitGame();
            }

            EndDrawing();
        }
    }

    CloseWindow();  // Close window and OpenGL context
    return 0;
}

void InitGame() {
    // Initialize Plants (One per Lane)
    for (int i = 0; i < LANE_COUNT; i++) {
        plants[i].position = (Vector2){-100, i * 100 + 50};  // Initialize off-screen
        plants[i].active = false;
        plants[i].shootTimer = 0;  // Reset shoot timer for each plant
    }

    // Initialize Robots
    for (int i = 0; i < MAX_ROBOTS; i++) {
        robots[i].active = false;
    }

    // Initialize Projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }

    // Reset score, remaining time, and plant count
    score = 0;
    remainingTime = gameTime;
    plantCount = 0;  // Reset the plant count at the start of the game
}

void DifficultyScreen() {
    // Show Difficulty Selection Screen
    while (!gameRunning && !gameOver) {
        BeginDrawing();
        ClearBackground(PINK);
        DrawText("Select Difficulty:", 250, 150, 40, DARKGRAY);
        DrawText("Press 1 for Easy", 250, 200, 20, DARKGRAY);
        DrawText("Press 2 for Hard", 250, 230, 20, DARKGRAY);

        if (IsKeyPressed(KEY_ONE)) {
            gameTime = EASY_TIME;
            robotSpeed = EASY_ROBOT_SPEED;
            gameRunning = true;
            InitGame();
        }
        if (IsKeyPressed(KEY_TWO)) {
            gameTime = HARD_TIME;
            robotSpeed = HARD_ROBOT_SPEED;
            gameRunning = true;
            InitGame();
        }

        EndDrawing();
    }
}

void UpdateGame() {
    // Track time for normal update rate
    timeSinceLastUpdate += GetFrameTime();
    
    // Decrease the remaining time based on frame rate
    if (timeSinceLastUpdate >= secondsPerUpdate) {
        remainingTime--;
        timeSinceLastUpdate = 0.0f;
    }

    // If time is up, end the game
    if (remainingTime <= 0) {
        EndGame("YOU WON!");  // Time's up, player wins
    }

    // Spawn Robots Randomly
    if (GetRandomValue(0, 100) < 2) {
        SpawnRobot();
    }

    // Move Robots
    for (int i = 0; i < MAX_ROBOTS; i++) {
        if (robots[i].active) {
            robots[i].position.x -= robotSpeed;  // Move robot to the left
            if (robots[i].position.x < 0) {
                robots[i].active = false;  // Robot exits screen
                baseHealth--;  // Decrease base health if robot reaches the base
                if (baseHealth <= 0) {
                    EndGame("YOU LOST!");       // Player loses if base health is 0
                }
            }
        }
    }

    // Handle Plant Placement - allow placing plants anywhere based on mouse position
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePosition = GetMousePosition();
        PlacePlant(mousePosition);  // Place plant at mouse position
    }

    // Update Plant Shoot Timers and Auto-Shoot Projectiles
    for (int i = 0; i < LANE_COUNT; i++) {
        if (plants[i].active) {
            // Decrease the shoot timer
            plants[i].shootTimer--;
            if (plants[i].shootTimer <= 0) {
                // Shoot automatically if a robot is in the same lane
                ShootProjectile(i);
                // Reset shoot timer
                plants[i].shootTimer = 60;
            }
        }
    }

    // Update Projectiles
    UpdateProjectiles();

    // Check for Collisions
    CheckCollisions();
}

void DrawGame() {
    // Draw Grid (Green) - this can be removed if no lanes are used
    for (int i = 0; i < LANE_COUNT; i++) {
        DrawRectangle(0, i * 100, SCREEN_WIDTH, 100, GREEN);
    }

    // Draw Plants
    for (int i = 0; i < LANE_COUNT; i++) {
        if (plants[i].active) {
            DrawCircleV(plants[i].position, 20, DARKGREEN);
        }
    }

    // Draw Robots
    for (int i = 0; i < MAX_ROBOTS; i++) {
        if (robots[i].active) {
            DrawRectangleV(robots[i].position, (Vector2){40, 40}, BLUE);
        }
    }

    // Draw Projectiles (small, round peas)
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            DrawCircle(projectiles[i].position.x, projectiles[i].position.y, 5, RED);  // Small circular projectiles
        }
    }

    // Draw Score
    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    DrawText(scoreText, SCREEN_WIDTH - 150, 20, 20, BLACK);

    // Draw Timer
    char timerText[32];
    sprintf(timerText, "Time: %d", (int)remainingTime);
    DrawText(timerText, SCREEN_WIDTH - 150, 50, 20, BLACK);

    // Draw Health
    char healthText[32];
    sprintf(healthText, "Health: %d", baseHealth);
    DrawText(healthText, SCREEN_WIDTH - 150, 80, 20, BLACK);
}

void SpawnRobot() {
    for (int i = 0; i < MAX_ROBOTS; i++) {
        if (!robots[i].active) {
            robots[i].position = (Vector2){SCREEN_WIDTH, GetRandomValue(0, LANE_COUNT - 1) * 100 + 50};  // Random lane
            robots[i].active = true;
            robots[i].health = 3;  // Default health of robots
            break;
        }
    }
}

void PlacePlant(Vector2 position) {
    // Check if we already have 7 plants placed
    if (plantCount >= 7) {
        return;  // No more plants can be placed
    }

    // Place plant in the first available lane (where plants can be placed)
    for (int i = 0; i < LANE_COUNT; i++) {
        if (plants[i].active == false) {  // Find an inactive plant slot
            plants[i].position = position;
            plants[i].active = true;
            plants[i].shootTimer = 60;  // Reset the shoot timer for the new plant
            plantCount++;  // Increase the count of placed plants
            break;
        }
    }
}

void ShootProjectile(int lane) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].position = (Vector2){plants[lane].position.x + 20, plants[lane].position.y};
            projectiles[i].active = true;
            projectiles[i].lane = lane;
            break;
        }
    }
}

void UpdateProjectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].position.x += 5;  // Move projectiles to the right
            if (projectiles[i].position.x > SCREEN_WIDTH) {
                projectiles[i].active = false;  // Deactivate if it goes off-screen
            }
        }
    }
}

void CheckCollisions() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            for (int j = 0; j < MAX_ROBOTS; j++) {
                if (robots[j].active && CheckCollisionCircleRec(projectiles[i].position, 5, (Rectangle){robots[j].position.x, robots[j].position.y, 40, 40})) {
                    robots[j].health--;  // Decrease robot health on collision
                    projectiles[i].active = false;  // Deactivate the projectile
                    if (robots[j].health <= 0) {
                        robots[j].active = false;
                        score += 10;  // Increase score when robot is defeated
                    }
                    break;
                }
            }
        }
    }
}

void EndGame(const char* message) {
    gameOver = true;
    strcpy(endMessage, message);
}
