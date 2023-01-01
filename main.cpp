#include "raylib.h"
#include "raymath.h"
#include <ctime>
#include <unistd.h>
#include <stdio.h>

int main()
{
    const int windowWidth{512};
    const int windowHeight{288};
    InitWindow(windowWidth, windowHeight, "Flappy Bird");
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    Texture2D background = LoadTexture("resources/background.png");
    Texture2D ground = LoadTexture("resources/ground.png");
    Texture2D bird = LoadTexture("resources/bird.png");
    Texture2D pipe = LoadTexture("resources/pipe.png");
    Texture2D pipe2 = LoadTexture("resources/pipe2.png");

    Music music = LoadMusicStream("resources/music.mp3");
    PlayMusicStream(music);

    int groundY = windowHeight - ground.height;
    int backgroundScroll = 0;
    int groundScroll = 0;
    int backgroundSpeed = 1;
    int groundSpeed = 3;
    int backgroundLP = 413;
    double gravity = 2;
    double antiGravity = 37.0;
    double velocity = 0.0;
    bool gameOver = false;
    int score = 0;
    int finalScore = 0;

#define NUM_PIPES 3

    // Array to hold the positions of the pipes
    Vector2 pipePositions[NUM_PIPES];

    // Array to hold the flags for whether a pipe has been passed or not
    bool passedPipes[NUM_PIPES];

    // Initialize the array of pipes
    for (int i = 0; i < NUM_PIPES; i++)
    {
        pipePositions[i] = {static_cast<float>(windowWidth + 200 * i), static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)))};
        passedPipes[i] = false;
    }

    bool gameStart = false;

    while (!gameStart && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(background, backgroundScroll, 0, WHITE);

        DrawText("Flappy Bird!", windowWidth / 2 - MeasureText("Flappy Bird!", 40) / 2, windowHeight / 2 - 30, 40, YELLOW);
        DrawText("Press 'S' to start", windowWidth / 2 - MeasureText("Press 'S' to start", 20) / 2, (windowHeight / 2) + 12, 20, WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_S))
        {
            gameStart = true;
        }
    }

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        backgroundScroll = (backgroundScroll - backgroundSpeed + static_cast<int>(GetFrameTime())) % backgroundLP;
        groundScroll = (groundScroll - groundSpeed + static_cast<int>(GetFrameTime())) % windowWidth;

        // Update the positions of the pipes
        for (int i = 0; i < NUM_PIPES; i++)
        {
            pipePositions[i].x -= groundSpeed;

            // Check if the pipe has gone off the screen
            if (pipePositions[i].x <= -pipe.width)
            {
                // If it has, reset the position and set the passed flag to false
                pipePositions[i].x = windowWidth;
                pipePositions[i].y = static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)));
                passedPipes[i] = false;
            }
        }

        velocity += gravity;
        if (IsKeyPressed(KEY_SPACE))
        {
            velocity -= antiGravity;
        }
        int birdY = (windowHeight / 2) - (bird.height / 2) + velocity;

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(background, backgroundScroll, 0, WHITE);

        // Draw the pipes
        for (int i = 0; i < NUM_PIPES; i++)
        {
            DrawTexture(pipe, pipePositions[i].x, pipePositions[i].y, WHITE);
            DrawTextureEx(pipe2, (Vector2){pipePositions[i].x, pipePositions[i].y - (pipe.height + 100)}, 0, 1.0f, WHITE);

            // Check if the bird has passed the pipe
            if (!passedPipes[i] && pipePositions[i].x + pipe.width < (windowWidth / 2) - (bird.width / 2))
            {
                // If it has, set the passed flag to true and increase the score
                passedPipes[i] = true;
                score++;
            }
        }

        DrawTexture(ground, groundScroll, groundY, WHITE);
        DrawTexture(ground, groundScroll + ground.width, groundY, WHITE);
        DrawTexture(bird, (windowWidth / 2) - (bird.width / 2), birdY, WHITE);

        for (int i = 0; i < NUM_PIPES; i++)
        {
            if (CheckCollisionRecs({static_cast<float>((windowWidth / 2) - (bird.width / 2)), static_cast<float>(birdY), static_cast<float>(bird.width), static_cast<float>(bird.height)},
                                   {static_cast<float>(pipePositions[i].x), static_cast<float>(pipePositions[i].y), static_cast<float>(pipe.width), static_cast<float>(pipe.height)}))
            {
                gameOver = true;
            }
            if (CheckCollisionRecs({static_cast<float>((windowWidth / 2) - (bird.width / 2)), static_cast<float>(birdY), static_cast<float>(bird.width), static_cast<float>(bird.height)},
                                   {static_cast<float>(pipePositions[i].x), static_cast<float>(pipePositions[i].y - (pipe.height + 100)), static_cast<float>(pipe.width), static_cast<float>(pipe.height)}))
            {
                gameOver = true;
            }
        }

        if (gameOver)
        {
            ClearBackground(WHITE);
            DrawTexture(background, backgroundScroll, 0, WHITE);
            DrawText("Game Over!", windowWidth / 2 - MeasureText("Game Over!", 40) / 2, windowHeight / 2 - 30, 40, RED);
            DrawText("Press 'R' to restart", windowWidth / 2 - MeasureText("Press 'R' to restart", 20) / 2, (windowHeight / 2) + 5, 20, WHITE);
            if (IsKeyPressed(KEY_R))
            {
                gameOver = false;
                score = 0;
                velocity = 0.0;
                for (int i = 0; i < NUM_PIPES; i++)
                {
                    passedPipes[i] = false;
                    pipePositions[i].x = windowWidth + 200 * i;
                    pipePositions[i].y = static_cast<float>(((rand() % (pipe.height / 2)) + ((pipe.height - 100) / 2)));
                }
            }
        }
        else
        {
            if (birdY <= 0 || birdY + bird.height >= groundY)
            {
                gameOver = true;
            }
        }

        // SCORE

        if (!gameOver)
            finalScore = score;

        // Display the score or the final score depending on whether the game is over
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
