// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_PLAY_STATUS_HPP_INCLUDED
#define QT_AUDIO_PLAY_STATUS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

enum class play_status_t
{
    playing,
    paused,
    stopped,
    position_changed
};

inline
constexpr const char* c_str( const play_status_t& Status )
{
    switch( Status )
    {
        case play_status_t::playing          : return "playing";
        case play_status_t::paused           : return "paused";
        case play_status_t::stopped          : return "stopped";
        case play_status_t::position_changed : return "position_changed";
    }
    return nullptr;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_PLAY_STATUS_HPP_INCLUDED
