#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include "Primitives.h"
#include "Sprite.h"

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

//Circle player = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 20};
Sprite player("assets/3.bmp");
Sprite arrow("assets/2.bmp");

// initialize game data in this function
void initialize() {
}


// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt, FPS& fps) {
	if (is_key_pressed(VK_ESCAPE))
		schedule_quit_game();
	if (is_key_pressed(VK_T)) {
		if (!fps.button_press_time) fps.on = !fps.on;
		fps.button_press_time += dt;
	} else {
		fps.button_press_time = 0.f;
	}

	Point direction(0.f, 0.f);

	if (is_key_pressed(VK_W))
		direction.y += 1.f;

	if (is_key_pressed(VK_S))
		direction.y -= 1.f;

	if (is_key_pressed(VK_D))
		direction.x += 1.f;

	if (is_key_pressed(VK_A)) 
		direction.x -= 1.f;

	direction.normalize();
	player.moveWithInertia(direction, dt);

	arrow.rotateClockWise(dt);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per A, R, G, B)
void draw() {
	// clear backbuffer
	memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			buffer[y][x] = 0x000000000;
		}
	}
	player.draw();
	arrow.draw();

}

// free game data in this function
void finalize() {
}
