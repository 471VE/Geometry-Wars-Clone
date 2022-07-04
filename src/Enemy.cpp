#include <array>
#include <random>

#include "Enemy.h"
#include "Highscore.h"
#include "Soundtrack.h"

void Enemy::updateAll(const Point& point, float dt) {
    if (m_spawning && !m_dead) {
        m_spawn_time += dt;
        if (m_spawn_time > m_max_spawn_time) {
            m_spawning = false;
            m_spawn_objects.clear();
        }
        else {
            spawnObjects(dt);
        }
        updateHighlightStatus(dt);
    } else if (!m_dead) {
        update(point, dt);
        updateHighlightStatus(dt);
    } else {
        updateScoreImage(dt);
        updateFragments(dt);
    }  
}

void Enemy::drawEnemy() {
    if (m_dead) {
        if (!game_over)
            drawScore();
        drawFragments();
    }
    else if (m_spawning) {
        drawSpawning();
    }
    else if (m_highlighted)
        draw(50.f);
    else
        draw();
}

void Enemy::die() {
    m_dead = true;
    explode();
    setScoreCenter();
}

void Enemy::updateHighlightStatus(float dt) {
    if (m_highlight_time > m_max_highlight_time) {
        m_highlighted = false;
        m_highlight_time = 0;
    }
    if (m_highlighted)
        m_highlight_time += dt;
}

float get_sign(float x) {
    if (x >= 0)
        return 1.f;
    return -1.f;
}

std::random_device dev;
std::mt19937 generator( dev() );

float Enemy::getRandom(float a, float b) {
    std::uniform_real_distribution<float> uniform(a, b);
    return uniform(generator);
}

Enemy::Enemy(const char* fname, float velocity,  float angular_velocity, int lives, int score)
    : GameObject(fname, 0, 0, angular_velocity, velocity)
    , m_lives(lives)
    , m_score(score)
    , m_spawning(true)
{   
    m_centerX = getRandom(float(m_width), float(SCREEN_WIDTH - m_width));
    m_centerY = getRandom(float(m_height), float(SCREEN_HEIGHT - m_height));

    for (int i = 0; i < 2; ++i) {
        m_spawn_objects.push_back(GameObject(fname, m_centerX, m_centerY, angular_velocity, velocity));
    }
    m_spawn_objects[1].offsetSpawnTime();
}

Enemy::Enemy(const Enemy& enemy, float rotate_spawn_angle)
    : GameObject(enemy)
    , m_lives(enemy.m_lives)
    , m_score(enemy.m_score)
    , m_spawning(true)
{
    m_centerX = getRandom(float(m_width), float(SCREEN_WIDTH - m_width));
    m_centerY = getRandom(float(m_height), float(SCREEN_HEIGHT - m_height));
    m_angle = rotate_spawn_angle;

    for (int i = 0; i < 2; ++i) {
        if (rotate_spawn_angle != 0.f)
            m_spawn_objects.push_back(GameObject(enemy, m_centerX, m_centerY, rotate_spawn_angle));
        else
            m_spawn_objects.push_back(GameObject(enemy, m_centerX, m_centerY));
    }
    m_spawn_objects[1].offsetSpawnTime();
}

void Enemy::spawnObjects(float dt) {
    for (auto& object: m_spawn_objects) {
        object.spawningAnimation(dt);
    }
}

void Enemy::drawSpawning() {
    for (auto& object: m_spawn_objects) {
        if (object.getSpawnTime() > 0) {
            if (m_highlighted)
                object.draw(20.f);
            else 
                object.draw();
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

void Enemy::removeLife() {
    m_lives--;
}


EnemyTypeOne::EnemyTypeOne(const char* fname, float velocity,  float angular_velocity, float rotational_velocity)
    : Enemy(fname, velocity, angular_velocity, 2, 50)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
    , m_rotational_velocity(rotational_velocity)
    , m_score_image(Object("assets/sprites/score50.bmp"))
{
    m_direction.normalize();
}

EnemyTypeOne::EnemyTypeOne(const EnemyTypeOne& enemy)
    : Enemy(enemy, M_PI / 4)
    , m_direction(Point(getRandom(-1.f, 1.f), getRandom(-1.f, 1.f)))
    , m_rotational_velocity(enemy.m_rotational_velocity)
    , m_score_image(enemy.m_score_image)
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

void EnemyTypeOne::updateScoreImage(float dt) {
    float scale = (std::max)(1.f - (m_death_time / 0.5f) * (m_death_time / 0.5f), 0.f);
    m_score_image.resize(scale, scale);
}

void EnemyTypeOne::drawScore() {
    m_score_image.draw();
}

void EnemyTypeOne::setScoreCenter() {
    m_score_image.setCenter(Point(m_centerX, m_centerY));
}

EnemyTypeTwo::EnemyTypeTwo(const char* fname, float velocity, float angular_velocity)
    : Enemy(fname, velocity, angular_velocity, 2, 100)
    , m_lifetime(0)
    , m_current_scaleX(1.f)
    , m_current_scaleY(1.f)
    , m_omega(2 * M_PI)
    , m_score_image(Object("assets/sprites/score100.bmp"))
{}

EnemyTypeTwo::EnemyTypeTwo(const EnemyTypeTwo& enemy)
    : Enemy(enemy)
    , m_lifetime(0)
    , m_current_scaleX(enemy.m_current_scaleX)
    , m_current_scaleY(enemy.m_current_scaleY)
    , m_omega(enemy.m_omega)
    , m_score_image(enemy.m_score_image)
{}


void EnemyTypeTwo::shapeShift() {
    m_current_scaleX = 1.f + 0.2f * std::sin(m_omega * m_lifetime);
    m_current_scaleY = 1.f - 0.2f * std::sin(m_omega * m_lifetime);
    resize(m_current_scaleX, m_current_scaleY);
}

void EnemyTypeTwo::moveToPoint(const Point& point, float dt) {
    Point direction(point.x - m_centerX, point.y - m_centerY);
    direction.normalize();
    m_last_velocity_direction = direction;
    move(direction, dt);
}

void EnemyTypeTwo::update(const Point& point, float dt) {
    if (!game_over) {
        moveToPoint(point, dt);
    } else {
        rotateCounterClockWise(dt);
        float angle = m_last_velocity_direction.getAngle();
        float new_angle = angle + M_PI / 8 * dt;
        m_last_velocity_direction = Point(new_angle);
        move(m_last_velocity_direction, dt);
        checkBoundaries(m_last_velocity_direction);
    }
    m_lifetime += dt;
    shapeShift();
}

void EnemyTypeTwo::updateScoreImage(float dt) {
    float scale = (std::max)(1.f - (m_death_time / 0.5f) * (m_death_time / 0.5f), 0.f);
    m_score_image.resize(scale, scale);
}

void EnemyTypeTwo::drawScore() {
    m_score_image.draw();
}

void EnemyTypeTwo::setScoreCenter() {
    m_score_image.setCenter(Point(m_centerX, m_centerY));
}

EnemyTypeThree::EnemyTypeThree(const char* fname, float min_velocity, float max_velocity,
    float min_angular_velocity, float max_angular_velocity, float angle_threshold)
    : Enemy(fname, min_velocity, min_angular_velocity, 1, 200)
    , m_min_velocity(min_velocity)
    , m_max_velocity(max_velocity)
    , m_min_angular_velocity(min_angular_velocity)
    , m_max_angular_velocity(max_angular_velocity)
    , m_angle_threshold(angle_threshold)
    , m_score_image(Object("assets/sprites/score200.bmp"))
{}

EnemyTypeThree::EnemyTypeThree(const EnemyTypeThree& enemy)
    : Enemy(enemy, M_PI / 4)
    , m_min_velocity(enemy.m_min_velocity)
    , m_max_velocity(enemy.m_max_velocity)
    , m_min_angular_velocity(enemy.m_min_angular_velocity)
    , m_max_angular_velocity(enemy.m_max_angular_velocity)
    , m_angle_threshold(enemy.m_angle_threshold)
    , m_score_image(enemy.m_score_image)
{}

void EnemyTypeThree::update_velocity(float&v, const float& min_v, const float& max_v, const float& angle) {
    if (angle < 2 * m_angle_threshold) {
        float scaled_angle = angle / (2 * m_angle_threshold);
        v = (max_v - min_v) * std::sqrt(1 - scaled_angle * scaled_angle) + min_v;
    } else
        v = min_v;
}

void EnemyTypeThree::update(const Point& point, float dt) {
    if (!game_over) {
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
        last_angle_sign = angle_sign;
    } else {
        rotateCounterClockWise(dt * last_angle_sign);
        float angle = m_last_direction.getAngle();
        float new_angle = angle + M_PI / 8 * dt;
        m_last_direction = Point(new_angle);
        move(m_last_direction, dt);
        checkBoundaries(m_last_direction);
    }
}

void EnemyTypeThree::updateScoreImage(float dt) {
    float scale = (std::max)(1.f - (m_death_time / 0.5f) * (m_death_time / 0.5f), 0.f);
    m_score_image.resize(scale, scale);
}

void EnemyTypeThree::drawScore() {
    m_score_image.draw();
}

void EnemyTypeThree::setScoreCenter() {
    m_score_image.setCenter(Point(m_centerX, m_centerY));
}

EnemySet::EnemySet(float time_between_enemies, float speedup_coefficient)
    : m_original_enemy1(EnemyTypeOne("assets/sprites/enemy1.bmp"))
    , m_original_enemy2(EnemyTypeTwo("assets/sprites/enemy2.bmp"))
    , m_original_enemy3(EnemyTypeThree("assets/sprites/enemy3.bmp"))
    , m_time_elapsed_since_last_enemy(0)
    , m_time_between_enemies(time_between_enemies)
    , m_start_time_between_enemies(time_between_enemies)
    , m_speedup_coefficient(speedup_coefficient)
    , m_enemies_created(0)
{}

void EnemySet::update(const Point& player_center, float dt, BulletSet& bullet_set, Player& player) {
    m_time_elapsed_since_last_enemy += dt;
    if (!game_over && m_time_elapsed_since_last_enemy > m_time_between_enemies) {
        m_time_elapsed_since_last_enemy = 0;
        m_time_between_enemies *= 0.99f;

        Enemy* enemy = chooseEnemy();
        m_enemies.insert(enemy);
    }

    for (auto enemy = m_enemies.begin(); enemy != m_enemies.end();) {
        if (!(*enemy)->isDead() && !(*enemy)->isSpawning() && !player.isDead() && player.hits(*(*enemy))) {
            player.die();
            soundtrack.decrease_volume();
            game_over = true;
        }

        for (auto bullet = bullet_set.m_bullets.begin(); bullet != bullet_set.m_bullets.end();) {
            if (!(*bullet)->isDead() && (*bullet)->hits(*(*enemy)) && !(*enemy)->isDead()) {

                (*bullet)->explodeBullet();
                (*enemy)->removeLife();

                if ((*enemy)->getLives() == 0) {
                    (*enemy)->die();
                    game_score += (*enemy)->getScore();
                    if (game_score > highscore) {
                        highscore = game_score;
                        save_highscore(highscore);
                    }
                    mciSendString("PLAY explosion from 0",0,0,0);
                    break;
                } else
                    (*enemy)->highlightOn();
            }
            ++bullet;
        }
        ++enemy;
    }  

    for (auto enemy = m_enemies.begin(); enemy != m_enemies.end();) {
        if ((*enemy)->isDeadCompletely()) {
            delete (*enemy);
            m_enemies.erase(enemy++);
        } else {
            (*enemy)->updateAll(player_center, dt);
            ++enemy;
        } 
    }  
}

void EnemySet::draw() {
    for (auto enemy = m_enemies.begin(); enemy != m_enemies.end(); ++enemy)
        (*enemy)->drawEnemy();
}

Enemy* EnemySet::chooseEnemy() {
    int enemy_type;
    if (m_enemies_created < 3) {
        enemy_type = m_enemies_created + 1;
    } else {
        std::uniform_int_distribution<int> uniform(1, 3);
        enemy_type = uniform(generator);
    }
    m_enemies_created++;
    switch(enemy_type) {
        case 1:
            return new EnemyTypeOne(m_original_enemy1);
        case 2:
            return new EnemyTypeTwo(m_original_enemy2);
        case 3:
            return new EnemyTypeThree(m_original_enemy3);
        default:
            log_error_and_exit("Something went wrong when creating new enemy.");
            return 0;
    }
}

void EnemySet::explodeAll() {
    for (auto enemy = m_enemies.begin(); enemy != m_enemies.end(); ++enemy) {
        if (!(*enemy)->isDead())
            (*enemy)->die();  
    }
}