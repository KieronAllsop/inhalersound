// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_WAVE_IMPORTER_HPP_INCLUDED
#define INHALER_WAVE_IMPORTER_HPP_INCLUDED
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

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


namespace exception
{
    struct patient_not_found : virtual boost::exception, virtual std::exception {};
    struct no_patient_set    : virtual boost::exception, virtual std::exception {};
}


enum class import_status
{
    reading,
    storing,
    processing,
    finished
};


constexpr const char* c_str( import_status& Status )
{
    switch( Status )
    {
        case import_status::reading    : return "Reading";
        case import_status::storing    : return "Storing";
        case import_status::processing : return "Processing";
        case import_status::finished   : return "Finished";
    }
    return nullptr;
}


class wave_importer
{
public:

    // Type Interface
    using shared_schema_t       = inhaler::server::shared_schema_t;
    using wave_details_t        = inhaler::wave_details;
    using import_handler_t      = std::function< void( const wave_details_t&, int, import_status ) >;
    using date_t                = boost::posix_time::ptime;
    using timestamp_t           = boost::posix_time::ptime;
    using patient_t             = data_model::patient;
    using optional_patient_t    = boost::optional<patient_t>;
    using wave_files_t          = std::vector<wave_details_t>;
    using data_t                = std::vector<uint8_t>;

public:

    // Creation --------------------------------------------------------------

    // Make this class non-copyable
    wave_importer( const wave_importer& other ) = delete;
    wave_importer& operator=( const wave_importer& other ) = delete;

    // Construct with a shared Schema
    explicit wave_importer( const shared_schema_t& Schema )
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

    const optional_patient_t& patient() const
    {
        return Patient_;
    }

    // Operations ------------------------------------------------------------

    void set_patient
        (   const std::string&  Forename,
            const std::string&  Surname,
            const date_t&       DateOfBirth,
            const std::string&  Postcode )
    {
        const auto& Patients = Schema_->patients();

        const auto Query
                    = Patients
                        .where(     Patients->forename      == Forename
                                &&  Patients->surname       == Surname
                                &&  Patients->date_of_birth == DateOfBirth
                                &&  Patients->postcode      == Postcode );

        auto Patient = Query.begin();

        if( Patient == Query.end())
        {
            BOOST_THROW_EXCEPTION( exception::patient_not_found() );
        }
        Patient_ = *Patient;
    }

    void set_wave_files
        (   wave_files_t&& WaveFiles   )
    {
        WaveFiles_ = std::move( WaveFiles );
    }

    void set_inhaler_model
        (   const std::string& InhalerModel   )
    {
        InhalerModel_ = InhalerModel;
    }

    void import_wave_files( const import_handler_t& Handler )
    {
        if( !Patient_ )
        {
            BOOST_THROW_EXCEPTION( exception::no_patient_set() );
        }

        auto ImportTime = boost::posix_time::microsec_clock::local_time();

        int Index = 0;

        for( const auto& WaveFile: WaveFiles_ )
        {
            Handler( WaveFile, Index, import_status::reading );

            auto Data = read_file( WaveFile.path(), WaveFile.size() );

            Handler( WaveFile, Index, import_status::storing );

            store_file( WaveFile, ImportTime, Data );

            Handler( WaveFile, Index, import_status::processing );

            process_file( WaveFile, InhalerModel_ );

            Handler( WaveFile, Index, import_status::finished );

            ++Index;
        }
    }

private:

    data_t read_file( const wave_details_t::path_t& Path, wave_details_t::size_t Size )
    {
        std::ifstream File( Path.c_str(), std::ios::binary );

        data_t Data;
        Data.reserve( Size );
        Data.assign
            (   std::istreambuf_iterator<char>( File ),
                std::istreambuf_iterator<char>()   );

        return std::move( Data );
    }

    void store_file( const wave_details_t& WaveFile, const timestamp_t& ImportTime, const data_t& Data )
    {
        Schema_->patientwaves()
            .insert
                ( { Patient_->id,
                    InhalerModel_,
                    WaveFile.path().filename().string(),
                    WaveFile.modified_time(),
                    ImportTime,
                    Data,
                    WaveFile.size() } );
    }

    void process_file( const wave_details_t& WaveFile, const std::string& InhalerModel )
    {
        // TODO
    }

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
#endif // INHALER_WAVE_IMPORTER_HPP_INCLUDED
