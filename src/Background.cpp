#include "Background.h"


void Background::draw() {
    uint8_t red, green, blue;
    uint32_t pixel;
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (at(y, x, 3) > 0) {
                red = uint8_t(at(y, x, 2));
                green = uint8_t(at(y, x, 1));
                blue = uint8_t(at(y, x, 0));

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }
        }
   }
}

uint8_t clamp_float(float x) {
    if (x > 255.f)
        return 255;
    return uint8_t(x);
}

void Background::draw_with_transparency(float transparency) {
    float alpha;
    uint8_t red, green, blue;
    int buffer_red, buffer_green, buffer_blue;
    uint32_t pixel;
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (at(y, x, 3) > 0) {
                alpha = float(at(y, x, 3)) / 255.f;

                buffer_red = (uint8_t)(buffer[y][x] >> 16);
                buffer_green = (uint8_t)(buffer[y][x] >> 8);
                buffer_blue = (uint8_t)buffer[y][x];

                red = clamp_float((float(at(y, x, 2)) * alpha + (1.f - alpha) * buffer_red) * transparency);
                green = clamp_float((float(at(y, x, 1)) * alpha + (1.f - alpha) * buffer_green) * transparency);
                blue = clamp_float((float(at(y, x, 0)) * alpha + (1.f - alpha) * buffer_blue) * transparency);

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }
        }
    }
}
