#include "GameObject.h"

GameObject::GameObject(const char *fname, float centerX, float centerY, float angular_velocity, float velocity)
    : Object(fname, centerX, centerY, angular_velocity, velocity)
    , m_spawn_time(0.f)
{}
    
GameObject::GameObject(const GameObject& object)
    : Object(object)
    , m_death_time(0.f)
    , m_spawn_time(0.f)
{}

GameObject::GameObject(const GameObject& object, float centerX, float centerY)
    : Object(object)
    , m_death_time(0.f)
    , m_spawn_time(0.f)
{
    m_centerX = centerX;
    m_centerY = centerY;
}

GameObject::GameObject(const GameObject& object, float centerX, float centerY, float angle)
    : Object(object)
    , m_death_time(0.f)
    , m_spawn_time(0.f)
{
    m_centerX = centerX;
    m_centerY = centerY;
    rotate(angle);
    m_data = m_data_render;
    m_width = m_width_render;
    m_height = m_height_render;
    m_angle = 0;
}

void GameObject::updateFragments(float dt) {
    m_death_time += dt;
    for (auto& fragment: m_fragments) {
        fragment.moveInTheLastDirection(dt);
        fragment.rotateCounterClockWise(dt);
        fragment.make_transparent(std::max(1.f - m_death_time/m_max_death_time, 0.f));
    }      
}

void GameObject::drawFragments() {
    for (auto& fragment: m_fragments)
        fragment.draw();
}

void GameObject::spawningAnimation(float dt) {
    m_spawn_time += dt;
    if (m_spawn_time > m_single_spawn_time)
        m_spawn_time -=m_single_spawn_time;
    float scale = 1.5f * m_spawn_time / (m_single_spawn_time);
    float transparency = std::sin(M_PI / (m_single_spawn_time) * m_spawn_time);
    resize(scale, scale);
    make_transparent(transparency);
}