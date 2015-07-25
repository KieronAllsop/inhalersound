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
#include <tuple>

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>

// Quince Includes
#include <quince/quince.h>

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

class data_retriever
{
public:

    // Type Interface
    using shared_schema_t       = inhaler::server::shared_schema_t;
    using string_t              = std::string;
    using timestamp_t           = boost::posix_time::ptime;
    using patient_t             = inhaler::wave_importer::patient_t;
    using optional_patient_t    = boost::optional<patient_t>;
    using results_t             = std::tuple<string_t, string_t, int, timestamp_t, timestamp_t>;

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


void get_all_wave_data()
{
    const auto& PatientWaves = Schema_->patientwaves();
    const quince::query< results_t >
        Query =
            PatientWaves
                .select
                    (   PatientWaves->inhaler_type,
                        PatientWaves->file_name,
                        PatientWaves->file_size,
                        PatientWaves->creation_timestamp,
                        PatientWaves->import_timestamp   )
                .where
                    (   PatientWaves->patient_id == Patient_->id   );
}


private:

    shared_schema_t         Schema_;
    optional_patient_t      Patient_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_DATA_RETRIEVER_HPP_INCLUDED
