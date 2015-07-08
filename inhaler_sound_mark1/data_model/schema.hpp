// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef DATA_MODEL_SCHEMA_HPP_INCLUDED
#define DATA_MODEL_SCHEMA_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
// None 

// Quince Includes
#include <quince/quince.h>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace data_model {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
    
// We place our basic data model in here. In the first case the tables we
// will use and then later the schema that defines the nature of the tables
// and how they are related to each other, for example foreign key constraints
    
struct user
{
    quince::serial id;                  // primary key with automatically assigned values
    std::string    title;
    std::string    forename;
    std::string    surname;
    std::string    user_role;
    std::string    email_address;
};
QUINCE_MAP_CLASS(user, (id)(title)(forename)(surname)(user_role)(email_address))


struct userlogin
{
    std::string    username;            // primary key
    quince::serial user_id;             // foreign key
    std::string    password;
};
QUINCE_MAP_CLASS(userlogin, (username)(user_id)(password))


// useful for patients
// boost::posix_time::ptime date_of_birth;


// We encapsulate the nature and relationships within our data model in a schema
class schema
{
public:
 
    using users_t      = quince::serial_table<user>;
    using userlogins_t = quince::table<userlogin>;
    
public:
    
    // Making this a function template allows us to take any database backend
    // This way we can aribrarily instantiate the schema on different backends
    // or using the same backend but in different databases
    template<class DatabaseT>
    explicit schema( const DatabaseT& Database )
    : Users_      ( Database, "users",      &user::id  )
    , Userlogins_ ( Database, "userlogins", &userlogin::username )
    {
        Userlogins_.specify_foreign( Userlogins_->user_id, Users_ );
    }
  
public:
    
    // Convenience functions
    
    void open_all_tables()
    {
        Users_.open();
        Userlogins_.open();
    }

    // initial population of user data
    void initial_population()
    {
        if (Users_.empty())       // only allows this to run on first install
        {
        const quince::serial sys_admin_id = Users_.insert({quince::serial(),
              "The", "System", "Administrator", "SystemAdministrator", "non_set" });
        Userlogins_.insert({"admin", sys_admin_id, "admin"});

        //TODO remove after testing
        //default DataTechnician for testing
        const quince::serial data_tech_id = Users_.insert({quince::serial(),
              "A", "Data", "Technician", "DataTechnician", "non_set" });
        Userlogins_.insert({"datatech", data_tech_id, "datatech"});

        //TODO remove after testing
        // default DiagnosingDoctor for testing
        const quince::serial diag_doc_id = Users_.insert({quince::serial(),
              "A", "Diagnosing", "Doctor", "DiagnosingDoctor", "non_set" });
        Userlogins_.insert({"diagdoc", diag_doc_id, "diagdoc"});

        }
    }

    // validate user based on input data
    bool validate_user(std::string InputUsername, std::string InputPassword)
    {
        const quince::query<userlogin> Query = Userlogins_.where(Userlogins_->username==InputUsername);
        const auto User = Query.begin();

        if( User != Query.end() && InputPassword==User->password)
        {
            return true;
        }
        return false;
    }

    void clear_all_tables()
    {
        Users_.remove();
        Userlogins_.remove();
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

        
private:
    
    quince::serial_table<user> Users_;
    quince::table<userlogin> Userlogins_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // data_model
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // DATA_MODEL_SCHEMA_HPP_INCLUDED
