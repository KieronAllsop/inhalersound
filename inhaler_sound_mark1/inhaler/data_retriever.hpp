// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_DATA_RETRIEVER_HPP_INCLUDED
#define INHALER_DATA_RETRIEVER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/wave_details.hpp"
#include "inhaler/wave_importer.hpp"
#include "inhaler/patient_wave_details.hpp"

// Quince Includes
#include <quince/quince.h>

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>

// Standard Library Includes
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <tuple>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

namespace exception
{
    // Patient will always have been retrieved before a wave file or other data is
    // retrieved from the database, only exception will be that the particular data
    // type requested is not available
    struct data_not_found : virtual boost::exception, virtual std::exception {};
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
    using wave_details_t            = inhaler::wave_details;
    using patient_wave_files_t      = std::vector< std::unique_ptr<patient_wave_details_t> >;
    using waves_iterator_t          = patient_wave_files_t::iterator;
    using waves_const_iterator_t    = patient_wave_files_t::const_iterator;
    using string_t                  = std::string;
    using timestamp_t               = boost::posix_time::ptime;
    using patient_t                 = shared_schema_t::element_type::patient_t;
    using result_t                  = std::tuple<string_t, string_t, int, timestamp_t, timestamp_t>;
    using wave_files_t              = std::vector<wave_details_t>;
    using data_t                    = std::vector<uint8_t>;


public:

    // Creation --------------------------------------------------------------

    // Make this class non-copyable
    data_retriever( const data_retriever& other ) = delete;
    data_retriever& operator=( const data_retriever& other ) = delete;

    // Construct with a shared Schema
    explicit data_retriever( const patient_t& Patient, const shared_schema_t& Schema )
    : Patient_( Patient )
    , Schema_( Schema )
    , LastImportTime_( boost::posix_time::microsec_clock::local_time() )
    {
        using results_t = patient_wave_details_t::result_t;

        const auto& PatientWaves = Schema_->patientwaves();
        const quince::query< results_t >
                Query
                    = PatientWaves
                        .where
                            (   PatientWaves->patient_id == Patient_.id  )
                        .select
                            (   PatientWaves->inhaler_type,
                                PatientWaves->import_timestamp,
                                PatientWaves->file_name,
                                PatientWaves->file_size,
                                PatientWaves->creation_timestamp   );

        for( const auto& WaveTuple: Query )
        {
            WaveFiles_.emplace_back( std::make_unique<patient_wave_details_t>( WaveTuple ) );
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

    const data_t& retrieved_file() const
    {
        return RetrievedFile_;
    }

    // Operations ------------------------------------------------------------

    waves_const_iterator_t updated_wave_data()
    {
        auto Size = WaveFiles_.size();
        auto From = LastImportTime_;
        LastImportTime_ = boost::posix_time::microsec_clock::local_time();
        get_new_wave_details( From );
        // Return an iterator to the start of the new files, if any
        return WaveFiles_.begin() + Size;
    }

    void retrieve_wave( const patient_wave_details_t& Selected )
    {
        const auto& PatientWaves = Schema_->patientwaves();
        const quince::query< data_t >
                Query
                    = PatientWaves
                        .where
                            (       PatientWaves->patient_id         == Patient_.id
                                &&  PatientWaves->inhaler_type       == Selected.inhaler_model()
                                &&  PatientWaves->file_name          == Selected.name()
                                &&  PatientWaves->creation_timestamp == Selected.modified_time()    )
                        .select
                            (   PatientWaves->wave_file   );

        for( const auto& Wave: Query )
        {
            RetrievedFile_ = Wave;
        }
    }

private:

    void get_new_wave_details( const boost::posix_time::ptime& From )
    {
        using results_t = patient_wave_details_t::result_t;

        const auto& PatientWaves = Schema_->patientwaves();
        const quince::query< results_t >
                Query
                    = PatientWaves
                        .where
                            (       PatientWaves->patient_id == Patient_.id
                                &&  PatientWaves->import_timestamp > From   )
                        .select
                            (   PatientWaves->inhaler_type,
                                PatientWaves->import_timestamp,
                                PatientWaves->file_name,
                                PatientWaves->file_size,
                                PatientWaves->creation_timestamp   );

        for( const auto& WaveTuple: Query )
        {
            WaveFiles_.emplace_back( std::make_unique<patient_wave_details_t>( WaveTuple ) );
        }
    }

private:

    patient_t                   Patient_;
    shared_schema_t             Schema_;
    patient_wave_files_t        WaveFiles_;
    boost::posix_time::ptime    LastImportTime_;
    data_t                      RetrievedFile_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_DATA_RETRIEVER_HPP_INCLUDED
