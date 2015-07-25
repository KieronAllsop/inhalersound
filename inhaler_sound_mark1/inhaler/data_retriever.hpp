// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_DATA_RETRIEVER_HPP_INCLUDED
#define INHALER_DATA_RETRIEVER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <fstream>

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/wave_details.hpp"
#include "inhaler/wave_importer.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

namespace exception
{
    // Patient will always have been retrieved before a wave file or other data is
    // retrieved from the database, only exception will be that the particular data
    // type requested is not available
    struct data_not_found    : virtual boost::exception, virtual std::exception {};
}

enum class import_status
{
    retrieving
};

constexpr const char* c_str( import_status& Status )
{
    switch( Status )
    {
        case import_status::retrieving    : return "Retrieving";
    }
    return nullptr;
}


class data_retriever
{
public:

    // Type Interface
    using shared_schema_t       = inhaler::server::shared_schema_t;
    using wave_details_t        = inhaler::wave_details;
 //   using import_handler_t      = std::function< void( const wave_details_t&, int, import_status ) >;
    using date_t                = boost::posix_time::ptime;
    using timestamp_t           = boost::posix_time::ptime;
    using patient_t             = data_model::patient;
    using optional_patient_t    = boost::optional<patient_t>;
    using wave_files_t          = std::vector<wave_details_t>;
    using data_t                = std::vector<uint8_t>;

public:

    // Creation --------------------------------------------------------------

    // Make this class non-copyable
    data_retriever( const data_retriever& other ) = delete;
    data_retriever& operator=( const data_retriever& other ) = delete;

    // Construct with a shared Schema
    explicit data_retriever( const shared_schema_t& Schema )
    : Schema_( Schema )
    {
    }

    // Observers -------------------------------------------------------------

    const std::string& inhaler_model() const
    {
        return InhalerModel_;
    }

    const wave_files_t& wave_files() const
    {
        return WaveFiles_;
    }

    // Operations ------------------------------------------------------------

    void set_inhaler_model
        (   const std::string& InhalerModel   )
    {
        InhalerModel_ = InhalerModel;
    }

    void set_wave_files
        (   wave_files_t&& WaveFiles   )
    {
        WaveFiles_ = std::move( WaveFiles );
    }


//    void set_import_date

private:

    shared_schema_t         Schema_;
    optional_patient_t      Patient_;
    std::string             InhalerModel_;
    wave_files_t            WaveFiles_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_DATA_RETRIEVER_HPP_INCLUDED
