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
#include "inhaler/patient_wave_details.hpp"

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

enum class retrieval_status
{
    retrieving
};

constexpr const char* c_str( retrieval_status& Status )
{
    switch( Status )
    {
        case retrieval_status::retrieving    : return "Retrieving";
    }
    return nullptr;
}

class data_retriever
{
public:

    // Type Interface
    using shared_schema_t           = inhaler::server::shared_schema_t;
    using patient_wave_details_t    = inhaler::patient_wave_details;
    using patient_wave_files_t      = std::vector<patient_wave_details_t>;
    using string_t                  = std::string;
    using timestamp_t               = boost::posix_time::ptime;
    using patient_t                 = inhaler::wave_importer::optional_patient_t;
    using optional_patient_t        = boost::optional<patient_t>;
    using result_t                  = std::tuple<string_t, string_t, int, timestamp_t, timestamp_t>;

public:

    // Creation --------------------------------------------------------------

    // Make this class non-copyable
    data_retriever( const data_retriever& other ) = delete;
    data_retriever& operator=( const data_retriever& other ) = delete;

    // Construct with a shared Schema
    explicit data_retriever( const data_model::patient& Patient, const shared_schema_t& Schema )
    : Patient_( Patient )
    , Schema_( Schema )
    {
        using results_t = patient_wave_details_t::result_t;

        const auto& PatientWaves = Schema_->patientwaves();
        const quince::query< results_t >
                Query
                    = PatientWaves
                        .select
                            (   PatientWaves->inhaler_type,
                                PatientWaves->import_timestamp,
                                PatientWaves->file_name,
                                PatientWaves->file_size,
                                PatientWaves->creation_timestamp   )
                        .where
                            (   PatientWaves->patient_id == Patient_->id  );

        for( const auto& WaveTuple: Query )
        {
            std::cout << "Inside retrieved data for loop" << std::endl;
            WaveFiles_.emplace_back( WaveTuple );
        }
    }

    // Observers -------------------------------------------------------------

    const patient_t& patient() const
    {
        return Patient_;
    }

    const patient_wave_files_t& wave_files() const
    {
        return WaveFiles_;
    }

    // Operations ------------------------------------------------------------

    void retrieve_wave_data()
    {

    }

private:

    patient_t               Patient_;
    shared_schema_t         Schema_;
    patient_wave_files_t    WaveFiles_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_DATA_RETRIEVER_HPP_INCLUDED
