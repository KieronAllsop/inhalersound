// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef DATA_MODEL_SCHEMA_HPP_INCLUDED
#define DATA_MODEL_SCHEMA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <vector>

// Custom Includes
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/optional.hpp>

// Quince Includes
#include <quince/quince.h>
// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace data_model{
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// We place our basic data model in here. In the first case the tables we
// will use and then later the schema that defines the nature of the tables
// and how they are related to each other, for example foreign key constraints


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
    quince::serial                  patient_id;             // primary key & foreign key
    std::string                     inhaler_type;           // primary key
    std::string                     file_name;              // primary key
    boost::posix_time::ptime        creation_timestamp;     // primary key
    boost::posix_time::ptime        import_timestamp;
    std::vector<uint8_t>            wave_file;
    int                             file_size;
};
QUINCE_MAP_CLASS(patientwave, (patient_id)(inhaler_type)(file_name)(creation_timestamp)(import_timestamp)(wave_file)(file_size))


// We encapsulate the nature and relationships within our data model in a schema
class schema
{
public:

    using users_t         = quince::serial_table<user>;
    using userlogins_t    = quince::table<userlogin>;
    using patients_t      = quince::serial_table<patient>;
    using patientwaves_t  = quince::table<patientwave>;

public:

    // Making this a function template allows us to take any database backend
    // This way we can aribrarily instantiate the schema on different backends
    // or using the same backend but in different databases
    template<class DatabaseT>
    explicit schema( const DatabaseT& Database )
        : Users_        ( Database, "users",          &user::id  )
        , Userlogins_   ( Database, "userlogins",     &userlogin::username )
        , Patients_     ( Database, "patients",       &patient::id )
        , Patientwaves_ ( Database, "patientwaves" )
    {
        Userlogins_.specify_foreign( Userlogins_  ->user_id, Users_ );

        Patientwaves_.specify_key
            ( Patientwaves_->patient_id,
              Patientwaves_->file_name,
              Patientwaves_->inhaler_type,
              Patientwaves_->creation_timestamp );

        Patientwaves_.specify_foreign( Patientwaves_->patient_id, Patients_ );
    }

public:

    // Convenience functions

    void open_all_tables( bool& Connected )
    {
        Users_.open();
        Connected = true;
        Userlogins_.open();
        Patients_.open();
        Patientwaves_.open();
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
        }
    }


    // TODO: move this function into its own class
    boost::optional<quince::serial> get_patient_id(
        const std::string& Forename,
        const std::string& Surname,
        const std::string& DateOfBirth,
        const std::string& Postcode )
    {
        auto Timestamp
            = boost::posix_time::ptime
                (   boost::gregorian::from_string( DateOfBirth ),
                    boost::posix_time::time_duration( 0, 0, 0 )    );

        const quince::query<patient>
            PatientQuery
                = Patients_
                    .where( Patients_->forename==Forename &&
                            Patients_->surname==Surname &&
                            Patients_->date_of_birth==Timestamp &&
                            Patients_->postcode==Postcode );

        const auto Patient = PatientQuery.begin();

        if( Patient != PatientQuery.end())
        {
            return boost::optional<quince::serial>( Patient->id );
        }
        return boost::optional<quince::serial>();       // allows for a zero return
    }


    // TODO: move this function into its own class
    void insert_wave(
        const quince::serial& PatientID,
        const std::string& Inhaler,
        const std::string& Filename,
        const boost::posix_time::ptime& Modified,
        const std::vector<uint8_t>& WaveFile,
        const int FileSize )
    {
        auto Timestamp
            = boost::posix_time::microsec_clock::local_time();

        Patientwaves_.insert(
        {
            PatientID,
            Inhaler,
            Filename,
            Modified,
            Timestamp,
            WaveFile,
            FileSize } );
    }


    // TODO: move this function into its own class
    boost::optional<user> validate_user(
        bool& ValidUser,
        std::string& UserRole,
        const std::string& Username,
        const std::string& Password )
    {
        const quince::query<userlogin>
            LoginQuery
                = Userlogins_
                    .where( Userlogins_->username==Username );

        const auto Login = LoginQuery.begin();

        if( Login != LoginQuery.end() &&
                Password == Login->password )
        {
            ValidUser = true;                               // confirms valid user

            const quince::query<user>
                UserQuery
                    = Users_
                        .where( Users_->id == Login->user_id );

            auto User = UserQuery.begin();

            if( User != UserQuery.end() )
            {
                UserRole = User->user_role;                 // set user role
                return boost::optional<user>( *User );      // return all user details
            }
        }
        return boost::optional<user>();
    }


    void clear_all_tables()
    {
        Users_.remove();
        Userlogins_.remove();
        Patients_.remove();
        Patientwaves_.remove();
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


private:

    quince::serial_table<user> Users_;
    quince::table<userlogin> Userlogins_;
    quince::serial_table<patient> Patients_;
    quince::table<patientwave> Patientwaves_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // data_model
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // DATA_MODEL_SCHEMA_HPP_INCLUDED
