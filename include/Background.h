#pragma once

#include "Sprite.h"

class Background: public Sprite {
    public:
        Background(const char* fname)
            : Sprite(fname)
        {}
        void draw();
        void draw_with_transparency(float transparency = 1.f);
};

