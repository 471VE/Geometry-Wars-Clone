#include "Background.h"


void Background::draw() {
    uint8_t red, green, blue;
    uint32_t pixel;
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (at(y, x, 3) == 255) {
                red = uint8_t(at(y, x, 2));
                green = uint8_t(at(y, x, 1));
                blue = uint8_t(at(y, x, 0));

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }
        }
   }
}