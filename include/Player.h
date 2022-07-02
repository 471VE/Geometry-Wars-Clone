#pragma once

#include "GameObject.h"

class PlayerSprite: public GameObject {
    public:
        PlayerSprite(const char *fname, float angular_velocity = 2 * M_PI, float velocity = 200.f):
            GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f, angular_velocity, velocity)
        {}

        
        void getToAngle(float angle, float dt, float difference, float angular_velocity);
        void getToAngleUniform(float angle, float dt);
        void getToAngleNonUniform(float angle, float dt, float time = 0.3f);
        
        Point getMovementDirection();
        void moveWithInertiaAndRotation(float dt);
};

class PlayerArrow: public GameObject {
    public:
        PlayerArrow(const char *fname)
            : GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f)
            , m_direction(Point(float(get_cursor_x()) - m_centerX, float(get_cursor_x()) - m_centerY))
        {}

        Point getDirection() { return m_direction; }

        void rotateToMouseDirection();
        void setCenter(float x, float y);
    
    protected:
        Point m_direction;
};

class Player {
    public:
        Player(float angular_velocity = 0.1f, float velocity = 300.f)
            : player_sprite(PlayerSprite("assets/sprites/player_main.bmp", angular_velocity, velocity))
            , player_arrow(PlayerArrow("assets/sprites/player_arrow.bmp"))
        {}

        Point getDirection() {
            return player_arrow.getDirection();
        }

        Point getCenter() {
            return Point(player_sprite.getCenterX(), player_sprite.getCenterY());
        }

        void update(float dt);
        void draw() {
            player_sprite.draw();
            player_arrow.draw();
        }

    protected:
        PlayerSprite player_sprite;
        PlayerArrow player_arrow;
};