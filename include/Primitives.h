#pragma once

#include "Engine.h"


struct Point {
    float x, y;
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