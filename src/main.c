#include "include/main.h"

// Globals
const int WIDTH = 768;
const int HEIGHT = 600;
const int PLAYER_SPEED = 4;
const int ENEMY_SPEED = 2;
const int PLAYER_SIZE = 64;
const float PLAYER_Y = HEIGHT - (PLAYER_SIZE * 2.5f);
const int DROP_RATE = 4;
const Vector2 DROP_SIZE = (Vector2){8, 16};

// Drops from the cloud enemy
typedef struct {
    Rectangle body;
    bool visible;
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

    // Characters
    Rectangle player = (Rectangle){WIDTH/2, PLAYER_Y, PLAYER_SIZE, PLAYER_SIZE};
    Rectangle enemy = (Rectangle){WIDTH/2, PLAYER_SIZE * 2.5f, PLAYER_SIZE, PLAYER_SIZE};


    // Build barriers
    Rectangle leftBarrier = (Rectangle){-PLAYER_SIZE, 0, PLAYER_SIZE, HEIGHT};
    Rectangle rightBarrier = (Rectangle){WIDTH, 0, PLAYER_SIZE, HEIGHT};

    // Initialize enemy properties
    int enemyDir = 1;
    unsigned int dropIndex = 0;
    float dropTime = 0.0f;
    Vector2 dropCoords = (Vector2){enemy.x + PLAYER_SIZE/2, enemy.y + PLAYER_SIZE/2};

    Drop drops[5] = {
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false},
        (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false}
    };

    // Game loop
    while(!WindowShouldClose()) {
        // Control drops
        for(unsigned int i = 0; i < 5; i++) {
            if(drops[i].visible) drops[i].body.y += DROP_RATE;
            else {
                dropCoords = (Vector2){enemy.x + PLAYER_SIZE/2, enemy.y + PLAYER_SIZE/2};
                drops[i].body.x = dropCoords.x;
                drops[i].body.y = dropCoords.y;
            }

            if(drops[i].body.y > HEIGHT) drops[i].visible = false;
        }

        if(dropTime >= DROP_RATE) {
            drops[dropIndex].visible = true;
            dropIndex = (dropIndex + 1) % 5;
            dropTime = 0.0f;
        }

        dropTime += 0.1f;

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

        // Let the enemy move
        if(CheckCollisionRecs(enemy, leftBarrier) || CheckCollisionRecs(enemy, rightBarrier))
            enemyDir *= -1;
        
        enemy.x += enemyDir*ENEMY_SPEED;

        // Drawing phase
        BeginDrawing();

            // Update the screen
            ClearBackground(SKYBLUE);

            // Create shapes
            DrawRectangleRec(player, BLUE);

            // Drops
            for(unsigned int i = 0; i < 5; i++) {
                if(drops[i].visible) DrawRectangleRec(drops[i].body, DARKBLUE);
            }

            DrawRectangleRec(enemy, WHITE);
            DrawRectangleRec(leftBarrier, BLACK);
            DrawRectangleRec(rightBarrier, BLACK);

            DrawFPS(10, 10);

        EndDrawing();
    }
    
    // Exit
    CloseWindow();
    return 0;
}