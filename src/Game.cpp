#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <windows.h>

#include "Primitives.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

//Circle player = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 20};


Player player;
BulletSet bullet_set;
EnemySet enemy_set(2.f, 0.99f);



// initialize game data in this function
void initialize() {
	mciSendString("OPEN assets/music/test.mp3 ALIAS sample",0,0,0);
	mciSendString("PLAY sample repeat",0,0,0);
}

void fade_audio() {

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

	if (is_key_pressed(VK_DOWN)) {
		mciSendString("PLAY sample from 0 repeat","",0,0);
	}
		//mciSendString("close MP3","",0,0);

	player.update(dt);
	Point player_position = player.getCenter();

	bullet_set.update(player_position, dt);
	enemy_set.update(player_position, dt, bullet_set);

	messages_to_render.clear();
	if (fps.on)
		messages_to_render.push_back(MessageToRender("Arcade", "FPS: " + std::to_string(int(1.f / dt)), 10, 10));
	messages_to_render.push_back(MessageToRender("outrun future", "Sasai", 100, 100, 100, 255, 0, 0));
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per A, R, G, B)
void draw() {
	// clear backbuffer
	clear_buffer();
	player.draw();
	enemy_set.draw();
	bullet_set.draw();
}

// free game data in this function
void finalize() {
	mciSendString("close shot",0,0,0);
	mciSendString("close explosion",0,0,0);
	mciSendString("close sample",0,0,0);
}
