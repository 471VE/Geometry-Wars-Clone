#pragma once

#include "Sprite.h"

class Background: public Sprite {
    public:
        Background(const char* fname)
            : Sprite(fname)
        {}
        void draw();
};

