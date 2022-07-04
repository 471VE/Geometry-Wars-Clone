#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// backbuffer
extern uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
extern int game_score;
extern bool game_over;
extern int highscore;

#ifndef VK_ESCAPE
#  define VK_ESCAPE 0x1B
#  define VK_SPACE  0x20
#  define VK_LEFT   0x25
#  define VK_UP     0x26
#  define VK_RIGHT  0x27
#  define VK_DOWN   0x28
#  define VK_RETURN 0x0D
#  define VK_W      0x57
#  define VK_A      0x41
#  define VK_S      0x53
#  define VK_D      0x44
#  define VK_T      0x54
#endif

#define M_PI 3.141592741f
#define M_PI_2 1.570796371f
#define M_PI_4 0.7853981853f

#define TIME_BETWEEN_BULLETS 0.15f
#define SPAWNING_TIME 1.f
#define ARROW_FADE_TIME 0.1f
#define ENEMY_SCORE_TEXT_SIZE 10.f
#define MAX_HIGHLIGHT_TIME 0.1f
#define TIME_BETWEEN_ENEMIES 2.f
#define TIME_BETWEEN_ENEMIES_MULTIPLICATION_COEF 0.995f
#define GAME_SCORE_TEXT_SIZE 30
#define DEATH_TIME 1.5f

struct FPS {
	bool on = false;
	float button_press_time = 0.f;
};

// VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B' ...
bool is_key_pressed(int button_vk_code);

// 0 - left button, 1 - right button
bool is_mouse_button_pressed(int button);

int get_cursor_x();
int get_cursor_y();

bool is_window_active();

void clear_buffer();

void initialize();
void finalize();

void act(float dt, FPS& fps);
void draw();

void schedule_quit_game();

void log_error_and_exit(std::string message);

struct MessageToRender {
    MessageToRender(std::string font, std::string message, int x, int y, int size = GAME_SCORE_TEXT_SIZE,
		uint8_t color_r = 177, uint8_t color_g = 250, uint8_t color_b = 60, bool left_alignment = true)
        : font(font)
        , message(message)
        , x(x)
        , y(y)
		, size(size)
		, color_r(color_r)
		, color_g(color_g)
		, color_b(color_b)
		, left_alignment(left_alignment)
    {}

	std::string font;
	std::string message;
	int x, y, size;
	uint8_t color_r, color_g, color_b;
	bool left_alignment;
};

extern std::vector<MessageToRender> messages_to_render;
