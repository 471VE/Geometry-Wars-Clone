#pragma once

#include <unordered_set>

#include "GameObject.h"
#include "Player.h"


class Enemy: public GameObject {
    public: 
        Enemy(const char* fname, float velocity = 200.f,  float angular_velocity = 0.f);
        Enemy(const Enemy& enemy);

        float getRandom(float a, float b);        
        void resize(float scaleX, float scaleY);

        virtual void update(const Point& point, float dt) = 0;
};


class EnemyTypeOne: public Enemy {
    public:
        EnemyTypeOne(const char* fname, float velocity = 200.f,  float angular_velocity = 3 * M_PI);
        EnemyTypeOne(const EnemyTypeOne& enemy);

        void update(const Point& point, float dt);
        void checkBoundaries();

    protected:
        Point m_direction;
};

class EnemyTypeTwo: public Enemy {
    public:
        EnemyTypeTwo(const char* fname, float velocity = 150.f,  float angular_velocity = 0.f);
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
        EnemyTypeThree(const char* fname, float velocity = 200.f,  float angular_velocity = 0.f)
            : Enemy(fname, velocity, angular_velocity)
        {}
};

class EnemySet {
    public:
        EnemySet();

        void update(const Point& player_center, float dt);
        void draw();

    protected:
        EnemyTypeOne original_enemy1;
        EnemyTypeTwo original_enemy2;
        std::unordered_set<Enemy*> enemies;
        float time_elapsed_since_last_enemy;
        float time_between_enemies;
};