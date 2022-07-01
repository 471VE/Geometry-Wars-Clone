#pragma once

#include <unordered_set>

#include "GameObject.h"

class Bullet: public GameObject {
    public:
        Bullet(const char* fname, float velocity = 600.f,  float angular_velocity = 0.f)
            : GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f, angular_velocity, velocity)
            , m_direction(Point(0.f, 1.f))
        {}

        Bullet(const Bullet& bullet, Point center);
        void move(float dt);

    protected:
        Point m_direction;
};


class BulletSet {
    public:
        BulletSet();
        void update(float dt, Point player_center);
        void draw();
            
    protected:
        Bullet original_bullet;
        std::unordered_set<Bullet*> bullets;
        float time_elapsed_since_last_bullet;
};