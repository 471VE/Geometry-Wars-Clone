#pragma once

#include "Engine.h"

#include <algorithm>


struct Point {
    float x, y;
};


struct Circle {
    Point center;
    float radius;
    float speed = 200.f;

    bool includes(Point P) const {
        return ((center.x - P.x) * (center.x - P.x) +
                (center.y - P.y) * (center.y - P.y) <=
                radius * radius);
    }
    
    void moveUp(float dt) {
        center.y -= speed * dt;
    }

    void moveDown(float dt) {
        center.y += speed * dt;
    }
    
    void moveLeft(float dt) {
        center.x -= speed * dt;
    }

    void moveRight(float dt) {
        center.x += speed * dt;
    }
};


void draw_circle(Circle circle, int hexColor) {
    float min_x = std::max(0.f, circle.center.x - circle.radius);
    float max_x = std::min(float(SCREEN_WIDTH), circle.center.x + circle.radius);
    float min_y = std::max(0.f, circle.center.y - circle.radius);
    float max_y = std::min(float(SCREEN_HEIGHT), circle.center.y + circle.radius);

    for (float x = min_x; x <= max_x; ++x) {
        for (float y = min_y; y <= max_y; ++y) {
            if (circle.includes({x, y})) {
                buffer[size_t(y)][size_t(x)] = hexColor;
            }
        }
    }
}