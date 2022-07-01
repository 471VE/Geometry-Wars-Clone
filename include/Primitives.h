#pragma once

#include <cmath>
#include "Engine.h"


struct Point {
    float x, y;

    float getAngle() {
        return std::atan2(y, x);
    }

    void normalize() {
        float squared_length = x * x + y * y;
        if (squared_length > 0.f) {
            float length = std::sqrt(squared_length);
            x /= length;
            y /= length;
        }
    }
    

    Point& operator=(const Point& point) {
        if (this != &point) {
            x = point.x;
            y = point.y;
        }
        return *this;
    }

    Point(float angle)
        : x(std::cos(angle))
        , y(std::sin(angle))
    {}

    Point(float x, float y)
        : x(x)
        , y(y)
    {}
};


struct Circle {
    Point center;
    float radius;
    float speed = 200.f;
    int hexColor = 0x0000FF00;

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

    void draw();
};