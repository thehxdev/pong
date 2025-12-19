#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

// Type cast
#ifdef __cplusplus
    #define CAST(T, V) (static_cast<T>((V)))
#else
    #define CAST(T, V) ((T)(V))
#endif

#define SCORE_BUF_SIZE (4)
#define BALL_RADIUS (20)

// Player side
enum {
    P_LEFT,
    P_RIGHT,

    P_COUNT
};

typedef struct {
    Rectangle rect;
    int score, key_up, key_down;
} Player;

static const Color
    BACKGROUND_COLOR = {0x18, 0x18, 0x18, 0xff},
    SCORE_COLOR = {0x44, 0x44, 0x4e, 0xff},
    ENTITIES_COLOR = RAYWHITE;

static const char WINDOW_NAME[] = "Pong!";

static const int
    BALL_Y_VELOCITY_RANGE = 5, // ball Y velocity must be less than -5 or greater than 5
    FONT_SIZE = 300,
    RECTANGLE_FACTOR = 10,
    TARGET_FPS = 60;

static const float
    PLAYER_VELOCITY = 10.f, // up and down velocity of rectangles
    RECTANGLE_WIDTH = 15.f,
    BALL_X_VELOCITY = 15.f,
    WINDOW_SCALE = 0.8f;

static Font default_font;

static float
    rectangle_heigth;

static int
    monitor_id = 0,
    window_width = 0,
    window_height = 0;

static inline int randint(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

int main(void) {
    int i;
    struct {
        Vector2 pos, velocity;
        int radius;
    } ball = {0};
    Player players[2] = {0};

    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(window_width, window_height, WINDOW_NAME);

    monitor_id = GetCurrentMonitor();
    window_width = GetMonitorWidth(monitor_id) * WINDOW_SCALE;
    window_height = GetMonitorHeight(monitor_id) * WINDOW_SCALE;
    default_font = GetFontDefault();

    SetWindowSize(window_width, window_height);
    SetTargetFPS(TARGET_FPS);

    // Initialize world
    {
        ball.pos = (Vector2){ CAST(float, window_width/2), CAST(float, window_height/2) };

        // Ball y axis velocity
        int yv = randint(-BALL_Y_VELOCITY_RANGE, BALL_Y_VELOCITY_RANGE);
        if (yv > (-BALL_Y_VELOCITY_RANGE) && yv < BALL_Y_VELOCITY_RANGE) {
            if (yv >= 0) {
                yv += BALL_Y_VELOCITY_RANGE;
            } else {
                yv -= BALL_Y_VELOCITY_RANGE;
            }
        }

        ball.velocity = (Vector2){
            .x = (time(NULL) & 1) ? -BALL_X_VELOCITY : BALL_X_VELOCITY,
            .y = CAST(float, yv)
        };
        ball.radius = BALL_RADIUS;

        rectangle_heigth = CAST(float, window_height / RECTANGLE_FACTOR);
        const float rect_y_pos = CAST(float, (window_height / 2) - (rectangle_heigth / 2));

        players[P_LEFT] = (Player){
            .rect = (Rectangle){
                0.f,
                rect_y_pos,
                RECTANGLE_WIDTH,
                rectangle_heigth,
            },
            .score = 0,
            .key_up = KEY_W,
            .key_down = KEY_S,
        };

        players[P_RIGHT] = (Player){
            .rect = (Rectangle){
                CAST(float, window_width - RECTANGLE_WIDTH),
                rect_y_pos,
                RECTANGLE_WIDTH,
                rectangle_heigth,
            },
            .score = 0,
            .key_up = KEY_UP,
            .key_down = KEY_DOWN,
        };
    }

    while (!WindowShouldClose()) {
        // Update
        {
            // Move entities in the world
            for (i = 0; i < P_COUNT; i++) {
                Player *p = &players[i];
                float *rect_y_pos = &p->rect.y;
                if (IsKeyDown(p->key_up)) {
                    *rect_y_pos -= PLAYER_VELOCITY;
                    if (*rect_y_pos <= 0)
                        *rect_y_pos = 0;
                } else if (IsKeyDown(p->key_down)) {
                    *rect_y_pos += PLAYER_VELOCITY;
                    if (*rect_y_pos >= (window_height - rectangle_heigth))
                        *rect_y_pos = (window_height - rectangle_heigth);
                }
            }

            ball.pos.x += ball.velocity.x;
            ball.pos.y += ball.velocity.y;

            // Handle Collisions
            {
                Rectangle *rect = &players[P_RIGHT].rect;
                if (CheckCollisionCircleRec(ball.pos, ball.radius, *rect)) {
                    ball.pos.x = rect->x - ball.radius;
                    ball.velocity.x *= -1;
                }
                rect = &players[P_LEFT].rect;
                if (CheckCollisionCircleRec(ball.pos, ball.radius, *rect)) {
                    ball.pos.x = rect->x + rect->width + ball.radius;
                    ball.velocity.x *= -1;
                }

                if (ball.pos.x <= ball.radius) {
                    // ball hit left wall
                    ball.velocity.x *= -1;
                    ball.pos.x = ball.radius;
                    players[P_RIGHT].score += 1;
                } else if (ball.pos.x >= (window_width - ball.radius)) {
                    // ball hit right wall
                    ball.pos.x = (window_width - ball.radius);
                    ball.velocity.x *= -1;
                    players[P_LEFT].score += 1;
                }

                if (ball.pos.y <= ball.radius) {
                    // ball hit top of window
                    ball.pos.y = ball.radius;
                    ball.velocity.y *= -1;
                } else if (ball.pos.y >= (window_height - ball.radius)) {
                    // ball hit bottom of window
                    ball.pos.y = window_height - ball.radius;
                    ball.velocity.y *= -1;
                }
            }
        }

        // Draw
        BeginDrawing();
        {
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            // Draw scores
            {
                Vector2 t_size, t_pos;
                char score_buf[SCORE_BUF_SIZE] = {0};

                snprintf(score_buf, sizeof(score_buf), "%d", players[P_LEFT].score);
                t_size = MeasureTextEx(default_font, score_buf, FONT_SIZE, 0);
                t_pos.x = (window_width/4) - (t_size.x/2);
                t_pos.y = (window_height/2) - (t_size.y/2);
                DrawText(score_buf, t_pos.x, t_pos.y, FONT_SIZE, SCORE_COLOR);

                snprintf(score_buf, sizeof(score_buf), "%d", players[P_RIGHT].score);
                t_size = MeasureTextEx(default_font, score_buf, FONT_SIZE, 0);
                t_pos.x = (window_width * .75f) - (t_size.x/2);
                DrawText(score_buf, t_pos.x, t_pos.y, FONT_SIZE, SCORE_COLOR);
            }

            // Draw players
            for (i = 0; i < P_COUNT; i++)
                DrawRectangleRec(players[i].rect, RAYWHITE);

            // Draw ball
            DrawCircleV(ball.pos, ball.radius, RAYWHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
