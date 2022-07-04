#pragma once

#include "GameObject.h"

class PlayerSprite: public GameObject {
    public:
        PlayerSprite(const char *fname, float angular_velocity = 2 * M_PI, float velocity = 200.f);
  
        void getToAngle(float angle, float dt, float difference, float angular_velocity);
        void getToAngleUniform(float angle, float dt);
        void getToAngleNonUniform(float angle, float dt, float time = 0.3f);
        
        Point getMovementDirection();
        void moveWithInertiaAndRotation(float dt);

        void updateAll(float dt);
        void drawPlayerSprite();
        void die();

        inline bool isDead() { return m_dead; }
        inline bool isDeadCompletely() { return (m_death_time > m_max_death_time); }
        inline float getDeathTime() { return m_death_time; }
        inline bool isSpawning() { return m_spawning; }

        void spawnObjects(float dt);
        void drawSpawning();
    
    protected:
        bool m_dead = false;

        bool m_spawning = true;
        float m_max_spawn_time = SPAWNING_TIME;
        std::vector<GameObject> m_spawn_objects;
};

class PlayerArrow: public GameObject {
    public:
        PlayerArrow(const char *fname)
            : GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f)
            , m_direction(Point(float(get_cursor_x()) - m_centerX, float(get_cursor_x()) - m_centerY))
        {}

        Point getDirection() { return m_direction; }

        void rotateToMouseDirection(bool paused);
        void setCenter(float x, float y);

        void incrementSpawnTime(float dt) { m_spawn_time += dt; }
        void drawArrow(bool paused) {
            rotateToMouseDirection(paused);
            if (m_spawn_time > SPAWNING_TIME - ARROW_FADE_TIME){
                if (m_spawn_time < SPAWNING_TIME)
                    make_transparent((m_spawn_time - SPAWNING_TIME + ARROW_FADE_TIME) / ARROW_FADE_TIME);
                draw();
            }
        }
    
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

        bool hits(Object& object) { return (player_sprite.hits(object)); }
        void update(float dt);
        void die();

        inline bool isDead() { return player_sprite.isDead(); }
        inline float getDeathTime() { return player_sprite.getDeathTime(); }
        inline bool isDeadCompletely() { return player_sprite.isDeadCompletely(); }

        void draw(bool paused) {
            player_sprite.drawPlayerSprite();
            if (!player_sprite.isDead())
                player_arrow.drawArrow(paused);
        }

    protected:
        PlayerSprite player_sprite;
        PlayerArrow player_arrow;
};