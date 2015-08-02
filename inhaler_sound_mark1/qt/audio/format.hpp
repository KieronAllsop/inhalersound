// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_FORMAT_HPP_INCLUDED
#define QT_AUDIO_FORMAT_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
//None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

enum class sample_type_t
{
    unknown,
    signed_int,
    unsigned_int,
    floating_point
};

inline
constexpr const char* c_str( const sample_type_t& Type )
{
    switch( Type )
    {
        case sample_type_t::unknown         : return "unknown";
        case sample_type_t::signed_int      : return "signed_int";
        case sample_type_t::unsigned_int    : return "unsigned_int";
        case sample_type_t::floating_point  : return "floating_point";
    }
    return nullptr;
}

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_FORMAT_HPP_INCLUDED
