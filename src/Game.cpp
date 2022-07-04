#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <windows.h>

#include "Primitives.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Soundtrack.h"
#include "Highscore.h"

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

int game_score = 0;
int highscore;
bool game_over = false;

Player player;
BulletSet bullet_set;
EnemySet enemy_set(TIME_BETWEEN_ENEMIES, TIME_BETWEEN_ENEMIES_MULTIPLICATION_COEF);
Object restart_sign("assets/sprites/restart_sign.bmp", float(SCREEN_WIDTH / 2), float(SCREEN_HEIGHT - 110));
float wait_for_restart = false;



// initialize game data in this function
void initialize() {
	load_highscore(highscore);

	mciSendString("OPEN assets/music/game_over.mp3 ALIAS soundtrack",0,0,0);

	mciSendString("OPEN assets/SFX/lose.mp3 ALIAS lose",0,0,0);
	mciSendString("OPEN assets/SFX/restart.mp3 ALIAS restart",0,0,0);

	mciSendString("OPEN assets/SFX/explosion.mp3 ALIAS explosion",0,0,0);
	mciSendString("SetAudio explosion volume to 500",0,0,0);

	mciSendString("OPEN assets/SFX/shot.mp3 ALIAS shot",0,0,0);
	mciSendString("SetAudio shot volume to 300",0,0,0);

	mciSendString("OPEN assets/SFX/pause.mp3 ALIAS pause",0,0,0);

	mciSendString("SetAudio soundtrack volume to 0",0,0,0);
	mciSendString("PLAY soundtrack repeat",0,0,0);
}

void reduce_brightness(float factor) {
	float red, green, blue;
	uint32_t pixel;
	for (int x = 0; x < SCREEN_WIDTH; ++x) {
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			red = float((uint8_t)(buffer[y][x] >> 16));
			green = float((uint8_t)(buffer[y][x] >> 8));
			blue = float((uint8_t)buffer[y][x]);
			
			pixel = (((((0 << 8) | uint8_t(red / factor)) << 8) | uint8_t(green / factor)) << 8) | uint8_t(blue / factor);
			buffer[y][x] = pixel;
        }
    }
}

void pause_game() {

}

void draw_background() {

}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt, FPS& fps) {
	if (is_key_pressed(VK_ESCAPE)) {
		pause_game();
		mciSendString("PLAY pause from 0",0,0,0);
		schedule_quit_game();
	}
	if (is_key_pressed(VK_T)) {
		if (!fps.button_press_time) fps.on = !fps.on;
		fps.button_press_time += dt;
	} else {
		fps.button_press_time = 0.f;
	}

	player.update(dt);
	Point player_position = player.getCenter();

	bullet_set.update(player_position, dt);
	enemy_set.update(player_position, dt, bullet_set, player);

	messages_to_render.clear();
	messages_to_render.push_back(MessageToRender("Arcade", "Score", 20, 20));
	messages_to_render.push_back(MessageToRender("Arcade", std::to_string(game_score), 20, 45));

	messages_to_render.push_back(MessageToRender(
		"Arcade", "Highscore", SCREEN_WIDTH - 20, 20, GAME_SCORE_TEXT_SIZE, 177, 250, 60, false));
	messages_to_render.push_back(MessageToRender(
		"Arcade", std::to_string(highscore), SCREEN_WIDTH - 20, 45, GAME_SCORE_TEXT_SIZE, 177, 250, 60, false));

	if (fps.on)
		messages_to_render.push_back(MessageToRender("Arcade", "FPS: " + std::to_string(int(1.f / dt)), 20, 70));

	if (wait_for_restart) {
		if (enemy_set.getNumberOfEnemies() == 0) {
			game_over = false;
			game_score = 0;
			player = Player();
			enemy_set.reset();
			wait_for_restart = false;
		}
	}
	if (game_over && is_key_pressed(VK_RETURN)) {
		wait_for_restart = true;
		enemy_set.explodeAll();
		mciSendString("PLAY restart from 0",0,0,0);
	}
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per A, R, G, B)
void draw() {
	clear_buffer();

	draw_background();

	if (!player.isDeadCompletely())
		player.draw();

	enemy_set.draw();
	bullet_set.draw();

	if (player.isDead()) {
		if (!player.isDeadCompletely())
			reduce_brightness(1 + 2 * player.getDeathTime() / DEATH_TIME);
		else {
			reduce_brightness(3.f);
		}
		float transparency_root = std::sin(M_PI_2 * player.getDeathTime());
		restart_sign.make_transparent(transparency_root * transparency_root, true);
		restart_sign.draw();
	}
}

// free game data in this function
void finalize() {
	mciSendString("close shot",0,0,0);
	mciSendString("close explosion",0,0,0);
	mciSendString("close soundtrack",0,0,0);
	mciSendString("close restart",0,0,0);
	mciSendString("close lose",0,0,0);
	mciSendString("close pause",0,0,0);
}
