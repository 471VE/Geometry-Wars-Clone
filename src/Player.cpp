#include "Player.h"

PlayerSprite::PlayerSprite(const char *fname, float angular_velocity, float velocity)
    : GameObject(fname, float(SCREEN_WIDTH) / 2.f, float(SCREEN_HEIGHT) / 2.f, angular_velocity, velocity)
{
    for (int i = 0; i < 2; ++i) {
        m_spawn_objects.push_back(GameObject(fname, m_centerX, m_centerY, angular_velocity, velocity));
    }
    m_spawn_objects[1].offsetSpawnTime();
}

void PlayerSprite::updateAll(float dt) {
    if (m_spawning && !m_dead) {
        m_spawn_time += dt;
        if (m_spawn_time > m_max_spawn_time) {
            m_spawning = false;
            m_spawn_objects.clear();
        }
        else {
            spawnObjects(dt);
        }
    } else if (!m_dead)
        moveWithInertiaAndRotation(dt);
    else
        updateFragments(dt);
}

void PlayerSprite::drawPlayerSprite() {
    if (m_dead) 
        drawFragments();
    else if (m_spawning)
        drawSpawning();
    else
        draw();
}

void PlayerSprite::die() {
    m_dead = true;
    explode();
}

void PlayerSprite::spawnObjects(float dt) {
    for (auto& object: m_spawn_objects)
        object.spawningAnimation(dt);
}

void PlayerSprite::drawSpawning() {
    for (auto& object: m_spawn_objects)
        if (object.getSpawnTime() > 0)
            object.draw();
}

Point PlayerSprite::getMovementDirection() {
    Point direction(0.f, 0.f);

	if (is_key_pressed(VK_W))
		direction.y -= 1.f;

	if (is_key_pressed(VK_S))
		direction.y += 1.f;

	if (is_key_pressed(VK_D))
		direction.x += 1.f;

	if (is_key_pressed(VK_A)) 
		direction.x -= 1.f;

    direction.normalize();
    return direction;
}

void PlayerSprite::getToAngle(float angle, float dt, float difference, float angular_velocity) {
    if (difference > m_angle_threshold) {
        m_angle += angular_velocity * dt;
        rotate(m_angle);
    } else if (difference < -m_angle_threshold) {
        m_angle -= angular_velocity * dt;
        rotate(m_angle);
    } else {
        m_angle = angle;
        rotate(m_angle);
    }
}

void PlayerSprite::getToAngleUniform(float angle, float dt) {
    float difference = getAngleDifference(angle);
    getToAngle(angle, dt, difference, m_angular_velocity);
}

void PlayerSprite::getToAngleNonUniform(float angle, float dt, float time) {
    float difference = getAngleDifference(angle);
    float angular_velocity =  4 * std::abs(difference / time);
    getToAngle(angle, dt, difference, angular_velocity);
}

void PlayerSprite::moveWithInertiaAndRotation(float dt) {
    Point expected_direction = getMovementDirection();
    float angle;
    if ((expected_direction.x == 0.f) && (expected_direction.y == 0.f))
        angle = getAngle();
    else 
        angle = expected_direction.getAngle();
    getToAngleNonUniform(angle, dt);
    Point direction(m_angle);
    float velocity;
    if ((expected_direction.x == 0.f) && (expected_direction.y == 0.f)) {
        m_last_velocity /= (dt * (1 / dt + 3));
        velocity = m_last_velocity;
    } else {
        m_last_velocity_direction = direction;
        m_last_velocity = m_velocity;
        velocity = m_velocity;
    }

    float new_centerY = m_centerY + m_last_velocity_direction.y * velocity * dt;
    float new_centerX = m_centerX + m_last_velocity_direction.x * velocity * dt;

    if (new_centerX < 16.f)
        m_centerX = 16.f;
    else if (new_centerX > float(SCREEN_WIDTH - 16))
        m_centerX = float(SCREEN_WIDTH - 16);
    else
        m_centerX = new_centerX;

    if (new_centerY < 16.f)
        m_centerY = 16.f;
    else if (new_centerY > float(SCREEN_HEIGHT - 16))
        m_centerY = float(SCREEN_HEIGHT - 16);
    else
        m_centerY = new_centerY;
}

void PlayerArrow::rotateToMouseDirection(bool paused) {
    if (!paused)
        m_direction = Point(float(get_cursor_x()) - m_centerX, float(get_cursor_y()) - m_centerY);

    rotate(m_direction.getAngle());
}

void PlayerArrow::setCenter(float x, float y) {
    m_centerX = x;
    m_centerY = y;
}

void Player::update(float dt) {
    player_sprite.updateAll(dt);
    player_arrow.setCenter(player_sprite.getCenterX(), player_sprite.getCenterY());
    player_arrow.incrementSpawnTime(dt);
}

#include <windows.h>

void Player::die() {
    player_sprite.die();
    mciSendString("PLAY lose from 0",0,0,0);
}
