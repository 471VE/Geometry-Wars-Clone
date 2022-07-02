#include <array>
#include <random>

#include "Enemy.h"

std::mt19937 generator;

float Enemy::getRandom(float a, float b) {
    std::uniform_real_distribution<float> uniform(a, b);
    return uniform(generator);
}

Enemy::Enemy(const char* fname, float velocity,  float angular_velocity)
    : GameObject(fname, 0, 0, angular_velocity, velocity)
{   
    m_centerX = getRandom(float(m_width), float(SCREEN_WIDTH - m_width));
    m_centerY = getRandom(float(m_height), float(SCREEN_HEIGHT - m_height));
}

Enemy::Enemy(const Enemy& enemy)
    : GameObject(enemy)
{
    m_centerX = getRandom(float(m_width), float(SCREEN_WIDTH - m_width));
    m_centerY = getRandom(float(m_height), float(SCREEN_HEIGHT - m_height));
}

void Enemy::resize(float scaleX, float scaleY) {
    m_height_render = int(float(m_height) * scaleY);
    m_width_render = int(float(m_width) * scaleX);

    m_data_render.clear();
    m_data_render.resize(m_width_render * m_height_render * m_channels);

    float x, x_floor, x_ceil;
    float y, y_floor, y_ceil;

    float q1, q2;
    uint8_t old1, old2, old3, old4;

	for (int i = 0; i < m_height_render; ++i) {
        for (int j = 0; j < m_width_render; ++j) {
			x = float(i) / scaleY;
			y = float(j) / scaleX;

			x_floor = std::floor(x);
			x_ceil = std::min(float(m_height - 1), std::ceil(x));
			y_floor = std::floor(y);
			y_ceil = std::min(float(m_width - 1), std::ceil(y));

			if ((x_ceil == x_floor) && (y_ceil == y_floor))
                for (int channel = 0; channel < 4; ++channel)
                    at_render(i, j, channel) = at(int(x), int(y), channel);         

            else if (x_ceil == x_floor)
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x), int(y_floor), channel);
                    old2 = at(int(x), int(y_ceil), channel);
                    at_render(i, j, channel) = uint8_t(float(old1) * (y_ceil - y) + float(old2) * (y - y_floor));
                }

            else if  (y_ceil == y_floor)
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x_floor), int(y), channel);
                    old2 = at(int(x_ceil), int(y), channel);
                    at_render(i, j, channel) = uint8_t(float(old1) * (x_ceil - x) + float(old2) * (x - x_floor));
                }

            else
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x_floor), int(y_floor), channel);
                    old2 = at(int(x_ceil), int(y_floor), channel);
                    old3 = at(int(x_floor), int(y_ceil), channel);
                    old4 = at(int(x_ceil), int(y_ceil), channel);

                    q1 = float(old1) * (x_ceil - x) + float(old2) * (x - x_floor);
                    q2 = float(old3) * (x_ceil - x) + float(old4) * (x - x_floor);
                    at_render(i, j, channel) = uint8_t(q1 * (y_ceil - y) + q2 * (y - y_floor));
                }
        }
    }
}


EnemyTypeOne::EnemyTypeOne(const char* fname, float velocity,  float angular_velocity)
    : Enemy(fname, velocity, angular_velocity)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
{
    m_direction.normalize();
}

EnemyTypeOne::EnemyTypeOne(const EnemyTypeOne& enemy)
    : Enemy(enemy)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
{
    m_direction.normalize();
}

void EnemyTypeOne::update(const Point& point, float dt) {
    checkBoundaries();
    rotateCounterClockWise(dt);
    move(m_direction, dt);
}

void EnemyTypeOne::checkBoundaries() {
    if (m_centerX < float(m_width) / 2.f) {
        m_direction.x *= -1;
        m_centerX = float(m_width) / 2.f;
    }
    else if (m_centerX > float(SCREEN_WIDTH - m_width / 2)) {
        m_direction.x *= -1;
        m_centerX = float(SCREEN_WIDTH - m_width / 2);
    }

    if (m_centerY < float(m_height) / 2.f) {
        m_direction.y *= -1;
        m_centerY = float(m_height) / 2.f;
    } 
    else if (m_centerY > float(SCREEN_HEIGHT - m_height / 2)) {
        m_direction.y *= -1;
        m_centerY = float(SCREEN_HEIGHT - m_height / 2);
    }

}

EnemyTypeTwo::EnemyTypeTwo(const char* fname, float velocity, float angular_velocity)
    : Enemy(fname, velocity, angular_velocity)
    , m_lifetime(0)
    , m_current_scaleX(1.f)
    , m_current_scaleY(1.f)
    , m_omega(2 * M_PI)
{}

EnemyTypeTwo::EnemyTypeTwo(const EnemyTypeTwo& enemy)
    : Enemy(enemy)
    , m_lifetime(0)
    , m_current_scaleX(enemy.m_current_scaleX)
    , m_current_scaleY(enemy.m_current_scaleY)
    , m_omega(enemy.m_omega)
{}


void EnemyTypeTwo::shapeShift() {
    m_current_scaleX = 1.f + 0.1f * std::sin(m_omega * m_lifetime);
    m_current_scaleY = 1.f - 0.1f * std::sin(m_omega * m_lifetime);
    resize(m_current_scaleX, m_current_scaleY);
}

void EnemyTypeTwo::moveToPoint(const Point& point, float dt) {
    Point direction(point.x - m_centerX, point.y - m_centerY);
    direction.normalize();
    move(direction, dt);
}

void EnemyTypeTwo::update(const Point& point, float dt) {
    moveToPoint(point, dt);
    m_lifetime += dt;
    shapeShift();
}


EnemySet::EnemySet()
    : original_enemy1(EnemyTypeOne("assets/sprites/enemy1.bmp"))
    , original_enemy2(EnemyTypeTwo("assets/sprites/enemy2.bmp"))
    , time_elapsed_since_last_enemy(0)
    , time_between_enemies(2.f)
{}

void EnemySet::update(const Point& player_center, float dt) {
    time_elapsed_since_last_enemy += dt;
    if (time_elapsed_since_last_enemy > time_between_enemies) {
        time_elapsed_since_last_enemy = 0;
        time_between_enemies *= 0.9f;

        Enemy* enemy =  new EnemyTypeTwo(original_enemy2);
        enemies.insert(enemy);
    }

    for (auto enemy = enemies.begin(); enemy != enemies.end();) {
        (*enemy)->update(player_center, dt);
        ++enemy;
    }  
}

void EnemySet::draw() {
    for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
        (*enemy)->draw();
}