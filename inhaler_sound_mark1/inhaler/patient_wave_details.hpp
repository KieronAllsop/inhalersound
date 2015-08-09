// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_PATIENT_WAVE_DETAILS_HPP_INCLUDED
#define INHALER_PATIENT_WAVE_DETAILS_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Standard Library Includes
#include <tuple>
#include <string>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class patient_wave_details
{
public:

    using string_t      = std::string;
    using timestamp_t   = boost::posix_time::ptime;
    using size_t        = int; //std::streamsize;
    using result_t      = std::tuple< string_t, timestamp_t, string_t, size_t, timestamp_t >;

public:

    patient_wave_details()
    : InhalerModel_ ( "" )
    , ImportTime_   ( timestamp_t() )
    , Name_         ( "" )
    , Size_         ( 0 )
    , ModifiedTime_ ( timestamp_t() )
    {
    }

    explicit patient_wave_details( const result_t& Details )
    : InhalerModel_ ( std::get<0>(Details) )
    , ImportTime_   ( std::get<1>(Details) )
    , Name_         ( std::get<2>(Details) )
    , Size_         ( std::get<3>(Details) )
    , ModifiedTime_ ( std::get<4>(Details) )
    {
    }

    patient_wave_details( const patient_wave_details& other ) = default;
    patient_wave_details& operator=( const patient_wave_details& other ) = default;

    const string_t& inhaler_model() const
    {
        return InhalerModel_;
    }

    const timestamp_t& import_time() const
    {
        return ImportTime_;
    }

    const string_t& name() const
    {
        return Name_;
    }

    const size_t& size() const
    {
        return Size_;
    }

    const timestamp_t& modified_time() const
    {
        return ModifiedTime_;
    }

private:

    string_t        InhalerModel_;
    timestamp_t     ImportTime_;
    string_t        Name_;
    size_t          Size_;
    timestamp_t     ModifiedTime_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_PATIENT_WAVE_DETAILS_HPP_INCLUDED
