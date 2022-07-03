#pragma once

#include <unordered_set>

#include "GameObject.h"

class Bullet: public GameObject {
    public:
        Bullet(const char* fname, float velocity = 900.f,  float angular_velocity = 0.f)
            : GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f, angular_velocity, velocity)
            , m_direction(Point(0.f, 1.f))
        {}
        Bullet(const Bullet& bullet, Point center);

        void move(float dt);
        void initialMove();
        void update(float dt) {
            m_death_time += dt;
            if (m_dead)
                updateFragments(dt);
            else
                move(dt);
        }

        void explodeBullet();
        void drawBullet();
        
        inline bool isDead() { return m_dead; }
        inline bool isDeadCompletely() { return (m_death_time > m_max_death_time); }
        bool outsideScreen();

    protected:
        Point m_direction;
        bool m_dead = false;
};


class BulletSet {
    public:
        BulletSet();
        void update(const Point& player_center, float dt);
        void draw();

        std::unordered_set<Bullet*> m_bullets;
            
    protected:
        Bullet m_original_bullet;
        float m_time_elapsed_since_last_bullet;
};