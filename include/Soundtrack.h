#pragma once

#include <windows.h>
#include <string>
#include "Engine.h"

class Soundtrack {
    public:
        Soundtrack() {}

        void decrease_volume() {
            m_target_volume = 400;
            m_volume_change_rate = (m_target_volume - m_volume) / m_max_change_time;
        }

        void increase_volume() {
            m_target_volume = 1000;
            m_volume_change_rate = (m_target_volume - m_volume) / m_max_change_time;
        }

        void update(float dt) {
            if (m_volume != m_target_volume) {
                m_volume += m_volume_change_rate * dt;
                if (m_volume > 1000.f)
                    m_volume = 1000.f;
                else if (m_volume < 400.f)
                    m_volume = 400.f;
                std::string change_volume_string = "SetAudio soundtrack volume to " + std::to_string(int(m_volume));
                mciSendString(change_volume_string.c_str(),0,0,0);
            }
        }

    protected:
        float m_change_time = 0.f;
        float m_max_change_time = DEATH_TIME;
        float m_volume = 1000.f;
        float m_target_volume;
        float m_volume_change_rate;
};

extern Soundtrack soundtrack;