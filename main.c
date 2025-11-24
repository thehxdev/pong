#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

// Type cast
#ifdef __cplusplus
    #define SC(T, V) (static_cast<T>((V)))
#else
    #define SC(T, V) ((T)(V))
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
    background_color = {0x18, 0x18, 0x18, 0xff},
    score_color = {0x44, 0x44, 0x4e, 0xff},
    entities_color = RAYWHITE;

static const char window_name[] = "Pong!";

static const int
    ball_y_velocity_range = 5, // ball Y velocity must be less than -5 or greater than 5
    font_size = 300,
    target_fps = 60;

static const float
    rectangle_velocity = 10.f,
    rectangle_width = 15.f,
    ball_x_velocity = 15.f,
    window_scale = 0.8f,
    player_velocity = 5.f;

static Font default_font;

static float
    rectangle_heigth;

static int
    monitor_id = 0,
    window_width = 0,
    window_height = 0;


static struct {
    Vector2 pos, velocity;
    int radius;
} ball = {0};

static Player players[2] = {0};

static inline void window_center(Vector2 *out) {
    out->x = SC(float, window_width)/2;
    out->y = SC(float, window_height)/2;
}

int randint(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

int main(void) {
    int i;
    srand(time(NULL));
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(window_width, window_height, window_name);

    monitor_id = GetCurrentMonitor();
    window_width = GetMonitorWidth(monitor_id) * window_scale;
    window_height = GetMonitorHeight(monitor_id) * window_scale;
    default_font = GetFontDefault();

    SetWindowSize(window_width, window_height);
    SetTargetFPS(target_fps);

    // Initialize world
    {
        window_center(&ball.pos);

        // Ball y axis velocity
        int yv = randint(-ball_y_velocity_range, ball_y_velocity_range);
        if (yv > (-ball_y_velocity_range) && yv < ball_y_velocity_range) {
            if (yv >= 0) {
                yv += ball_y_velocity_range;
            } else {
                yv -= ball_y_velocity_range;
            }
        }

        ball.velocity = (Vector2){
            .x = (time(NULL) & 1) ? -ball_x_velocity : ball_x_velocity,
            .y = SC(float, yv)
        };
        ball.radius = BALL_RADIUS;

        rectangle_heigth = SC(float, window_height / 4);
        const float rect_y_pos = SC(float, (window_height / 2) - (rectangle_heigth / 2));

        players[P_LEFT] = (Player){
            .rect = (Rectangle){
                0.f,
                rect_y_pos,
                rectangle_width,
                rectangle_heigth,
            },
            .score = 0,
            .key_up = KEY_W,
            .key_down = KEY_S,
        };

        players[P_RIGHT] = (Player){
            .rect = (Rectangle){
                SC(float, window_width - rectangle_width),
                rect_y_pos,
                rectangle_width,
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
                    *rect_y_pos -= rectangle_velocity;
                    if (*rect_y_pos <= 0)
                        *rect_y_pos = 0;
                } else if (IsKeyDown(p->key_down)) {
                    *rect_y_pos += rectangle_velocity;
                    if (*rect_y_pos >= (window_height - rectangle_heigth))
                        *rect_y_pos = (window_height - rectangle_heigth);
                }
            }

            ball.pos.x += ball.velocity.x;
            ball.pos.y += ball.velocity.y;

            // Handle Collisions
            {
                // FIXME: This is not the correct way to handle collision between ball
                // and a player. The ball may get stuck in one rectangle. Fix this!
                // But it works for the most part!
                for (i = 0; i < P_COUNT; i++) {
                    if (CheckCollisionCircleRec(ball.pos, ball.radius, players[i].rect))
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
            ClearBackground(background_color);
            DrawFPS(10, 10);

            // Draw scores
            {
                Vector2 t_size, t_pos;
                char score_buf[SCORE_BUF_SIZE] = {0};

                snprintf(score_buf, sizeof(score_buf), "%d", players[P_LEFT].score);
                t_size = MeasureTextEx(default_font, score_buf, font_size, 0);
                t_pos.x = (window_width/4) - (t_size.x/2);
                t_pos.y = (window_height/2) - (t_size.y/2);
                DrawText(score_buf, t_pos.x, t_pos.y, font_size, score_color);

                snprintf(score_buf, sizeof(score_buf), "%d", players[P_RIGHT].score);
                t_size = MeasureTextEx(default_font, score_buf, font_size, 0);
                t_pos.x = (window_width * .75f) - (t_size.x/2);
                DrawText(score_buf, t_pos.x, t_pos.y, font_size, score_color);
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
