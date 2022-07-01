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
        Sprite(const char *fname);

        inline int getHeight() const { return m_height; }
        inline int getWidth() const { return m_width; }
        inline int getChannels() const { return m_channels; }
        inline std::vector<uint8_t> getData() const { return m_data; }

        inline const uint8_t& at(int row, int col, int channel) const {
            return m_data[m_channels * ( m_width * (m_height - 1 - row) + col) + channel];
        }
        inline uint8_t& at(int row, int col, int channel) {
            return m_data[m_channels * ( m_width * (m_height - 1 - row) + col) + channel];
        }

    protected:
        int m_height;
        int m_width;
        int m_channels = 4;
        std::vector<uint8_t> m_data;
};