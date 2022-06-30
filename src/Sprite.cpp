#include <fstream>
#include <algorithm>
#include <array>

#include "Sprite.h"
#include "BMP_Headers.h"
#include "Engine.h"


void check_angle(float& angle) {
    if (angle > M_PI)
        angle -= 2 * M_PI;
    if (angle <= -M_PI)
        angle += 2 * M_PI;
}

// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask)
        log_error_and_exit("Unexpected color mask format. Pixel data must be in the BGRA format.");

    if (expected_color_header.color_space_type != bmp_color_header.color_space_type)
        log_error_and_exit("Unexpected color space type (must be sRGB).");
}


// Load sprite from .bmp
Sprite::Sprite(const char *fname) {
    std::ifstream input_file{ fname, std::ios_base::binary };
    if (!input_file)
        log_error_and_exit("Unable to open the input image file.");
    {
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

        m_height = bmp_info_header.height;
        m_width = bmp_info_header.width;
    }
    m_channels = 4;
    m_centerX = float(SCREEN_WIDTH) / 2.f;
    m_centerY = float(SCREEN_HEIGHT) / 2.f;

    m_data_render = m_data;
    m_height_render = m_height;
    m_width_render = m_width;
}

struct WeightedPoint {
    WeightedPoint(int x, int y, float weight)
        : x(x)
        , y(y)
        , weight(weight)
    {}

    int x, y;
    float weight;
};


// Rotation by Area Mapping with bilinear interpolation

void Sprite::rotate(float angle) { 
    m_angle = angle;
    checkAngle();

    float sinAngle = std::sin(m_angle);
    float cosAngle = std::cos(m_angle);
    float newHeightRaw, newWidthRaw;

    if (std::abs(m_angle) <= M_PI_2) {
        newHeightRaw = m_width * std::sin(std::abs(m_angle)) + m_height * std::cos(std::abs(m_angle));
        newWidthRaw = m_width * std::cos(std::abs(m_angle)) + m_height * std::sin(std::abs(m_angle));
    }
    else if (std::abs(m_angle) <= M_PI) {
        newHeightRaw = m_width * std::sin(M_PI - std::abs(m_angle))
            + m_height * std::sin(std::abs(m_angle) - M_PI_2);
        newWidthRaw = m_width * std::cos(M_PI - std::abs(m_angle))
            + m_height * std::cos(std::abs(m_angle) - M_PI_2);
    }

    m_width_render = (int)std::round(newWidthRaw);
    m_height_render = (int)std::round(newHeightRaw);

    m_data_render.clear();
    m_data_render.resize(m_width_render * m_height_render * m_channels);

    for (int x = 0; x < m_height_render; x++) {
        for (int y = 0; y < m_width_render; y++) {
            float xt = x - (newHeightRaw - 1) / 2.f;
            float yt = y - (newWidthRaw - 1) / 2.f;

            float originalX = xt * cosAngle - yt * sinAngle + (m_height - 1) / 2.f;
            float originalY = xt * sinAngle + yt * cosAngle + (m_width - 1) / 2.f;

            float fractionX = std::abs(originalX - std::round(originalX));
            float fractionY = std::abs(originalY - std::round(originalY));

            std::array<WeightedPoint, 4> neighbors = {
                WeightedPoint(int(std::round(originalX)), int(std::round(originalY)), (1 - fractionX) * (1 - fractionY)),
                WeightedPoint(int(std::round(originalX)), int(std::round(originalY)) + 1, fractionX * (1 - fractionY)),
                WeightedPoint(int(std::round(originalX)) + 1, int(std::round(originalY)), (1 - fractionX) * fractionY),
                WeightedPoint(int(std::round(originalX)) + 1, int(std::round(originalY)) + 1, fractionX * fractionY)
            };

            bool hasData = false;
            std::array<float, 4> newPixel = { 0 };
            float weight = 0;

            for (const auto& neighbor: neighbors) {
                if (neighbor.x >= 0 && neighbor.x < m_height
                    && neighbor.y >= 0 && neighbor.y < m_width) {
                    hasData = true;
                    for (int channel = 0; channel < 4; ++channel)
                        newPixel[channel] += neighbor.weight * at(neighbor.x, neighbor.y, channel);
                    weight += neighbor.weight;
                }
            }                    
            if ((hasData) && (weight != 0)) {
                for (int channel = 0; channel < 4; ++channel)
                    at_render(x, y, channel) = uint8_t(newPixel[channel] / weight);                        
            }
        }                
    }
}
   

void Sprite::draw() {
    int left_sprite_border = int(m_centerX) - m_width_render / 2;
    int right_sprite_border = int(m_centerX) + m_width_render / 2;
    int upper_sprite_border = int(m_centerY) - m_height_render / 2;
    int lower_sprite_border = int(m_centerY) + m_height_render / 2;

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

            // SetDIBitsToDevice does not support alpha channel so alpha blending is done manually
            alpha = float(at_render(spriteY, spriteX, 3)) / 255.f;
            
            if (alpha > 0) {
                // Get previous buffer pixel color for alpha blending
                buffer_red = (uint8_t)(buffer[y][x] >> 16);
                buffer_green = (uint8_t)(buffer[y][x] >> 8);
                buffer_blue = (uint8_t)buffer[y][x];

                red = uint8_t(float(at_render(spriteY, spriteX, 2)) * alpha + (1. - alpha) * buffer_red);
                green = uint8_t(float(at_render(spriteY, spriteX, 1)) * alpha + (1. - alpha) * buffer_green);
                blue = uint8_t(float(at_render(spriteY, spriteX, 0)) * alpha + (1. - alpha) * buffer_blue);

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }

            ++spriteY;
        }
        spriteY = offsetY;
        ++spriteX;
    }
}


void Sprite::getToAngle(float angle, float dt, float difference, float angular_velocity) {
    if (difference > m_angle_threshold) {
        m_angle += angular_velocity * dt;
        rotate(m_angle);
    } else if (difference < -m_angle_threshold) {
        m_angle -= angular_velocity * dt;
        rotate(m_angle);
    } else {
        m_angle = angle;
        rotate(m_angle);
    }
}

void Sprite::getToAngleUniform(float angle, float dt) {
    float difference = getAngleDifference(angle);
    getToAngle(angle, dt, difference, m_angular_velocity);
}

void Sprite::getToAngleNonUniform(float angle, float dt, float time) {
    float difference = getAngleDifference(angle);
    float angular_velocity =  2 * std::abs(difference / time);
    getToAngle(angle, dt, difference, angular_velocity);
}

void Sprite::moveWithInertia(Point expected_direction, float dt) {
    float angle;
    if ((expected_direction.x == 0.f) && (expected_direction.y == 0.f))
        angle = getAngle();
    else 
        angle = -expected_direction.getAngle() + M_PI_2;
    getToAngleNonUniform(angle, dt);
    Point direction(-m_angle + M_PI_2);
    float velocity;
    if ((expected_direction.x == 0.f) && (expected_direction.y == 0.f)) {
        m_last_velocity /= (dt * (1 / dt + 2));
        velocity = m_last_velocity;
    } else {
        m_last_velocity_direction = direction;
        m_last_velocity = m_velocity;
        velocity = m_velocity;
    }
    m_centerY -= m_last_velocity_direction.y * velocity * dt;
    m_centerX += m_last_velocity_direction.x * velocity * dt;
}

void Sprite::move(Point expected_direction, float dt) {
    m_centerY -= expected_direction.y * m_velocity * dt;
    m_centerX += expected_direction.x * m_velocity * dt;
}

void Sprite::checkAngle() {
    if (m_angle > M_PI)
        m_angle -= 2 * M_PI;
    if (m_angle <= -M_PI)
        m_angle += 2 * M_PI;
}