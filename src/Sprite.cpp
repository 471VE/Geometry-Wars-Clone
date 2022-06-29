#include <fstream>
#include <algorithm>

#include "Sprite.h"
#include "BMP_Headers.h"
#include "Engine.h"


Sprite::Sprite(int rows, int cols, int channels, int initValue)
    : m_rows(rows)
    , m_cols(cols)
    , m_channels(std::max(channels, 1))
    , m_data(rows * cols * m_channels, initValue)
{}


// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask)
        log_error_and_exit("Unexpected color mask format! Pixel data must be in the BGRA format.");

    if (expected_color_header.color_space_type != bmp_color_header.color_space_type)
        log_error_and_exit("Unexpected color space type! It must be sRGB.");
}


Sprite::Sprite(const char *fname) {
    std::ifstream input_file{ fname, std::ios_base::binary };
    if (!input_file)
        log_error_and_exit("Unable to open the input image file.");

    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;

    input_file.read((char*)&file_header, sizeof(file_header));
    if(file_header.file_type != 0x4D42)
        log_error_and_exit("Unrecognized file format.");
    
    input_file.read((char*)&bmp_info_header, sizeof(bmp_info_header));

    if(bmp_info_header.bit_count != 32)
        log_error_and_exit("For purposes of this game images must contain alpha channel.");
    
    if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
        input_file.read((char*)&bmp_color_header, sizeof(bmp_color_header));
        check_color_header(bmp_color_header);
    } else
        log_error_and_exit("Unrecognized file format. The file does not seem to contain bit mask information.");

    // Jump to the pixel data location
    input_file.seekg(file_header.offset_data, input_file.beg);

    if (bmp_info_header.height < 0)
        log_error_and_exit("BMP images here must be with the origin in the bottom left corner.");

    m_data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

    // Check if we need to take into account row padding
    if (bmp_info_header.width % 4 == 0)
        input_file.read((char*)m_data.data(), m_data.size());
    else {
        uint32_t row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
        for (int y = 0; y < bmp_info_header.height; ++y) {
            input_file.read((char*)(m_data.data() + row_stride * y), row_stride);
        }
    }

    m_rows = bmp_info_header.height;
    m_cols = bmp_info_header.width;
    m_channels = 4;
    m_centerX = float(SCREEN_WIDTH) / 2.f;
    m_centerY = float(SCREEN_HEIGHT) / 2.f;
}


void Sprite::draw() {
    int left_sprite_border = int(m_centerX) - m_cols / 2;
    int right_sprite_border = int(m_centerX) + m_cols / 2;
    int upper_sprite_border = int(m_centerY) - m_rows / 2;
    int lower_sprite_border = int(m_centerY) + m_rows / 2;

    int left_render_border, right_render_border, upper_render_border, lower_render_border;
    int offsetX, offsetY;

    if (left_sprite_border < 0) {
        offsetX = -left_sprite_border;
        left_render_border = 0;
    } else {
        offsetX = 0;
        left_render_border = left_sprite_border;
    }
        
    if (upper_sprite_border < 0) {
        offsetY = - upper_sprite_border;
        upper_render_border = 0;
    } else {
        offsetY = 0;
        upper_render_border = upper_sprite_border;
    }

    right_render_border = std::min(SCREEN_WIDTH, right_sprite_border);
    lower_render_border = std::min(SCREEN_HEIGHT, lower_sprite_border);    

    uint32_t pixel;
    float alpha;
    uint8_t red, green, blue;
    int buffer_red, buffer_green, buffer_blue;

    // Object's frame of reference
    int spriteX = offsetX;
    int spriteY = offsetY;

    for (int x = left_render_border; x < right_render_border; ++x) {
        for (int y = upper_render_border; y < lower_render_border; ++y) {
            
            // Get previous buffer pixel color for alpha blending
            buffer_red = (uint8_t)(buffer[y][x] >> 16);
            buffer_green = (uint8_t)(buffer[y][x] >> 8);
            buffer_blue = (uint8_t)buffer[y][x];

            // SetDIBitsToDevice does not support alpha channel so alpha blending is done manually
            alpha = float(at(spriteY, spriteX, 3)) / 255.f;
            red = uint8_t(float(at(spriteY, spriteX, 2)) * alpha + (1. - alpha) * buffer_red);
            green = uint8_t(float(at(spriteY, spriteX, 1)) * alpha + (1. - alpha) * buffer_green);
            blue = uint8_t(float(at(spriteY, spriteX, 0)) * alpha + (1. - alpha) * buffer_blue);

            pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
            buffer[y][x] = pixel;
            ++spriteY;
        }
        spriteY = offsetY;
        ++spriteX;
    }
}