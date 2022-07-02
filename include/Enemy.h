#pragma once

#include <unordered_set>
#include <algorithm>

#define NOMINMAX
#include <windows.h>

#include "GameObject.h"
#include "Player.h"
#include "Bullet.h"


class Enemy: public GameObject {
    public: 
        Enemy(const char* fname, float velocity = 200.f,  float angular_velocity = 0.f);
        Enemy(const Enemy& enemy);

        float getRandom(float a, float b);        
        void resize(float scaleX, float scaleY);
        void checkBoundaries(Point& direction);
        virtual void update(const Point& point, float dt) = 0;
};


class EnemyTypeOne: public Enemy {
    public:
        EnemyTypeOne(const char* fname, float velocity = 200.f,  float angular_velocity = 3 * M_PI, float rotational_velocity = M_PI / 8);
        EnemyTypeOne(const EnemyTypeOne& enemy);

        void update(const Point& point, float dt);

    protected:
        Point m_direction;
        float m_rotational_velocity;
};

class EnemyTypeTwo: public Enemy {
    public:
        EnemyTypeTwo(const char* fname, float velocity = 250.f,  float angular_velocity = 0.f);
        EnemyTypeTwo(const EnemyTypeTwo& enemy);

        void shapeShift();
        void moveToPoint(const Point& point, float dt);
        void update(const Point& point, float dt);

    protected:
        float m_lifetime;
        float m_current_scaleX;
        float m_current_scaleY;
        float m_omega;
};

class EnemyTypeThree: public Enemy {
    public:
        EnemyTypeThree(const char* fname, float min_velocity = 300.f, float max_velocity = 450.f,
            float min_angular_velocity = M_PI, float max_angular_velocity = 4 * M_PI, float angle_threshold = M_PI / 6);
        EnemyTypeThree(const EnemyTypeThree& enemy);

        void update_velocity(float&v, const float& min_v, const float& max_v, const float& angle);
        void update(const Point& point, float dt);

    protected:
        float m_min_velocity, m_max_velocity;
        float m_min_angular_velocity, m_max_angular_velocity;
        float m_angle_threshold;
        Point m_last_direction = Point(0.f, 0.f);
};

class EnemySet {
    public:
        EnemySet(float time_between_enemies = 2.f, float speedup_coefficient = 0.99f);

        void update(const Point& player_center, float dt, BulletSet& bullet_set);
        void draw();
        Enemy* chooseEnemy();

    protected:
        EnemyTypeOne original_enemy1;
        EnemyTypeTwo original_enemy2;
        EnemyTypeThree original_enemy3;
        std::unordered_set<Enemy*> enemies;
        float time_elapsed_since_last_enemy;
        float time_between_enemies;
        int enemies_created;
        float speedup_coefficient;
};