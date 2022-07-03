#include <fstream>
#include <algorithm>
#include <array>

#include "Object.h"
#include "Engine.h"

uint8_t clamp_int(int x) {
    if (x > 255)
        return 255;
    return uint8_t(x);
}


void check_angle(float& angle) {
    if (angle > M_PI)
        angle -= 2 * M_PI;
    if (angle <= -M_PI)
        angle += 2 * M_PI;
}


// Load Object from .bmp
Object::Object(const char *fname, float centerX, float centerY, float angular_velocity, float velocity)
    : Sprite(fname)
    , m_centerX(centerX)
    , m_centerY(centerY)   
    , m_velocity(velocity)
    , m_angular_velocity(angular_velocity)
{
    m_data_render = m_data;
    m_height_render = m_height;
    m_width_render = m_width;
    m_hitbox_radius = float(m_height / 2) * 0.7f;
}

Object::Object(const Object& object)
    : Sprite(object)
    , m_centerX(object.m_centerX)
    , m_centerY(object.m_centerY)
    , m_velocity(object.m_velocity)
    , m_angular_velocity(object.m_angular_velocity)
    , m_data_render(object.m_data)
    , m_height_render(object.m_height)
    , m_width_render(object.m_width)
    , m_hitbox_radius(object.m_hitbox_radius)
    , m_last_velocity_direction(object.m_last_velocity_direction)
{}


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

void Object::rotate(float angle) { 
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

uint8_t clamp(float x) {
    if (x > 255.f)
        return 255;
    return uint8_t(x);
}

void Object::draw(float add_value) {
    int left_Object_border = int(m_centerX) - m_width_render / 2;
    int right_Object_border = int(m_centerX) + m_width_render / 2;
    int upper_Object_border = int(m_centerY) - m_height_render / 2;
    int lower_Object_border = int(m_centerY) + m_height_render / 2;

    int left_render_border, right_render_border, upper_render_border, lower_render_border;
    int offsetX, offsetY;

    if (left_Object_border < 0) {
        offsetX = -left_Object_border;
        left_render_border = 0;
    } else {
        offsetX = 0;
        left_render_border = left_Object_border;
    }
        
    if (upper_Object_border < 0) {
        offsetY = - upper_Object_border;
        upper_render_border = 0;
    } else {
        offsetY = 0;
        upper_render_border = upper_Object_border;
    }

    right_render_border = std::min(SCREEN_WIDTH, right_Object_border);
    lower_render_border = std::min(SCREEN_HEIGHT, lower_Object_border);    

    uint32_t pixel;
    float alpha;
    uint8_t red, green, blue;
    int buffer_red, buffer_green, buffer_blue;

    // Object's frame of reference
    int ObjectX = offsetX;
    int ObjectY = offsetY;

    for (int x = left_render_border; x < right_render_border; ++x) {
        for (int y = upper_render_border; y < lower_render_border; ++y) {

            // SetDIBitsToDevice does not support alpha channel so alpha blending is done manually
            alpha = float(at_render(ObjectY, ObjectX, 3)) / 255.f;
            
            if (alpha > 0) {
                // Get previous buffer pixel color for alpha blending
                buffer_red = (uint8_t)(buffer[y][x] >> 16);
                buffer_green = (uint8_t)(buffer[y][x] >> 8);
                buffer_blue = (uint8_t)buffer[y][x];

                red = clamp(float(at_render(ObjectY, ObjectX, 2)) * alpha + (1.f - alpha) * buffer_red + add_value);
                green = clamp(float(at_render(ObjectY, ObjectX, 1)) * alpha + (1.f - alpha) * buffer_green + add_value);
                blue = clamp(float(at_render(ObjectY, ObjectX, 0)) * alpha + (1.f - alpha) * buffer_blue + add_value);

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }

            ++ObjectY;
        }
        ObjectY = offsetY;
        ++ObjectX;
    }
}

void Object::move(Point expected_direction, float dt) {
    m_centerY += expected_direction.y * m_velocity * dt;
    m_centerX += expected_direction.x * m_velocity * dt;
}

void Object::checkAngle() {
    if (m_angle > M_PI)
        m_angle -= 2 * M_PI;
    if (m_angle <= -M_PI)
        m_angle += 2 * M_PI;
}

bool Object::hits(const Object& object) {
    float deltaX = object.m_centerX - m_centerX;
    float deltaY = object.m_centerY - m_centerY;
    bool tmp = (std::sqrt(deltaX * deltaX + deltaY * deltaY) <= m_hitbox_radius + object.m_hitbox_radius);
    return (std::sqrt(deltaX * deltaX + deltaY * deltaY) <= m_hitbox_radius + object.m_hitbox_radius);
}

void Object::resize(float scaleX, float scaleY) {
    m_height_render = int(float(m_height) * scaleY);
    m_width_render = int(float(m_width) * scaleX);

    m_data_render.clear();
    m_data_render.resize(m_width_render * m_height_render * m_channels);

    float x, x_floor, x_ceil;
    float y, y_floor, y_ceil;

    float q1, q2;
    uint8_t old1, old2, old3, old4;

	for (int i = 0; i < m_height_render; ++i) {
        for (int j = 0; j < m_width_render; ++j) {
			x = float(i) / scaleY;
			y = float(j) / scaleX;

			x_floor = std::floor(x);
			x_ceil = std::min(float(m_height - 1), std::ceil(x));
			y_floor = std::floor(y);
			y_ceil = std::min(float(m_width - 1), std::ceil(y));

			if ((x_ceil == x_floor) && (y_ceil == y_floor))
                for (int channel = 0; channel < 4; ++channel)
                    at_render(i, j, channel) = at(int(x), int(y), channel);         

            else if (x_ceil == x_floor)
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x), int(y_floor), channel);
                    old2 = at(int(x), int(y_ceil), channel);
                    at_render(i, j, channel) = uint8_t(float(old1) * (y_ceil - y) + float(old2) * (y - y_floor));
                }

            else if  (y_ceil == y_floor)
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x_floor), int(y), channel);
                    old2 = at(int(x_ceil), int(y), channel);
                    at_render(i, j, channel) = uint8_t(float(old1) * (x_ceil - x) + float(old2) * (x - x_floor));
                }

            else
                for (int channel = 0; channel < 4; ++channel) {
                    old1 = at(int(x_floor), int(y_floor), channel);
                    old2 = at(int(x_ceil), int(y_floor), channel);
                    old3 = at(int(x_floor), int(y_ceil), channel);
                    old4 = at(int(x_ceil), int(y_ceil), channel);

                    q1 = float(old1) * (x_ceil - x) + float(old2) * (x - x_floor);
                    q2 = float(old3) * (x_ceil - x) + float(old4) * (x - x_floor);
                    at_render(i, j, channel) = uint8_t(q1 * (y_ceil - y) + q2 * (y - y_floor));
                }
        }
    }
}


void Object::make_transparent(float death_time, float total_time) {
    for (int y = 0; y < m_height_render; ++y) {
        for (int x = 0; x < m_width_render; ++x) {
            at_render(y, x, 3) = uint8_t(float(at_render(y, x, 3) * (1- death_time/total_time)));
        }
    }  
}

void Object::crop(int x_first, int x_last, int y_first, int y_last) {
    m_data = m_data_render;
    m_width = m_width_render;
    m_height = m_height_render;

    m_width_render = x_last - x_first;
    m_height_render = y_last - y_first;
    m_data_render.clear();
    m_data_render.resize(m_width_render * m_height_render * m_channels, 0);

    for (int i = 0; i < m_width_render; ++i) {
        for (int j = 0; j < m_height_render; ++j) {
            if (at(y_first + j, x_first + i, 3) == 255) {
                for (int channel = 0; channel < m_channels; ++channel) {
                    at_render(j, i, channel) = at(y_first + j, x_first + i, channel);
                }
            }
        }
    }

    m_data = m_data_render;

    m_centerX = m_centerX - float(m_width) / 2.f + float(x_first + x_last) / 2.f;
    m_centerY = m_centerY - float(m_height) / 2.f + float(y_first + y_last) / 2.f;

    m_last_velocity_direction.x = float(x_first + x_last - m_width) / (2.f * float(m_width));
    m_last_velocity_direction.y = float(y_first + y_last - m_height) / (2.f * float(m_height));

    m_width = m_width_render;
    m_height = m_height_render;
    m_velocity = 600.f * rand() / static_cast<float>(RAND_MAX);
    m_angular_velocity = M_PI * rand() / static_cast<float>(RAND_MAX);
}

std::vector<Object> Object::createFragments(int chunk_sizeX, int chunk_sizeY) {
    std::uniform_real_distribution<float> uniform(0.5f, 1.5f);
    int new_width = m_width;
    while (new_width % chunk_sizeX != 0)
        new_width++;

    int new_height = m_height;
    while (new_height % chunk_sizeY != 0)
        new_height++;
    resize(float(new_width) / float(m_width), float(new_height) / float(m_height));

    m_data = m_data_render;
    m_height = m_height_render;
    m_width = m_width_render;

    int number_of_chunksX = m_width_render / chunk_sizeX;
    int number_of_chunksY = m_height_render / chunk_sizeY;

    std::vector<Object> fragments;

    for (int chunkX = 0; chunkX < number_of_chunksX; ++chunkX) {
        for (int chunkY = 0; chunkY < number_of_chunksY; ++chunkY) {
            Object fragment(*this);
            fragment.crop(chunkX * chunk_sizeX, (chunkX + 1) * chunk_sizeX, chunkY * chunk_sizeY, (chunkY + 1) * chunk_sizeY);
            fragments.push_back(fragment);
        }
    }
    return fragments;
}

void Object::moveInTheLastDirection(float dt) {
    if (m_centerX < 0.f) {
        m_last_velocity_direction.x *= -1;
        m_centerX = 0.f;
    } else if (m_centerX > SCREEN_WIDTH) {
        m_last_velocity_direction.x *= -1;
        m_centerX = float(SCREEN_WIDTH);
    }

    if (m_centerY < 0.f) {
        m_last_velocity_direction.y *= -1;
        m_centerY = 0.f;
    } else if (m_centerY > SCREEN_HEIGHT) {
        m_last_velocity_direction.y *= -1;
        m_centerY = float(SCREEN_HEIGHT);
    }
    
    move(m_last_velocity_direction, dt);
}