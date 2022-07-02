#include <fstream>
#include <algorithm>
#include <array>

#include "GameObject.h"
#include "Engine.h"


void check_angle(float& angle) {
    if (angle > M_PI)
        angle -= 2 * M_PI;
    if (angle <= -M_PI)
        angle += 2 * M_PI;
}


// Load GameObject from .bmp
GameObject::GameObject(const char *fname, float centerX, float centerY, float angular_velocity, float velocity)
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

GameObject::GameObject(const GameObject& object)
    : Sprite(object)
    , m_centerX(object.m_centerX)
    , m_centerY(object.m_centerY)
    , m_velocity(object.m_velocity)
    , m_angular_velocity(object.m_angular_velocity)
    , m_data_render(object.m_data)
    , m_height_render(object.m_height)
    , m_width_render(object.m_width)
    , m_hitbox_radius(object.m_hitbox_radius)
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

void GameObject::rotate(float angle) { 
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
   

void GameObject::draw() {
    int left_GameObject_border = int(m_centerX) - m_width_render / 2;
    int right_GameObject_border = int(m_centerX) + m_width_render / 2;
    int upper_GameObject_border = int(m_centerY) - m_height_render / 2;
    int lower_GameObject_border = int(m_centerY) + m_height_render / 2;

    int left_render_border, right_render_border, upper_render_border, lower_render_border;
    int offsetX, offsetY;

    if (left_GameObject_border < 0) {
        offsetX = -left_GameObject_border;
        left_render_border = 0;
    } else {
        offsetX = 0;
        left_render_border = left_GameObject_border;
    }
        
    if (upper_GameObject_border < 0) {
        offsetY = - upper_GameObject_border;
        upper_render_border = 0;
    } else {
        offsetY = 0;
        upper_render_border = upper_GameObject_border;
    }

    right_render_border = std::min(SCREEN_WIDTH, right_GameObject_border);
    lower_render_border = std::min(SCREEN_HEIGHT, lower_GameObject_border);    

    uint32_t pixel;
    float alpha;
    uint8_t red, green, blue;
    int buffer_red, buffer_green, buffer_blue;

    // Object's frame of reference
    int GameObjectX = offsetX;
    int GameObjectY = offsetY;

    for (int x = left_render_border; x < right_render_border; ++x) {
        for (int y = upper_render_border; y < lower_render_border; ++y) {

            // SetDIBitsToDevice does not support alpha channel so alpha blending is done manually
            alpha = float(at_render(GameObjectY, GameObjectX, 3)) / 255.f;
            
            if (alpha > 0) {
                // Get previous buffer pixel color for alpha blending
                buffer_red = (uint8_t)(buffer[y][x] >> 16);
                buffer_green = (uint8_t)(buffer[y][x] >> 8);
                buffer_blue = (uint8_t)buffer[y][x];

                red = uint8_t(float(at_render(GameObjectY, GameObjectX, 2)) * alpha + (1. - alpha) * buffer_red);
                green = uint8_t(float(at_render(GameObjectY, GameObjectX, 1)) * alpha + (1. - alpha) * buffer_green);
                blue = uint8_t(float(at_render(GameObjectY, GameObjectX, 0)) * alpha + (1. - alpha) * buffer_blue);

                pixel = (((((0 << 8) | red) << 8) | green) << 8) | blue;
                buffer[y][x] = pixel;
            }

            ++GameObjectY;
        }
        GameObjectY = offsetY;
        ++GameObjectX;
    }
}

void GameObject::move(Point expected_direction, float dt) {
    m_centerY += expected_direction.y * m_velocity * dt;
    m_centerX += expected_direction.x * m_velocity * dt;
}

void GameObject::checkAngle() {
    if (m_angle > M_PI)
        m_angle -= 2 * M_PI;
    if (m_angle <= -M_PI)
        m_angle += 2 * M_PI;
}

bool GameObject::hits(const GameObject& object) {
    float deltaX = object.m_centerX - m_centerX;
    float deltaY = object.m_centerY - m_centerY;
    bool tmp = (std::sqrt(deltaX * deltaX + deltaY * deltaY) <= m_hitbox_radius + object.m_hitbox_radius);
    if (tmp) {
        float angle =0;
        return (std::sqrt(deltaX * deltaX + deltaY * deltaY) <= m_hitbox_radius + object.m_hitbox_radius);
    }
    return (std::sqrt(deltaX * deltaX + deltaY * deltaY) <= m_hitbox_radius + object.m_hitbox_radius);
}