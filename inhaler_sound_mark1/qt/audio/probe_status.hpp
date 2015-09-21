// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_PROBE_STATUS_HPP_INCLUDED
#define QT_AUDIO_PROBE_STATUS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  probe_status.hpp
//! \author Kieron Allsop
//!
//! \brief  enum class for probe status
//!
enum class probe_status_t
{
    buffer_ready,
    monitoring_not_supported,
    operation_aborted,
    invalid_buffer
};


inline
constexpr bool is_error( const probe_status_t& Status )
{
    switch( Status )
    {
        case probe_status_t::buffer_ready              : return false;
        case probe_status_t::monitoring_not_supported  : return true;
        case probe_status_t::operation_aborted         : return true;
        case probe_status_t::invalid_buffer            : return true;
    }
    return true;
}


inline
constexpr const char* c_str( const probe_status_t& Status )
{
    switch( Status )
    {
        case probe_status_t::buffer_ready              : return "buffer_ready";
        case probe_status_t::monitoring_not_supported  : return "monitoring_not_supported";
        case probe_status_t::operation_aborted         : return "operation_aborted";
        case probe_status_t::invalid_buffer            : return "invalid_buffer";
    }
    return nullptr;
}


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_PROBE_STATUS_HPP_INCLUDED
