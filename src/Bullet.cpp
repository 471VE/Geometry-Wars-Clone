#include "Bullet.h"
#include <windows.h>

Bullet::Bullet(const Bullet& bullet, Point center)
    : GameObject(bullet)
    , m_direction(Point(0.f, 1.f))
{
    m_centerX = center.x;
    m_centerY = center.y;
    m_direction = Point(float(get_cursor_x()) - m_centerX, float(get_cursor_y()) - m_centerY);
    m_direction.normalize();
    rotate(m_direction.getAngle());
}

void Bullet::move(float dt) {
    m_centerY += m_direction.y * m_velocity * dt;
    m_centerX += m_direction.x * m_velocity * dt;
}

void Bullet::initialMove() {
    m_centerY += m_direction.y * 25;
    m_centerX += m_direction.x * 25;
}

bool Bullet::outsideScreen() {
    return (
        m_centerX < m_hitbox_radius / 2.f ||
        m_centerX > SCREEN_WIDTH - m_hitbox_radius / 2.f ||
        m_centerY < m_hitbox_radius / 2.f / 2.f ||
        m_centerY > SCREEN_HEIGHT - m_hitbox_radius / 2.f
    );
}

void Bullet::explodeBullet() { 
    explode();
    m_dead = true;
}

void Bullet::drawBullet() {
    if (m_dead) 
        drawFragments();
    else
        draw();
}

BulletSet::BulletSet()
    : m_original_bullet(Bullet("assets/sprites/bullets.bmp"))
    , m_time_elapsed_since_last_bullet(0)
{
    mciSendString("OPEN assets/SFX/shot.mp3 ALIAS shot",0,0,0);
}

void BulletSet::update(const Point& player_center, float dt) {
    m_time_elapsed_since_last_bullet += dt;
    if (!game_over && (is_key_pressed(VK_SPACE) || is_mouse_button_pressed(0))) {
        if (m_time_elapsed_since_last_bullet > TIME_BETWEEN_BULLETS) {
            m_time_elapsed_since_last_bullet = 0;
            Bullet* bullet =  new Bullet(m_original_bullet, player_center);
            bullet->initialMove();
            m_bullets.insert(bullet);
            mciSendString("PLAY shot from 0",0,0,0);
        }
    }

    for (auto bullet = m_bullets.begin(); bullet != m_bullets.end();) {
        (*bullet)->update(dt);
        if ((*bullet)->outsideScreen()) {
            if (!(*bullet)->isDead())
                (*bullet)->explodeBullet();
        }
        if ((*bullet)->isDeadCompletely()) {
            delete (*bullet);
            m_bullets.erase(bullet++);
        } else
            ++bullet;
    }           
}

void BulletSet::draw() {
    for (auto bullet = m_bullets.begin(); bullet != m_bullets.end(); ++bullet)
        (*bullet)->drawBullet();
}