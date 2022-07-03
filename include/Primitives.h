#pragma once

#include <cmath>
#include "Engine.h"


struct Point {
    float x, y;

    float getAngle() {
        return std::atan2(y, x) + M_PI_2;
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
        : x(std::cos(angle - M_PI_2))
        , y(std::sin(angle - M_PI_2))
    {}

    Point(float x, float y)
        : x(x)
        , y(y)
    {}

    Point()
        : x(0)
        , y(0)
    {}
};