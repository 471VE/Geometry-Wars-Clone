#include "GameObject.h"

GameObject::GameObject(const char *fname, float centerX, float centerY, float angular_velocity, float velocity)
    : Object(fname, centerX, centerY, angular_velocity, velocity)
{}
    
GameObject::GameObject(const GameObject& object)
    : Object(object)
    , m_death_time(0.f)
{}

void GameObject::updateFragments(float dt) {
    m_death_time += dt;
    for (auto& fragment: m_fragments) {
        fragment.moveInTheLastDirection(dt);
        fragment.rotateCounterClockWise(dt);
        fragment.make_transparent(m_death_time, m_max_death_time);
    }      
}

void GameObject::drawFragments() {
    for (auto& fragment: m_fragments)
        fragment.draw();
}