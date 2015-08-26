// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef DATA_MODEL_SCHEMA_HPP_INCLUDED
#define DATA_MODEL_SCHEMA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Quince Includes
#include <quince/quince.h>

// Boost Includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

// C++ Standard Library Includes
#include <vector>
#include <fstream>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace data_model{
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// We place our basic data model in here. In the first case the tables we
// will use and then later the schema that defines the nature of the tables
// and how they are related to each other, for example foreign key constraints


using timestamp_t   = boost::posix_time::ptime;
using data_t        = std::vector<uint8_t>;

struct user
{
    quince::serial                  id;                     // primary key
    std::string                     title;
    std::string                     forename;
    std::string                     surname;
    std::string                     user_role;
    boost::optional<std::string>    email_address;
};
QUINCE_MAP_CLASS(user, (id)(title)(forename)(surname)(user_role)(email_address))


struct userlogin
{
    std::string                     username;               // primary key
    quince::serial                  user_id;                // foreign key
    std::string                     password;
};
QUINCE_MAP_CLASS(userlogin, (username)(user_id)(password))


struct patient
{
    quince::serial                  id;                     // primary key
    std::string                     title;
    std::string                     forename;
    boost::optional<std::string>    middlename;
    std::string                     surname;
    boost::posix_time::ptime        date_of_birth;
    std::string                     postcode;
};
QUINCE_MAP_CLASS(patient, (id)(title)(forename)(middlename)(surname)(date_of_birth)(postcode))


struct patientwave
{
    quince::serial                  id;                     // primary key
    quince::serial                  patient_id;             // unique & foreign key
    std::string                     inhaler_type;           // unique
    std::string                     file_name;              // unique
    boost::posix_time::ptime        creation_timestamp;     // unique
    boost::posix_time::ptime        import_timestamp;
    std::vector<uint8_t>            wave_file;
    int                             file_size;
};
QUINCE_MAP_CLASS(patientwave, (id)(patient_id)(inhaler_type)(file_name)(creation_timestamp)(import_timestamp)(wave_file)(file_size))


struct wavelabelfile
{
    quince::serial                  patientwave_id;         // primary key & foreign key
    int                             label_number;           // primary key
    std::int64_t                    start_sample;
    std::int64_t                    end_sample;
    std::string                     event;
};
QUINCE_MAP_CLASS(wavelabelfile, (patientwave_id)(label_number)(start_sample)(end_sample)(event))


struct inhalerdata
{
    std::string                     inhaler_type;           // primary key
    std::string                     vocabulary;
};
QUINCE_MAP_CLASS(inhalerdata, (inhaler_type)(vocabulary))


template<class DatabaseT>
class schema
{
public:

    using database_t        = DatabaseT;

    using user_t            = user;
    using patient_t         = patient;
    using userlogin_t       = userlogin;
    using patientwave_t     = patientwave;
    using wavelabelfile_t   = wavelabelfile;
    using inhalerdata_t     = inhalerdata;

    using users_t           = quince::serial_table<user>;
    using userlogins_t      = quince::table<userlogin>;
    using patients_t        = quince::serial_table<patient>;
    using patientwaves_t    = quince::serial_table<patientwave>;
    using wavelabelfiles_t  = quince::table<wavelabelfile>;
    using inhalersdata_t    = quince::table<inhalerdata>;

public:

    // forward all the arguments from Schema to the Database
    template<class... ArgsT>
    explicit schema( ArgsT&&... Args )                     // parameter pack
        : Database_         ( std::forward<ArgsT>(Args)... )   // takes type list and parameter pack
        , Initialised_      ( false )
        , Users_            ( Database_, "users",          &user::id  )
        , Userlogins_       ( Database_, "userlogins",     &userlogin::username )
        , Patients_         ( Database_, "patients",       &patient::id )
        , Patientwaves_     ( Database_, "patientwaves",   &patientwave::id)
        , Wavelabelfiles_   ( Database_, "wavelabelfiles" )
        , Inhalersdata_     ( Database_, "inhalersdata",   &inhalerdata::inhaler_type )
    {
        Userlogins_.specify_foreign( Userlogins_->user_id, Users_ );

        Patientwaves_.specify_unique
            ( Patientwaves_->patient_id,
              Patientwaves_->file_name,
              Patientwaves_->inhaler_type,
              Patientwaves_->creation_timestamp );

        Patientwaves_.specify_foreign( Patientwaves_->patient_id, Patients_ );

        Wavelabelfiles_.specify_key
            ( Wavelabelfiles_->patientwave_id,
              Wavelabelfiles_->label_number );

        Wavelabelfiles_.specify_foreign( Wavelabelfiles_->patientwave_id, Patientwaves_ );
    }


    void initialise_once()
    {
        if( !Initialised_ )
        {
            open_all_tables();
            initial_population();
            Initialised_ = true;
        }
    }

private:

    // Convenience functions

    void open_all_tables()
    {
        Users_.open();
        Userlogins_.open();
        Patients_.open();
        Patientwaves_.open();
        Wavelabelfiles_.open();
        Inhalersdata_.open();
    }

    // initial population of user data
    void initial_population()
    {
        if( Users_.empty() )       // only allows this to run on first install
        {
            const quince::serial
                sys_admin_id
                    = Users_.insert
                    ( { quince::serial(),
                        "The",
                        "System",
                        "Administrator",
                        "SystemAdministrator",
                        boost::optional<std::string>() } );

            Userlogins_.insert(
            {
                "admin",
                sys_admin_id,
                "admin" } );

            // TODO: remove after testing - default DataTechnician
            const quince::serial
                data_tech_id
                    = Users_.insert
                    ( { quince::serial(),
                        "Mr",
                        "Data",
                        "Technician",
                        "DataTechnician",
                        boost::optional<std::string>() } );

            Userlogins_.insert(
            {
                "datatech",
                data_tech_id,
                "datatech" } );

            // TODO: remove after testing - default DiagnosingDoctor
            const quince::serial
                diag_doc_id
                    = Users_.insert
                    ( { quince::serial(),
                        "Dr",
                        "Diagnosing",
                        "Doctor",
                        "DiagnosingDoctor",
                        boost::optional<std::string>()
                });

            Userlogins_.insert(
            {
                "diagdoc",
                diag_doc_id,
                "diagdoc" } );

        }

        if( Patients_.empty() )
        {
            // TODO: remove after testing - default patient
            Patients_.insert(
            {
                quince::serial(),
                "Mr",
                "Kieron",
                boost::optional<std::string>(),
                "Allsop",
                boost::posix_time::ptime
                    (   boost::gregorian::from_string( "1972-Oct-14" ),
                        boost::posix_time::time_duration( 0, 0, 0 )    ),
                "BT191YX" } );

            Patients_.insert(
            {
                quince::serial(),
                "Mrs",
                "Helen",
                boost::optional<std::string>(),
                "Allsop",
                boost::posix_time::ptime
                    (   boost::gregorian::from_string( "1974-Feb-01" ),
                        boost::posix_time::time_duration( 0, 0, 0 )    ),
                "BT191YX" } );

        }

        if( Inhalersdata_.empty() )
        {
            Inhalersdata_.insert(
            {
                "Accuhaler",
                "on prime inhale exhale off silence" } );
        }
    }

    void clear_all_tables()
    {
        Users_.remove();
        Userlogins_.remove();
        Patients_.remove();
        Patientwaves_.remove();
        Wavelabelfiles_.remove();
        Inhalersdata_.remove();
    }


public:

    // Accessor functions to allow querying and manipulation
    // of the objects (tables) in the schema

    const users_t& users() const
    {
        return Users_;
    }

    users_t& users()
    {
        return Users_;
    }

    const userlogins_t& userlogins() const
    {
        return Userlogins_;
    }

    userlogins_t& userlogins()
    {
        return Userlogins_;
    }

    const patients_t& patients() const
    {
        return Patients_;
    }

    patients_t& patients()
    {
        return Patients_;
    }

    const patientwaves_t& patientwaves() const
    {
        return Patientwaves_;
    }

    patientwaves_t& patientwaves()
    {
        return Patientwaves_;
    }

    const wavelabelfiles_t& wavelabelfiles() const
    {
        return Wavelabelfiles_;
    }

    wavelabelfiles_t& wavelabelfiles()
    {
        return Wavelabelfiles_;
    }

    const inhalersdata_t& inhalersdata() const
    {
        return Inhalersdata_;
    }

    inhalersdata_t& inhalersdata()
    {
        return Inhalersdata_;
    }

private:

    DatabaseT   Database_;
    bool        Initialised_;

    // Schema Objects
    quince::serial_table<user>          Users_;
    quince::table<userlogin>            Userlogins_;
    quince::serial_table<patient>       Patients_;
    quince::serial_table<patientwave>   Patientwaves_;
    quince::table<wavelabelfile>        Wavelabelfiles_;
    quince::table<inhalerdata>          Inhalersdata_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // data_model
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // DATA_MODEL_SCHEMA_HPP_INCLUDED
