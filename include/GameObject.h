#pragma once

#include "Object.h"

class GameObject: public Object {
    public:
        GameObject(const char *fname, float centerX = float(SCREEN_WIDTH) / 2.f, float centerY = float(SCREEN_HEIGHT) / 2.f,
            float angular_velocity = 2 * M_PI, float velocity = 200.f);
        GameObject(const GameObject& object);

        void explode() { m_fragments = createFragments(); }
        void updateFragments(float dt);
        void drawFragments();

    protected:
        std::vector<Object> m_fragments;
        float m_death_time = 0.f;
        float m_max_death_time = 1.5f;
};