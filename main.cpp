#include "raylib.h"
#include "raymath.h"
#include <ctime>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>

int main() {
    #define NUM_PIPES 3

    const int WINDOW_WIDTH{512};
    const int WINDOW_HEIGHT{288};

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Flappy Bird");
    SetTargetFPS(60);
    InitAudioDevice();
    srand(time(NULL));

    Texture2D background = LoadTexture("resources/background.png");
    Texture2D ground = LoadTexture("resources/ground.png"); 
    Texture2D bird = LoadTexture("resources/bird.png"); 
    Texture2D pipe = LoadTexture("resources/pipe.png"); 
    Texture2D pipe2 = LoadTexture("resources/pipe2.png"); 
    Music music = LoadMusicStream("resources/music.mp3");   
    PlayMusicStream(music);

    float groundY = WINDOW_HEIGHT - ground.height;
    int backgroundScroll{0};
    int groundScroll{0}; 
    int backgroundSpeed{1}; 
    int groundSpeed{3}; 
    int backgroundLP{413};
    float gravity{2};  
    float antiGravity{22.0};
    float velocity{0.0};  
    bool gameOver{false}; 
    int score{0};
    int finalScore{0};  
    Vector2 pipePositions[NUM_PIPES];
    bool passedPipes[NUM_PIPES];
    bool gameStart{false};
    float elapsedTime = 0.0;

    // Initialize the array of pipes
    for (int i = 0; i < NUM_PIPES; i++) {
        pipePositions[i] = {static_cast<float>(WINDOW_WIDTH + 200 * i), static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)))}; 
        passedPipes[i] = false;
    }

    while (!gameStart && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(background, backgroundScroll, 0, WHITE);
        DrawText("Flappy Bird!", WINDOW_WIDTH / 2 - MeasureText("Flappy Bird!", 40) / 2, WINDOW_HEIGHT / 2 - 30, 40, YELLOW);
        DrawText("Press 'S' to start", WINDOW_WIDTH / 2 - MeasureText("Press 'S' to start", 20) / 2, (WINDOW_HEIGHT / 2) + 12, 20, WHITE);

        EndDrawing();

        if (IsKeyPressed(KEY_S)) {
            gameStart = true;
        }
    }

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        elapsedTime += GetFrameTime();

        backgroundScroll = (backgroundScroll - backgroundSpeed + static_cast<int>(GetFrameTime())) % backgroundLP;
        groundScroll = (groundScroll - groundSpeed + static_cast<int>(GetFrameTime())) % WINDOW_WIDTH;  

        // Update the positions of the pipes
        for (int i = 0; i < NUM_PIPES; i++) {
            pipePositions[i].x -= groundSpeed;

            // Check if the pipe has gone off the screen
            if (pipePositions[i].x <= -pipe.width) {
                // If it has, reset the position and set the passed flag to false
                pipePositions[i].x = WINDOW_WIDTH;
                pipePositions[i].y = static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)));
                passedPipes[i] = false;
            }
        }

        if (elapsedTime >= 0.1) {
        velocity += gravity;
        }
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            velocity -= antiGravity;
            elapsedTime = 0;
        }
        float birdY = (WINDOW_HEIGHT / 2) - (bird.height / 2) + velocity;
        birdY += velocity * (GetFrameTime() * 40);

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(background, backgroundScroll, 0, WHITE);

        // Draw the pipes
        for (int i = 0; i < NUM_PIPES; i++) {
            DrawTexture(pipe, pipePositions[i].x, pipePositions[i].y, WHITE);
            DrawTextureEx(pipe2, (Vector2){pipePositions[i].x, pipePositions[i].y - (pipe.height + 100)}, 0, 1.0f, WHITE);

            // Check if the bird has passed the pipe
            if (!passedPipes[i] && pipePositions[i].x + pipe.width < (WINDOW_WIDTH / 2) - (bird.width / 2)) {
                // If it has, set the passed flag to true and increase the score
                passedPipes[i] = true;
                score++;
            }
        }

        DrawTexture(ground, groundScroll, groundY, WHITE);
        DrawTexture(bird, (WINDOW_WIDTH / 2) - (bird.width / 2), birdY, WHITE);

        // ************************************************************** DEBUG ************************************************************
        // Debug lines for the pipes - PIPE - RECTANGLE
        // for (int i = 0; i < NUM_PIPES; i++) {
        //     DrawRectangleLines(pipePositions[i].x + pipe.width / 4, pipePositions[i].y + 7, pipe.width / 2, pipe.height, RED);
        //     DrawRectangleLines(pipePositions[i].x + pipe.width / 4, pipePositions[i].y - (pipe.height + 100) - 7, pipe.width / 2, pipe.height, RED);
        // }
        // Debug RECTANGLE lines for the bird - BIRD - RECTANGLE
        // DrawRectangleLines(windowWidth / 2 - bird.width / 2, birdY, bird.width, bird.height, RED);
        // Debug CIRCLE lines for the bird - BIRD - CIRCLE
        // float radius = std::max(bird.width, bird.height) / 3.0f;
        // DrawCircleLines(WINDOW_WIDTH / 2, birdY - -(bird.height / 2), radius, RED);
        // *********************************************************************************************************************************

        // COLLISION
       float radius = std::max(bird.width, bird.height) / 2.0f;
        Vector2 circleCenter = {static_cast<float>(WINDOW_WIDTH / 2), static_cast<float>(birdY - (bird.height / 3) + radius)};
        float circleRadius = radius;

        for (int i = 0; i < NUM_PIPES; i++) {
            if (CheckCollisionCircleRec(circleCenter, circleRadius, {static_cast<float>(pipePositions[i].x + pipe.width / 4), static_cast<float>(pipePositions[i].y + 7), static_cast<float>(pipe.width / 2), static_cast<float>(pipe.height)})) {
                gameOver = true;
            }

            if (CheckCollisionCircleRec(circleCenter, circleRadius, {static_cast<float>(pipePositions[i].x + pipe.width / 4), static_cast<float>(pipePositions[i].y - (pipe.height + 100) - 7), static_cast<float>(pipe.width / 2), static_cast<float>(pipe.height)})) {
                gameOver = true;
            }
        }

        if (gameOver) {
            ClearBackground(WHITE);
            DrawTexture(background, backgroundScroll, 0, WHITE);
            DrawText("Game Over!", WINDOW_WIDTH / 2 - MeasureText("Game Over!", 40) / 2, WINDOW_HEIGHT / 2 - 30, 40, RED);
            DrawText("Press 'R' to restart", WINDOW_WIDTH / 2 - MeasureText("Press 'R' to restart", 20) / 2, (WINDOW_HEIGHT / 2) + 5, 20, WHITE);

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                score = 0;
                velocity = 0;
                for (int i = 0; i < NUM_PIPES; i++) {
                    passedPipes[i] = false;
                    pipePositions[i].x = WINDOW_WIDTH + 200 * i;
                    pipePositions[i].y = static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)));
                }
            }
        }
        else {
            if (birdY <= 0 || birdY + bird.height >= groundY) {
                gameOver = true;
            }
        }

        // SCORE
        if (!gameOver) finalScore = score; 

        if (gameOver)
        {
            char text[100];
            snprintf(text, sizeof(text), "Final Score: %i", finalScore);
            DrawText(text, 10, 10, 20, WHITE);
        }
        else
        {
            char text[100];
            snprintf(text, sizeof(text), "Score: %i", score);
            DrawText(text, 10, 10, 20, WHITE);
        }
        EndDrawing();
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
}
