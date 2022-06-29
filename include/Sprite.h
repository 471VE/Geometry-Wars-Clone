#pragma once

#include <vector>

#include "Primitives.h"


class Sprite {
    public:
        Sprite(int rows, int cols, int channels, int initValue);
        Sprite(const char *fname);

        inline int getRows() const { return m_rows; }
        inline int getCols() const { return m_cols; }
        inline int getChannels() const { return m_channels; }
        
        inline const uint8_t& at(int row, int col, int channel) const {
            // Reverse in Y axis since BMP stores image from bottom
            return m_data[m_channels * ( m_cols * (m_rows - 1 - row) + col) + channel];
        }
        inline uint8_t& at(int row, int col, int channel) {
            // Reverse in Y axis since BMP stores image from bottom
            return m_data[m_channels * ( m_cols * (m_rows - 1 - row) + col) + channel];
        }

        void draw();

        void setCenter(Point new_center) {
            m_centerX = new_center.x;
            m_centerY = new_center.y;
        }

        inline void moveUp(float dt) { m_centerY -= speed * dt; }
        inline void moveDown(float dt) { m_centerY += speed * dt; }        
        inline void moveLeft(float dt) { m_centerX -= speed * dt; }
        inline void moveRight(float dt) { m_centerX += speed * dt; }

    protected:
        int m_rows;
        int m_cols;
        int m_channels;
        std::vector<uint8_t> m_data;
        float m_centerX;
        float m_centerY;
        float speed = 200.f;
};