#pragma once

#include <stdint.h>
#include <string>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// backbuffer
extern uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

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
