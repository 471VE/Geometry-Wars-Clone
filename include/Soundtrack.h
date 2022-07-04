#pragma once

#include <windows.h>
#include <string>
#include "Engine.h"

#define STOPPING_PLAYBACK 0
#define INCREASING_VOLUME 1
#define DECREASING_VOLUME 2
#define IDLE              3

class Soundtrack {
    public:
        Soundtrack(std::string handle);
        
        void decrease_volume(float target_volume = 400.f);
        void increase_volume(float target_volume = 1000.f);
        void stop();
        void start();
        void update(float dt);

    protected:
        std::string m_handle;

        float m_max_change_time = DEATH_TIME;
        float m_volume = 0.f;
        float m_target_volume = 1000.f;
        float m_volume_change_rate;
        int status = IDLE;
};

extern Soundtrack game_soundtrack;