#pragma once

#include "Sprite.h"

class PlayerSprite: public Sprite {
    public:
        PlayerSprite(const char *fname, float angular_velocity = 2 * M_PI, float velocity = 200.f):
            Sprite(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f, angular_velocity, velocity)
        {}
        Point getMovementDirection();
        void moveWithInertiaAndRotation(float dt);
};

class PlayerArrow: public Sprite {
    public:
        PlayerArrow(const char *fname):
            Sprite(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f)
        {}        
        void rotateToMouseDirection();
        void setCenter(float x, float y);
};

class Player {
    public:
        Player(float angular_velocity = 2 * M_PI, float velocity = 200.f)
            : player_sprite(PlayerSprite("assets/sprites/player_main.bmp", angular_velocity, velocity))
            , player_arrow(PlayerArrow("assets/sprites/player_arrow.bmp"))
        {}

        void update(float dt);
        void draw() {
            player_sprite.draw();
            player_arrow.draw();
        }

    protected:
        PlayerSprite player_sprite;
        PlayerArrow player_arrow;
};