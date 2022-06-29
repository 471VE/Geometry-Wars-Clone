#include "Primitives.h"

#include <algorithm>

void Circle::draw() {
    float min_x = std::max(0.f, center.x - radius);
    float max_x = std::min(float(SCREEN_WIDTH), center.x + radius);
    float min_y = std::max(0.f, center.y - radius);
    float max_y = std::min(float(SCREEN_HEIGHT), center.y + radius);

    for (float x = min_x; x <= max_x; ++x) {
        for (float y = min_y; y <= max_y; ++y) {
            if (includes({x, y})) {
                buffer[size_t(y)][size_t(x)] = hexColor;
            }
        }
    }
}