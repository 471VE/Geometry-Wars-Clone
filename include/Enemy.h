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
        Enemy(const Enemy& enemy, float rotate_spawn_angle = 0.f);
        virtual ~Enemy() {}

        float getRandom(float a, float b);        
        void checkBoundaries(Point& direction);
        virtual void update(const Point& point, float dt) = 0;
        virtual void updateScoreImage(float dt) = 0;
        virtual void drawScore() = 0;
        virtual void setScoreCenter() = 0;

        void updateAll(const Point& point, float dt);
        void removeLife();
        inline int getLives() { return m_lives; }

        void drawEnemy();
        void updateHighlightStatus(float dt);
    
        inline void highlightOn() {  m_highlighted = true; }
        inline int getScore() { return m_score; }

        void die();

        inline bool isDead() { return m_dead; }
        inline bool isDeadCompletely() { return (m_death_time > m_max_death_time); }
        inline bool isSpawning() { return m_spawning; }

        void spawnObjects(float dt);
        void drawSpawning();

    protected:
        int m_lives;
        bool m_dead = false;
        int m_score;
        float m_max_text_size = ENEMY_SCORE_TEXT_SIZE;

        bool m_highlighted = false;
        float m_highlight_time = 0;
        const float m_max_highlight_time = MAX_HIGHLIGHT_TIME;

        bool m_spawning = true;
        float m_max_spawn_time = SPAWNING_TIME;
        std::vector<GameObject> m_spawn_objects;
};


class EnemyTypeOne: public Enemy {
    public:
        EnemyTypeOne(const char* fname, float velocity = 200.f,  float angular_velocity = 3 * M_PI, float rotational_velocity = M_PI / 8);
        EnemyTypeOne(const EnemyTypeOne& enemy);
        ~EnemyTypeOne() {}

        void updateScoreImage(float dt);
        void drawScore();
        void setScoreCenter();
        void update(const Point& point, float dt);

    protected:
        Point m_direction;
        float m_rotational_velocity;
        Object m_score_image;
};

class EnemyTypeTwo: public Enemy {
    public:
        EnemyTypeTwo(const char* fname, float velocity = 250.f,  float angular_velocity = 0.f);
        EnemyTypeTwo(const EnemyTypeTwo& enemy);
        ~EnemyTypeTwo() {}

        void shapeShift();
        void moveToPoint(const Point& point, float dt);

        void updateScoreImage(float dt);
        void drawScore();
        void setScoreCenter();
        void update(const Point& point, float dt);

    protected:
        float m_lifetime;
        float m_current_scaleX;
        float m_current_scaleY;
        float m_omega;
        Object m_score_image;
};

class EnemyTypeThree: public Enemy {
    public:
        EnemyTypeThree(const char* fname, float min_velocity = 300.f, float max_velocity = 450.f,
            float min_angular_velocity = M_PI, float max_angular_velocity = 4 * M_PI, float angle_threshold = M_PI / 6);
        EnemyTypeThree(const EnemyTypeThree& enemy);
        ~EnemyTypeThree() {}

        void updateScoreImage(float dt);
        void drawScore();
        void setScoreCenter();
        void update_velocity(float&v, const float& min_v, const float& max_v, const float& angle);
        void update(const Point& point, float dt);

    protected:
        float m_min_velocity, m_max_velocity;
        float m_min_angular_velocity, m_max_angular_velocity;
        float m_angle_threshold;
        Point m_last_direction = Point(0.f, 0.f);
        Object m_score_image;
        float last_angle_sign = 1.f;
};

class EnemySet {
    public:
        EnemySet(float time_between_enemies = TIME_BETWEEN_ENEMIES,
            float speedup_coefficient = TIME_BETWEEN_ENEMIES_MULTIPLICATION_COEF);

        void update(const Point& player_center, float dt, BulletSet& bullet_set, Player& player);
        void draw();
        Enemy* chooseEnemy();
        void explodeAll();
        inline int getNumberOfEnemies() { return int(m_enemies.size()); }
        inline void reset() {
            m_time_elapsed_since_last_enemy = 0;
            m_time_between_enemies = m_start_time_between_enemies;
            m_enemies_created = 0;
        }

    protected:
        EnemyTypeOne m_original_enemy1;
        EnemyTypeTwo m_original_enemy2;
        EnemyTypeThree m_original_enemy3;
        std::unordered_set<Enemy*> m_enemies;
        float m_time_elapsed_since_last_enemy;
        float m_time_between_enemies;
        float m_start_time_between_enemies;
        int m_enemies_created;
        float m_speedup_coefficient;
};