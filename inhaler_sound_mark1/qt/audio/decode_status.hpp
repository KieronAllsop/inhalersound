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
    started,
    status,
    buffer_ready,
    finished,
    operation_aborted,
    invalid_buffer,
    format_not_supported,
    wrong_decoder_for_stream,
    resource_error,
    access_denied,
    service_missing
};


inline
constexpr bool is_error( const decode_status_t& Status )
{
    switch( Status )
    {
        case decode_status_t::started                   : return false;
        case decode_status_t::status                    : return false;
        case decode_status_t::buffer_ready              : return false;
        case decode_status_t::finished                  : return false;
        case decode_status_t::operation_aborted         : return true;
        case decode_status_t::invalid_buffer            : return true;
        case decode_status_t::format_not_supported      : return true;
        case decode_status_t::wrong_decoder_for_stream  : return true;
        case decode_status_t::resource_error            : return true;
        case decode_status_t::access_denied             : return true;
        case decode_status_t::service_missing           : return true;
    }
    return true;
}

inline
constexpr const char* c_str( const decode_status_t& Status )
{
    switch( Status )
    {
        case decode_status_t::started                   : return "started";
        case decode_status_t::status                    : return "status";
        case decode_status_t::buffer_ready              : return "buffer_ready";
        case decode_status_t::finished                  : return "finished";
        case decode_status_t::operation_aborted         : return "operation_aborted";
        case decode_status_t::invalid_buffer            : return "invalid_buffer";
        case decode_status_t::format_not_supported      : return "format_not_supported";
        case decode_status_t::wrong_decoder_for_stream  : return "wrong_decoder_for_stream";
        case decode_status_t::resource_error            : return "resource_error";
        case decode_status_t::access_denied             : return "access_denied";
        case decode_status_t::service_missing           : return "service_missing";
    }
    return nullptr;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_DECODE_STATUS_HPP_INCLUDED
