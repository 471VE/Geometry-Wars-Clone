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
#include "Background.h"

bool title_screen = true;
float time_since_launch = 0.f;

bool controls = true;
float controls_fade_time = 0.f;

int game_score = 0;
int highscore;
bool game_over = false;

Player player;
BulletSet bullet_set;
EnemySet enemy_set(TIME_BETWEEN_ENEMIES, TIME_BETWEEN_ENEMIES_MULTIPLICATION_COEF);

Object restart_sign("assets/sprites/restart_sign.bmp", float(SCREEN_WIDTH / 2), float(SCREEN_HEIGHT - 110));
float wait_for_start = false;
float wait_for_start_time = 0.f;
float time_since_last_game_start = 0.f;

Object pause_sign("assets/sprites/pause_sign.bmp");
Object pause_info("assets/sprites/pause_info.bmp", float(SCREEN_WIDTH / 2), float(SCREEN_HEIGHT - 120));
bool paused = false;
float pause_time = 0;

Soundtrack game_soundtrack("game_soundtrack");
Soundtrack title_soundtrack("title_soundtrack");
Background background("assets/sprites/background.bmp");
Background border("assets/sprites/border.bmp");

Object start_sign("assets/sprites/start_sign.bmp", float(SCREEN_WIDTH / 2), float(SCREEN_HEIGHT - 110));
Background controls_info("assets/sprites/controls.bmp");

// initialize game data in this function
void initialize() {
	load_highscore(highscore);

	mciSendString("OPEN assets/music/game_over.mp3 ALIAS game_soundtrack",0,0,0);
	mciSendString("OPEN assets/music/black_rainbows.mp3 ALIAS title_soundtrack",0,0,0);

	mciSendString("OPEN assets/SFX/lose.mp3 ALIAS lose",0,0,0);
	mciSendString("SetAudio lose volume to 500",0,0,0);

	mciSendString("OPEN assets/SFX/restart.mp3 ALIAS restart",0,0,0);

	mciSendString("OPEN assets/SFX/explosion.mp3 ALIAS explosion",0,0,0);
	mciSendString("SetAudio explosion volume to 500",0,0,0);

	mciSendString("OPEN assets/SFX/shot.mp3 ALIAS shot",0,0,0);
	mciSendString("SetAudio shot volume to 300",0,0,0);

	mciSendString("OPEN assets/SFX/pause.mp3 ALIAS pause",0,0,0);

	title_soundtrack.start();
}

void scale_brightness(float factor) {
	float red, green, blue;
	uint32_t pixel;
	for (int x = 0; x < SCREEN_WIDTH; ++x) {
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			red = float((uint8_t)(buffer[y][x] >> 16));
			green = float((uint8_t)(buffer[y][x] >> 8));
			blue = float((uint8_t)buffer[y][x]);
			
			pixel = (((((0 << 8) | uint8_t(red * factor)) << 8) | uint8_t(green * factor)) << 8) | uint8_t(blue * factor);
			buffer[y][x] = pixel;
        }
    }
}


void act_title_screen(float dt) {
	if (controls && (is_key_pressed(VK_SPACE) ||
		is_key_pressed(VK_RETURN) || is_mouse_button_pressed(0))) {
			mciSendString("PLAY restart from 0",0,0,0);
			controls = false;
	}
	if (!controls)
		controls_fade_time += dt;
	
	time_since_launch += dt;

	if (wait_for_start)
		wait_for_start_time += dt;

	if (wait_for_start_time > DEATH_TIME)
		title_screen = false;

	if (!controls && is_key_pressed(VK_RETURN) && !wait_for_start && controls_fade_time >= DEATH_TIME / 2.f) {
		mciSendString("PLAY restart from 0",0,0,0);
		title_soundtrack.stop();
		game_soundtrack.start();
		wait_for_start = true;
		wait_for_start_time = 0;
	}
}

void act_game(float dt) {
	if (time_since_last_game_start > DEATH_TIME && is_key_pressed(VK_ESCAPE) && !paused && !player.isDead()) {
		mciSendString("PLAY pause from 0",0,0,0);
		paused = true;
		game_soundtrack.decrease_volume();
		pause_time = 0;
	}

	time_since_last_game_start += dt;	

	if (!paused) {
		player.update(dt);
		Point player_position = player.getCenter();

		bullet_set.update(player_position, dt);
		enemy_set.update(player_position, dt, bullet_set, player);
	} else if (is_key_pressed(VK_SPACE)) {
		mciSendString("PLAY pause from 0",0,0,0);
		paused = false;
		game_soundtrack.increase_volume();
	} else {
		pause_time += dt;
		float transparency_root = std::sin(M_PI_2 * pause_time);
		pause_sign.make_transparent(transparency_root * transparency_root, true);
	}

	messages_to_render.push_back(MessageToRender("Arcade", "Score", 20, 20));
	messages_to_render.push_back(MessageToRender("Arcade", std::to_string(game_score), 20, 45));

	messages_to_render.push_back(MessageToRender(
		"Arcade", "Highscore", SCREEN_WIDTH - 20, 20, GAME_SCORE_TEXT_SIZE, 177, 250, 60, false));
	messages_to_render.push_back(MessageToRender(
		"Arcade", std::to_string(highscore), SCREEN_WIDTH - 20, 45, GAME_SCORE_TEXT_SIZE, 177, 250, 60, false));

	if (wait_for_start) {
		if (enemy_set.getNumberOfEnemies() == 0) {
			game_over = false;
			game_score = 0;
			player = Player();
			enemy_set.reset();
			wait_for_start = false;
			wait_for_start_time = 0;
			time_since_last_game_start = 0;
		} else
			wait_for_start_time += dt;
	}
	if (game_over && is_key_pressed(VK_RETURN)) {
		wait_for_start = true;
		enemy_set.explodeAll();
		mciSendString("PLAY restart from 0",0,0,0);
		game_soundtrack.increase_volume();
	}
}


void act(float dt, FPS& fps) {
	if (is_key_pressed(VK_F1))
		schedule_quit_game();

	if (is_key_pressed(VK_T)) {
		if (!fps.button_press_time) fps.on = !fps.on;
		fps.button_press_time += dt;
	} else {
		fps.button_press_time = 0.f;
	}

	messages_to_render.clear();
	if (fps.on)
		messages_to_render.push_back(MessageToRender("Arcade", "FPS: " + std::to_string(int(1.f / dt)), 20, 70));

	if (title_screen)
		act_title_screen(dt);
	else
		act_game(dt);
	
	game_soundtrack.update(dt);
	title_soundtrack.update(dt);
}

void draw_title_screen() {
	clear_buffer();
	background.draw();

	if (time_since_launch < DEATH_TIME) {
		scale_brightness((std::max)(0.f, time_since_launch / DEATH_TIME / 3.f));
		border.draw_with_transparency(time_since_launch / DEATH_TIME);
	}	
	else {
		scale_brightness(1 / 3.f);
		border.draw_with_transparency();
	}
		
	if (controls && time_since_launch < DEATH_TIME) {
		float transparency_root = std::sin(M_PI_2 * time_since_launch / DEATH_TIME);
		controls_info.draw_with_transparency(transparency_root * transparency_root);
	} else if (controls && time_since_launch >= DEATH_TIME)
		controls_info.draw_with_transparency();
	
	if (!controls && controls_fade_time < DEATH_TIME / 2.f) {
		float transparency = (std::max)(0.f, std::cos(M_PI * controls_fade_time / DEATH_TIME));
		controls_info.draw_with_transparency(transparency);
	}

	if (!controls && controls_fade_time >= DEATH_TIME / 2.f) {
		if (!wait_for_start) {
			float transparency_root = std::sin(M_PI_2 * (controls_fade_time - DEATH_TIME / 2.f));
			start_sign.make_transparent(transparency_root * transparency_root, true);
			start_sign.draw();
		} else {
			float transparency_root = std::sin(2 * M_PI * wait_for_start_time - M_PI_4);
			start_sign.make_transparent(float((transparency_root * transparency_root) > 0.5), true);
			start_sign.draw();
		}		
	}	
}

void draw_game() {
	clear_buffer();
	background.draw();

	if (!player.isDeadCompletely())
		player.draw(paused);

	enemy_set.draw();
	bullet_set.draw();

	if (time_since_last_game_start < DEATH_TIME) {
		scale_brightness((std::min)(1.f, (1 + 2 * time_since_last_game_start / DEATH_TIME) / 3.f));
	} else if (player.isDead()) {
		if (!player.isDeadCompletely())
			scale_brightness(1 - 2 / 3.f * player.getDeathTime() / DEATH_TIME);
		else {
			scale_brightness(1 / 3.f);
		}
		if (!wait_for_start) {
			float transparency_root = std::sin(M_PI_2 * player.getDeathTime());
			restart_sign.make_transparent(transparency_root * transparency_root, true);
			restart_sign.draw();
		} else {
			float transparency_root = std::sin(2 * M_PI * wait_for_start_time - M_PI_4);
			restart_sign.make_transparent(float((transparency_root * transparency_root) > 0.5), true);
			restart_sign.draw();
		}
	} else if (paused) {
		scale_brightness(1 / 3.f);
		pause_sign.draw();
		pause_info.draw();
	}
	border.draw_with_transparency();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per A, R, G, B)
void draw() {
	if (title_screen)
		draw_title_screen();
	else
		draw_game();
}

// free game data in this function
void finalize() {
	mciSendString("close shot",0,0,0);
	mciSendString("close explosion",0,0,0);
	mciSendString("close game_soundtrack",0,0,0);
	mciSendString("close title_soundtrack",0,0,0);
	mciSendString("close restart",0,0,0);
	mciSendString("close lose",0,0,0);
	mciSendString("close pause",0,0,0);
}
