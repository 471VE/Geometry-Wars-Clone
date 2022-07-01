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
    rotate(m_direction.getAngle() + M_PI_2);
}

void Bullet::move(float dt) {
    m_centerY += m_direction.y * m_velocity * dt;
    m_centerX += m_direction.x * m_velocity * dt;
}

BulletSet::BulletSet()
    : original_bullet(Bullet("assets/sprites/bullets.bmp"))
    , time_elapsed_since_last_bullet(0)
{
    mciSendString("OPEN assets/SFX/shot.mp3 ALIAS shot",0,0,0);
}

void BulletSet::update(float dt, Point player_center) {
    time_elapsed_since_last_bullet += dt;
    if (is_key_pressed(VK_SPACE) || is_mouse_button_pressed(0)) {
        if (time_elapsed_since_last_bullet > 0.2) {
            time_elapsed_since_last_bullet = 0;
            Bullet* bullet =  new Bullet(original_bullet, player_center);
            bullets.insert(bullet);
            mciSendString("PLAY shot from 0",0,0,0);
        }
    }

    for (auto bullet = bullets.begin(); bullet != bullets.end();) {
        (*bullet)->move(dt);
        if ((*bullet)->getCenterX() < -float((*bullet)->getWidth()) / 2.f ||
            (*bullet)->getCenterX() > SCREEN_WIDTH + float((*bullet)->getWidth()) / 2.f ||
            (*bullet)->getCenterY() < -float((*bullet)->getHeight()) / 2.f ||
            (*bullet)->getCenterY() > SCREEN_HEIGHT + float((*bullet)->getHeight()) / 2.f) {

            delete (*bullet);
            bullets.erase(bullet++);
        }
        else
            ++bullet;
    }
        
}

void BulletSet::draw() {
    for (auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
        (*bullet)->draw();
}