#include "Soundtrack.h"

Soundtrack::Soundtrack(std::string handle)
    : m_handle(handle)
{}

void Soundtrack::decrease_volume(float target_volume) {
    if (status != STOPPING_PLAYBACK)
        status = DECREASING_VOLUME;
    m_target_volume = target_volume;
    m_volume_change_rate = (m_target_volume - m_volume) / m_max_change_time;
}

void Soundtrack::increase_volume(float target_volume) {
    status = INCREASING_VOLUME;
    m_target_volume = target_volume;
    m_volume_change_rate = (m_target_volume - m_volume) / m_max_change_time;
}

void Soundtrack::stop() {
    status = STOPPING_PLAYBACK;
    decrease_volume(0.f);
}

void Soundtrack::start() {
    std::string start_string = "Play " + m_handle + " from 0 repeat";
    mciSendString(start_string.c_str(),0,0,0);
    increase_volume();
}

void Soundtrack::update(float dt) {
    if (m_volume != m_target_volume) {
        m_volume += m_volume_change_rate * dt;
        std::string change_status;
        switch (status) {
            case STOPPING_PLAYBACK:
                if (m_volume < m_target_volume) {
                    change_status = "stop " + m_handle;
                    mciSendString(change_status.c_str(),0,0,0);
                    break;
                } else {
                    change_status = "SetAudio " + m_handle + " volume to " + std::to_string(int(m_volume));
                    mciSendString(change_status.c_str(),0,0,0);
                    break;
                }

            case INCREASING_VOLUME:
                if (m_volume > m_target_volume) {
                    m_volume = m_target_volume;
                    status = IDLE;
                }       
                change_status = "SetAudio " + m_handle + " volume to " + std::to_string(int(m_volume));
                mciSendString(change_status.c_str(),0,0,0);
                break;

            case DECREASING_VOLUME:
                if (m_volume < m_target_volume) {
                    m_volume = m_target_volume;
                    status = IDLE;
                }
                change_status = "SetAudio " + m_handle + " volume to " + std::to_string(int(m_volume));
                mciSendString(change_status.c_str(),0,0,0);
                break;
            
            case IDLE:
                break;

            default:
                log_error_and_exit("UNsupported mode for soundtrack");
        }
    }
}