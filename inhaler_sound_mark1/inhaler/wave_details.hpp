// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_WAVE_DETAILS_HPP_INCLUDED
#define INHALER_WAVE_DETAILS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  wave_details.hpp
//! \author Kieron Allsop
//!
//! \brief  WAV file details
//!
class wave_details
{
public:

    using path_t        = boost::filesystem::path;
    using timestamp_t   = boost::posix_time::ptime;
    using size_t        = int;

public:

    wave_details
        (   const path_t&       Path,
            const timestamp_t&  Modified,
            const size_t        Size   )
    : Path_( Path )
    , Modified_( Modified )
    , Size_( Size )
    {
    }


    const path_t& path() const
    {
        return Path_;
    }


    const timestamp_t& modified_time() const
    {
        return Modified_;
    }


    const size_t& size() const
    {
        return Size_;
    }

private:

    path_t          Path_;
    timestamp_t     Modified_;
    size_t          Size_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_WAVE_DETAILS_HPP_INCLUDED
