#include "include/main.h"

// Globals
const int WIDTH = 768;
const int HEIGHT = 600;
const int PLAYER_SPEED = 6;
const int PLAYER_SIZE = 64;
const int DROP_SPEED = 4;
const int RISE_RATE = 10;
const int FONT_SPACE = 2;
const unsigned int OCEAN_CAP = 100;
const unsigned int DROP_AMOUNT = 10;
const float PLAYER_Y = HEIGHT - (PLAYER_SIZE * 2.5f);
const float OCEAN_RATE = 0.2f;
const float BLINK_RATE = 0.1f;
const float BLINK_CAP = 4.0f;
const float CONTINUE_RATE = 0.1f;
const float CONTINUE_CAP = 5.0f;
const Vector2 DROP_SIZE = (Vector2){8, 16};
const KeyboardKey accept = KEY_ENTER;

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

    InitAudioDevice();
    SetMasterVolume(0.75f);

    unsigned int sceneNo = 0;

    // Sprites
    Texture2D playerImg = LoadTexture("resources/umby.png");
    Texture2D enemyImg = LoadTexture("resources/cloud.png");
    Texture2D boltImg = LoadTexture("resources/bolt.png");

    // Sounds
    Sound catch = LoadSound("resources/catch.wav");
    Sound drop = LoadSound("resources/drop.wav");
    Sound sink = LoadSound("resources/sink.wav");
    Sound zap = LoadSound("resources/zap.wav");
    Sound boom = LoadSound("resources/boom.wav");

    // Font
    Font font = LoadFont("resources/barcade-brawl-font/BarcadeBrawlRegular-plYD.ttf");

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

    // Build setting
    Rectangle leftBarrier = (Rectangle){-PLAYER_SIZE, 0, PLAYER_SIZE, HEIGHT};
    Rectangle rightBarrier = (Rectangle){WIDTH, 0, PLAYER_SIZE, HEIGHT};

    float front = GetRandomValue(5, 15);
    float back = front + GetRandomValue(3, 7);

    Rectangle frontBuildings[15];
    Rectangle backBuildings[22];

    for(unsigned int i = 0; i < back; i++) {
        backBuildings[i] = (Rectangle){i*(WIDTH/back), HEIGHT - GetRandomValue(32, 128), WIDTH/back, HEIGHT};
    }

    for(unsigned int i = 0; i < front; i++) {
        frontBuildings[i] = (Rectangle){i*(WIDTH/front), HEIGHT - GetRandomValue(32, 128), WIDTH/front, HEIGHT};
    }

    // (Ocean)
    Rectangle ocean = (Rectangle){0, HEIGHT - PLAYER_SIZE/2, WIDTH, HEIGHT};
    Rectangle oceanTip = (Rectangle){ocean.x, ocean.y, WIDTH, 4};
    Rectangle oceanTipShade = (Rectangle){ocean.x, ocean.y, WIDTH, 12};
    Rectangle oceanReflection = (Rectangle){ocean.x, ocean.y, WIDTH, 24};

    unsigned int oceanHeight = 0;
    unsigned int oceanHeightTracker = 0;
    bool rise = true;

    // Initialize game properties
    unsigned int score = 0;
    unsigned int lives = 3;

    // Title screen
    Texture2D logoImg = LoadTexture("resources/logo.png");
    float blink = 0.0f;
    bool blank = false;

    // Game over
    bool _continue = false;
    float continueTime = 0.0f;

    // Game loop
    while(!WindowShouldClose()) {
        switch(sceneNo) {
            case 0:
                // Characters
                player = (Rectangle){WIDTH/2, PLAYER_Y, PLAYER_SIZE, PLAYER_SIZE};
                enemy = (Rectangle){WIDTH/2, PLAYER_SIZE * 1.5f, PLAYER_SIZE, PLAYER_SIZE};

                // Initialize enemy properties
                enemyDir = 1;
                enemySpeed = 2;
                dropIndex = 0;
                dropTime = 0.0f;
                dropCoords = (Vector2){enemy.x + PLAYER_SIZE/2, enemy.y + PLAYER_SIZE/2};

                for(unsigned int i = 0; i < DROP_AMOUNT; i++) {    
                    drops[i] = (Drop){(Rectangle){dropCoords.x, dropCoords.y, 8, 16}, false, false};
                }

                // Build setting
                leftBarrier = (Rectangle){-PLAYER_SIZE, 0, PLAYER_SIZE, HEIGHT};
                rightBarrier = (Rectangle){WIDTH, 0, PLAYER_SIZE, HEIGHT};

                front = GetRandomValue(5, 15);
                back = front + GetRandomValue(3, 7);

                for(unsigned int i = 0; i < back; i++) {
                    backBuildings[i] = (Rectangle){i*(WIDTH/back), HEIGHT - GetRandomValue(32, 128), WIDTH/back, HEIGHT};
                }

                for(unsigned int i = 0; i < front; i++) {
                    frontBuildings[i] = (Rectangle){i*(WIDTH/front), HEIGHT - GetRandomValue(32, 128), WIDTH/front, HEIGHT};
                }

                // (Ocean)
                ocean = (Rectangle){0, HEIGHT - PLAYER_SIZE/2, WIDTH, HEIGHT};
                oceanTip = (Rectangle){ocean.x, ocean.y, WIDTH, 4};
                oceanTipShade = (Rectangle){ocean.x, ocean.y, WIDTH, 12};
                oceanReflection = (Rectangle){ocean.x, ocean.y, WIDTH, 24};

                oceanHeight = 0;
                oceanHeightTracker = 0;
                rise = true;

                // Initialize game properties
                score = 0;
                lives = 3;

                // Game over
                _continue = false;
                continueTime = 0.0f;
            
                // Blinking stuff
                if(blink < BLINK_CAP && !blank) blink += BLINK_RATE;
                else {
                    blink -= BLINK_RATE;
                    blank = true;
                }

                if(blink <= 0.0f) {
                    blank = false;
                    blink = 0.0f;
                }

                // Start game
                if(IsKeyPressed(accept)) sceneNo = 1;

                // Display title screen
                BeginDrawing();

                    ClearBackground(BLACK);

                    DrawTexture(logoImg, WIDTH/2 - 256, -32, WHITE);
                    DrawTextEx(font, "Press ENTER to start", (Vector2){WIDTH/2 - 180, HEIGHT - 128}, 16, FONT_SPACE, blank ? BLANK : WHITE);

                EndDrawing();

                break;

            case 1:
                // Signal a game over
                if(lives <= 0 || CheckCollisionRecs(ocean, enemy)) sceneNo = 2;

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

                    if(drops[i].body.y > HEIGHT) {
                        drops[i].visible = false;
                        if(!drops[i].lightning) {
                            ocean.y -= RISE_RATE;
                            oceanHeightTracker++;
                        }
                        else if(oceanHeightTracker > 0) {
                            ocean.y += RISE_RATE;
                            oceanHeightTracker--;
                            PlaySound(sink);
                        }
                    }
                }

                if(dropTime >= dropRate) {
                    drops[dropIndex].visible = true;
                    PlaySound(drops[dropIndex].lightning ? zap : drop);
                    dropIndex = (dropIndex + 1) % DROP_AMOUNT;
                    dropTime = 0.0f;
                }

                dropTime += 0.1f;

                // Check if a drop was caught
                for(unsigned int i = 0; i < DROP_AMOUNT; i++) {
                    if(CheckCollisionRecs(player, drops[i].body)) {
                        if(drops[i].lightning) {
                            lives--;
                            PlaySound(boom);
                        }
                        else {
                            score++;
                            PlaySound(catch);
                        }

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

                // Move ocean
                if(oceanHeight < OCEAN_CAP && rise) {
                    ocean.y -= OCEAN_RATE;
                    oceanHeight++;
                }
                else {
                    ocean.y += OCEAN_RATE;
                    oceanHeight--;
                    rise = false;
                }

                if(oceanHeight == 0) rise = true;

                // (Other oceanic attributes)
                oceanTip.y = ocean.y;
                oceanTipShade.y = ocean.y;
                oceanReflection.y = ocean.y;

                // Drawing phase
                BeginDrawing();

                    // Update the screen
                    ClearBackground((Color){255, 169, 103, 255});
                    DrawRectangle(0, 0, WIDTH, HEIGHT/2, (Color){255, 152, 73, 255});
                    DrawRectangle(0, 0, WIDTH, HEIGHT/4, (Color){255, 128, 31, 255});
                    DrawRectangle(0, 0, WIDTH, HEIGHT/8, (Color){255, 111, 0, 255});

                    // Draw some nice buildings
                    for(unsigned int i = 0; i < back; i++) {
                        DrawRectangleRec(backBuildings[i], BLACK);
                    }

                    for(unsigned int i = 0; i < front; i++) {
                        DrawRectangleRec(frontBuildings[i], DARKGRAY);
                    }

                    // Create shapes
                    DrawTexture(playerImg, player.x, player.y, WHITE);

                    // (Drops)
                    for(unsigned int i = 0; i < DROP_AMOUNT; i++) {
                        if(drops[i].visible) {
                            if(drops[i].lightning)
                                DrawTexture(boltImg, drops[i].body.x - 32, drops[i].body.y - 32, WHITE);
                            else DrawRectangleRec(drops[i].body, DARKBLUE);
                        }
                    }

                    DrawTexture(enemyImg, enemy.x, enemy.y, WHITE);
                    DrawRectangleRec(leftBarrier, BLACK);
                    DrawRectangleRec(rightBarrier, BLACK);
                    DrawRectangleRec(ocean, BLUE);
                    DrawRectangleRec(oceanReflection, SKYBLUE);
                    DrawRectangleRec(oceanTipShade, (Color){141, 208, 255, 255});
                    DrawRectangleRec(oceanTip, WHITE);

                    // Text
                    // DrawFPS(10, 10);
                    DrawTextEx(font, TextFormat("Score: %i", score), (Vector2){10, 10}, 16, FONT_SPACE, BLACK);
                    DrawTextEx(font, TextFormat("Lives: %i", lives), (Vector2){10, 44}, 16, FONT_SPACE, BLACK);

                EndDrawing();

                break;

            case 2:
                // Control resetting the game
                if(continueTime < CONTINUE_CAP) continueTime += CONTINUE_RATE;
                else _continue = true;

                if(IsKeyPressed(accept) && _continue) {
                    
                    // (Reset title variables)
                    blink = 0.0f;
                    blank = false;

                    sceneNo = 0;
                }

                // Display game over screen
                BeginDrawing();

                    ClearBackground(RED);

                    DrawTextEx(font, "Game Over", (Vector2){WIDTH/2 - 256, HEIGHT/2 - 40}, 40, FONT_SPACE, MAROON);
                    DrawTextEx(font, TextFormat("Final Score: %i", score), (Vector2){WIDTH/2 - 256, HEIGHT/2 + 20}, 20, FONT_SPACE, MAROON);
                    DrawTextEx(font, "Press Enter to continue", (Vector2){WIDTH/2 - 256, HEIGHT/2 + 60}, 10, FONT_SPACE, _continue ? YELLOW : BLANK);

                EndDrawing();

                break;
        }
    }
    
    // Exit
    CloseAudioDevice();
    CloseWindow();
    return 0;
}