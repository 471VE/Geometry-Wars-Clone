#pragma once

#include <vector>
#include <cmath>
#include "Primitives.h"

#define M_PI 3.141592741f
#define M_PI_2 1.570796371f
#define M_PI_4 0.7853981853f

void check_angle(float& angle);

class Sprite {
    public:
        Sprite(const char *fname,   float centerX = float(SCREEN_WIDTH) / 2.f, float centerY = float(SCREEN_HEIGHT) / 2.f,
            float angular_velocity = 2 * M_PI, float velocity = 200.f);

        inline int getHeight() const { return m_height; }
        inline int getWidth() const { return m_width; }
        inline int getChannels() const { return m_channels; }
        inline float getAngle() const { return m_angle; }
        inline float getCenterX() const { return m_centerX; }
        inline float getCenterY() const { return m_centerY; }
        
        inline const uint8_t& at_render(int row, int col, int channel) const {
            // Reverse in Y axis since BMP stores image from bottom
            return m_data_render[m_channels * ( m_width_render * (m_height_render - 1 - row) + col) + channel];
        }
        inline uint8_t& at_render(int row, int col, int channel) {
            return m_data_render[m_channels * ( m_width_render * (m_height_render - 1 - row) + col) + channel];
        }

        inline const uint8_t& at(int row, int col, int channel) const {
            return m_data[m_channels * ( m_width * (m_height - 1 - row) + col) + channel];
        }
        inline uint8_t& at(int row, int col, int channel) {
            return m_data[m_channels * ( m_width * (m_height - 1 - row) + col) + channel];
        }

        void draw();

        void move(Point expected_direction, float dt);

        void rotateClockWise(float dt) {
            m_angle += m_angular_velocity * dt;
            rotate(m_angle);
        }
        void rotateCounterClockWise(float dt) {
            m_angle -= m_angular_velocity * dt;
            rotate(m_angle);
        }

        void getToAngle(float angle, float dt, float difference, float angular_velocity);
        void getToAngleUniform(float angle, float dt);
        void getToAngleNonUniform(float angle, float dt, float time = 0.3f);



    protected:
        int m_height;
        int m_height_render;

        int m_width;
        int m_width_render;

        int m_channels;

        std::vector<uint8_t> m_data;
        std::vector<uint8_t> m_data_render;

        float m_centerX;
        float m_centerY;
        float m_velocity;

        float m_angle = 0;
        float m_angular_velocity;
        float m_angle_threshold = M_PI/90;

        float m_last_velocity = m_velocity;
        Point m_last_velocity_direction{0.f, 0.f};

        void checkAngle();
        void rotate(float angle);

        inline float getAngleDifference(float angle) {
            float difference = angle - m_angle;
            check_angle(difference);
            return difference;
        }
};