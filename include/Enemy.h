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
        Enemy(const char* fname, float velocity = 200.f,  float angular_velocity = 0.f, int lives = 1, int score = 50);
        Enemy(const Enemy& enemy);

        float getRandom(float a, float b);        
        void checkBoundaries(Point& direction);
        virtual void update(const Point& point, float dt) = 0;

        void updateAll(const Point& point, float dt);
        void removeLife();
        inline int getLives() { return m_lives; }

        void drawEnemy();
        void updateHighlightStatus(float dt);
    
        inline void highlightOn() {  m_highlighted = true; }

        void updateFragments(float dt);
        void drawFragments();

        void die();
        void explode() { m_fragments = createFragments(); }

        inline bool isDead() { return m_dead; }
        inline bool isDeadCompletely() { return (m_death_time > m_max_death_time); }

    protected:
        int m_lives;
        bool m_highlighted = false;
        bool m_dead = false;
        float m_highlight_time = 0;
        const float m_max_highlight_time = 0.1f;
        std::vector<GameObject> m_fragments;
        float m_death_time = 0.f;
        float m_max_death_time = 2.f;
        int m_score;
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
        EnemyTypeOne m_original_enemy1;
        EnemyTypeTwo m_original_enemy2;
        EnemyTypeThree m_original_enemy3;
        std::unordered_set<Enemy*> m_enemies;
        float m_time_elapsed_since_last_enemy;
        float m_time_between_enemies;
        int m_enemies_created;
        float m_speedup_coefficient;
};