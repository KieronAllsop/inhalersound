// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_DECODE_STATUS_HPP_INCLUDED
#define QT_AUDIO_DECODE_STATUS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

enum class decode_status_t
{
    buffer_ready,
    invalid_buffer,
    finished,
    format_not_supported,
    error
};

inline
constexpr const char* c_str( const decode_status_t& Status )
{
    switch( Status )
    {
        case decode_status_t::buffer_ready         : return "buffer_ready";
        case decode_status_t::invalid_buffer       : return "invalid_buffer";
        case decode_status_t::finished             : return "finished";
        case decode_status_t::format_not_supported : return "format_not_supported";
        case decode_status_t::error                : return "error";
    }
    return nullptr;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_DECODE_STATUS_HPP_INCLUDED
