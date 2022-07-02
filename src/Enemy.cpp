#include <array>
#include <random>

#include "Enemy.h"

float get_sign(float x) {
    if (x >= 0)
        return 1.f;
    return -1.f;
}

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

void Enemy::checkBoundaries(Point& direction) {
    if (m_centerX < float(m_width) / 2.f) {
        direction.x *= -1;
        m_centerX = float(m_width) / 2.f;
    }
    else if (m_centerX > float(SCREEN_WIDTH - m_width / 2)) {
        direction.x *= -1;
        m_centerX = float(SCREEN_WIDTH - m_width / 2);
    }

    if (m_centerY < float(m_height) / 2.f) {
        direction.y *= -1;
        m_centerY = float(m_height) / 2.f;
    } 
    else if (m_centerY > float(SCREEN_HEIGHT - m_height / 2)) {
        direction.y *= -1;
        m_centerY = float(SCREEN_HEIGHT - m_height / 2);
    }
}


EnemyTypeOne::EnemyTypeOne(const char* fname, float velocity,  float angular_velocity, float rotational_velocity)
    : Enemy(fname, velocity, angular_velocity)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
    , m_rotational_velocity(rotational_velocity)
{
    m_direction.normalize();
}

EnemyTypeOne::EnemyTypeOne(const EnemyTypeOne& enemy)
    : Enemy(enemy)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
    , m_rotational_velocity(enemy.m_rotational_velocity)
{
    m_direction.normalize();
}

void EnemyTypeOne::update(const Point& point, float dt) {
    rotateCounterClockWise(dt);
    float angle = m_direction.getAngle();
    float new_angle = angle + m_rotational_velocity * dt;
    m_direction = Point(new_angle);
    move(m_direction, dt);
    checkBoundaries(m_direction);
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
    m_current_scaleX = 1.f + 0.2f * std::sin(m_omega * m_lifetime);
    m_current_scaleY = 1.f - 0.2f * std::sin(m_omega * m_lifetime);
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

EnemyTypeThree::EnemyTypeThree(const char* fname, float min_velocity, float max_velocity,
    float min_angular_velocity, float max_angular_velocity, float angle_threshold)
    : Enemy(fname, min_velocity, min_angular_velocity)
    , m_min_velocity(min_velocity)
    , m_max_velocity(max_velocity)
    , m_min_angular_velocity(min_angular_velocity)
    , m_max_angular_velocity(max_angular_velocity)
    , m_angle_threshold(angle_threshold)
{}

EnemyTypeThree::EnemyTypeThree(const EnemyTypeThree& enemy)
    : Enemy(enemy)
    , m_min_velocity(enemy.m_min_velocity)
    , m_max_velocity(enemy.m_max_velocity)
    , m_min_angular_velocity(enemy.m_min_angular_velocity)
    , m_max_angular_velocity(enemy.m_max_angular_velocity)
    , m_angle_threshold(enemy.m_angle_threshold)
{}

void EnemyTypeThree::update_velocity(float&v, const float& min_v, const float& max_v, const float& angle) {
    if (angle < 2 * m_angle_threshold) {
        float scaled_angle = angle / (2 * m_angle_threshold);
        v = (max_v - min_v) * std::sqrt(1 - scaled_angle * scaled_angle) + min_v;
    } else
        v = min_v;
}

void EnemyTypeThree::update(const Point& point, float dt) {
    float arrow_direction_angle = std::atan2(get_cursor_y() - point.y, get_cursor_x() - point.x);
    float enemy_direction_angle = std::atan2(m_centerY - point.y, m_centerX - point.x);
    Point enemy_direction(m_centerX - point.x, m_centerY - point.y);

    float angle = enemy_direction_angle - arrow_direction_angle;
    check_angle(angle);

    float angle_sign = get_sign(angle);
    angle = std::abs(angle);

    update_velocity(m_angular_velocity, m_min_angular_velocity, m_max_angular_velocity, angle);
    update_velocity(m_velocity, m_min_velocity, m_max_velocity, angle);

    Point direction;
    if (angle < m_angle_threshold) {
        direction.x = - enemy_direction.y * angle_sign;
        direction.y = enemy_direction.x * angle_sign;
    } else {
        direction = Point(point.x - m_centerX, point.y - m_centerY);
    }

    direction.normalize();
    if (m_last_direction.x == 0.f && m_last_direction.y == 0.f) {
        m_last_direction = direction;
    }
    direction.x = direction.x * 5 * dt + m_last_direction.x * (1 - 5 * dt);
    direction.y = direction.y * 5 * dt + m_last_direction.y * (1 - 5 * dt);

    direction.normalize();
    checkBoundaries(direction);
    move(direction, dt);  

    m_last_direction = direction;          
    rotateCounterClockWise(dt * angle_sign);
}

EnemySet::EnemySet(float time_between_enemies, float speedup_coefficient)
    : original_enemy1(EnemyTypeOne("assets/sprites/enemy1.bmp"))
    , original_enemy2(EnemyTypeTwo("assets/sprites/enemy2.bmp"))
    , original_enemy3(EnemyTypeThree("assets/sprites/enemy3.bmp"))
    , time_elapsed_since_last_enemy(0)
    , time_between_enemies(time_between_enemies)
    , speedup_coefficient(speedup_coefficient)
    , enemies_created(0)
{
    mciSendString("OPEN assets/SFX/explosion.wav ALIAS explosion",0,0,0);
}

void EnemySet::update(const Point& player_center, float dt, BulletSet& bullet_set) {
    time_elapsed_since_last_enemy += dt;
    if (time_elapsed_since_last_enemy > time_between_enemies) {
        time_elapsed_since_last_enemy = 0;
        time_between_enemies *= 0.99f;

        Enemy* enemy = chooseEnemy();
        enemies.insert(enemy);
    }

    for (auto enemy = enemies.begin(); enemy != enemies.end();) {
        bool enemy_killed = false;
        for (auto bullet = bullet_set.bullets.begin(); bullet != bullet_set.bullets.end();) {
            if ((*bullet)->hits(*(*enemy))) {
                delete (*bullet);
                bullet_set.bullets.erase(bullet++);

                delete (*enemy);
                enemies.erase(enemy++);
                enemy_killed = true;
                mciSendString("PLAY explosion from 0",0,0,0);
                break;
            }
            else
                ++bullet;
        }
        if (!enemy_killed)
            ++enemy;
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

Enemy* EnemySet::chooseEnemy() {
    int enemy_type;
    if (enemies_created < 3) {
        enemy_type = enemies_created + 1;
    } else {
        std::uniform_int_distribution<int> uniform(1, 3);
        enemy_type = uniform(generator);
    }
    enemies_created++;
    switch(enemy_type) {
        case 1:
            return new EnemyTypeOne(original_enemy1);
        case 2:
            return new EnemyTypeTwo(original_enemy2);
        case 3:
            return new EnemyTypeThree(original_enemy3);
        default:
            log_error_and_exit("Something went wrong when creating new enemy.");
            return 0;
    }
}