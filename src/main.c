#include "include/main.h"

// Globals
const int WIDTH = 768;
const int HEIGHT = 600;
const int PLAYER_SPEED = 6;
const int PLAYER_SIZE = 64;
const int DROP_SPEED = 4;
const unsigned int DROP_AMOUNT = 10;
const float PLAYER_Y = HEIGHT - (PLAYER_SIZE * 2.5f);
const Vector2 DROP_SIZE = (Vector2){8, 16};

// Drop objects from the cloud enemy
typedef struct {
    Rectangle body;
    bool visible;
    bool lightning;
} Drop;

// Let the player move around
// @param left: reads buttons down representing LEFT
// @param right: reads buttons down representing RIGHT
// @return distance traveled
int movePlayer(bool left, bool right) {
    int x = (int)right - (int)left;
    return x * PLAYER_SPEED;
}

int main() {
    // Setup
    InitWindow(WIDTH, HEIGHT, "Splish! Splash!");
    SetTargetFPS(60);

    // Images
    Texture2D playerImg = LoadTexture("resources/umby.png");
    Texture2D enemyImg = LoadTexture("resources/cloud.png");
    Texture2D boltImg = LoadTexture("resources/bolt.png");

    // Characters
    Rectangle player = (Rectangle){WIDTH/2, PLAYER_Y, PLAYER_SIZE, PLAYER_SIZE};
    Rectangle enemy = (Rectangle){WIDTH/2, PLAYER_SIZE * 1.5f, PLAYER_SIZE, PLAYER_SIZE};

    // Initialize enemy properties
    int enemyDir = 1;
    int enemySpeed = 2;
    int dropRate;
    unsigned int dropIndex = 0;
    float dropTime = 0.0f;
    Vector2 dropCoords = (Vector2){enemy.x + PLAYER_SIZE/2, enemy.y + PLAYER_SIZE/2};
    Drop drops[10] = {
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false}
    };

    // Build barriers
    Rectangle leftBarrier = (Rectangle){-PLAYER_SIZE, 0, PLAYER_SIZE, HEIGHT};
    Rectangle rightBarrier = (Rectangle){WIDTH, 0, PLAYER_SIZE, HEIGHT};

    // Initialize game properties
    unsigned int score = 0;
    unsigned int lives = 3;

    // Game loop
    while(!WindowShouldClose()) {

        // Change enemy rates based on score
        if(score >= 1000) {
            enemySpeed = 6;
            dropRate = GetRandomValue(3, 7);
        }
        else if(score >= 500) {
            enemySpeed = 6;
            dropRate = GetRandomValue(4, 7);
        }
        else if(score >= 250) {
            enemySpeed = 5;
            dropRate = GetRandomValue(4, 8);
        }
        else if(score >= 100) {
            enemySpeed = 5;
            dropRate = GetRandomValue(5, 8);
        }
        else if(score >= 50) {
            enemySpeed = 4;
            dropRate = GetRandomValue(6, 8);
        }
        else if(score >= 25) {
            enemySpeed = 4;
            dropRate = GetRandomValue(6, 9);
        }
        else if(score >= 10) {
            enemySpeed = 3;
            dropRate = GetRandomValue(6, 10);
        }
        else if(score >= 5) {
            enemySpeed = 3;
            dropRate = GetRandomValue(7, 10);
        }
        else {
            enemySpeed = 2;
            dropRate = GetRandomValue(8, 10);
        }

        // Let the enemy move
        if(CheckCollisionRecs(enemy, leftBarrier) || CheckCollisionRecs(enemy, rightBarrier))
            enemyDir *= -1;
        
        enemy.x += enemyDir * enemySpeed;

        // Control drops
        for(unsigned int i = 0; i < DROP_AMOUNT; i++) {
            if(drops[i].visible) drops[i].body.y += DROP_SPEED;
            else {
                dropCoords = (Vector2){enemy.x + PLAYER_SIZE/2, enemy.y + PLAYER_SIZE/2};
                drops[i].body.x = dropCoords.x;
                drops[i].body.y = dropCoords.y;
                drops[i].lightning = (GetRandomValue(0, 10) >= 7);
            }

            if(drops[i].body.y > HEIGHT) drops[i].visible = false;
        }

        if(dropTime >= dropRate) {
            drops[dropIndex].visible = true;
            dropIndex = (dropIndex + 1) % DROP_AMOUNT;
            dropTime = 0.0f;
        }

        dropTime += 0.1f;

        // Check if a drop was caught
        for(unsigned int i = 0; i < DROP_AMOUNT; i++) {
            if(CheckCollisionRecs(player, drops[i].body)) {
                if(drops[i].lightning) lives--;
                else score++;

                drops[i].visible = false;
            }
        }

        // Gather the player's input
        bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

        if(left) {
            if(!CheckCollisionRecs(player, leftBarrier))
                player.x += movePlayer(left, right);
        }
        else if(right) {
            if(!CheckCollisionRecs(player, rightBarrier))
                player.x += movePlayer(left, right);
        }

        // Drawing phase
        BeginDrawing();

            // Update the screen
            ClearBackground(SKYBLUE);

            // Create shapes
            DrawTexture(playerImg, player.x, player.y, WHITE);

            // (Drops)
            for(unsigned int i = 0; i < DROP_AMOUNT; i++) {
                if(drops[i].visible) {
                    if(drops[i].lightning) DrawTexture(boltImg, drops[i].body.x, drops[i].body.y, WHITE);
                    else DrawRectangleRec(drops[i].body, DARKBLUE);
                }
            }

            DrawTexture(enemyImg, enemy.x, enemy.y, WHITE);
            DrawRectangleRec(leftBarrier, BLACK);
            DrawRectangleRec(rightBarrier, BLACK);

            // Text
            // DrawFPS(10, 10);
            DrawText(TextFormat("Score: %i", score), 10, 10, 24, BLACK);
            DrawText(TextFormat("Lives: %i", lives), 10, 44, 24, BLACK);

        EndDrawing();
    }
    
    // Exit
    CloseWindow();
    return 0;
}