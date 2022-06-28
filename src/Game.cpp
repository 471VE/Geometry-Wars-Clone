#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include "Primitives.h"


//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

Circle circle = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 20};

// initialize game data in this function
void initialize()
{
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();

	if (is_key_pressed(VK_W))
		circle.moveUp(dt);

	if (is_key_pressed(VK_S))
		circle.moveDown(dt);

	if (is_key_pressed(VK_A))
		circle.moveLeft(dt);

	if (is_key_pressed(VK_D))
		circle.moveRight(dt);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B, A)
void draw()
{
	// clear backbuffer
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
	draw_circle(circle, 0xFFFFFFFF);

}

// free game data in this function
void finalize()
{
}