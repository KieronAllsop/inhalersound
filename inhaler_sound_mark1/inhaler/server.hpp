// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_SERVER_HPP_INCLUDED
#define INHALER_SERVER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
#include <string>
#include <stdexcept>

// Boost Includes
#include <boost/optional.hpp>
#include <boost/exception/all.hpp>

// Quince Includes
#include <quince/quince.h>
#include <quince_postgresql/database.h>

// Data Model Includes
#include "data_model/schema.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


struct could_not_connect_to_server : virtual boost::exception, virtual std::exception {};


class server
{
public:

    // Type Interface
    using shared_schema_t = std::shared_ptr< data_model::schema<quince_postgresql::database> >;
    using optional_user_t = boost::optional<data_model::user>;

public:

    // Creation --------------------------------------------------------------

    // Make this class non-copyable
    server( const server& other ) = delete;
    server& operator=( const server& other ) = delete;

    server()
    : Schema_
        ( std::make_shared< data_model::schema<quince_postgresql::database> >
            (   "localhost",         // host
                "inhaler",           // user
                "inhaler",           // password
                "inhalersound",      // database
                "inhalersound"  ) )  // schema
    {
    }

    // Operations ------------------------------------------------------------

    shared_schema_t connect_to_schema()
    {
        try
        {
            Schema_->initialise();
            return Schema_;
        }
        catch( const quince::failed_connection_exception& Error )
        {
            BOOST_THROW_EXCEPTION( could_not_connect_to_server() );
        }
    }

    // rename validate user
    optional_user_t authenticate
        (   const std::string& Username,
            const std::string& Password   )
    {
        const auto& UserLogins = Schema_->userlogins();

        const auto LoginQuery
                    = UserLogins
                        .where( Username == UserLogins->username );

        auto Login = LoginQuery.begin();

        if(     Login != LoginQuery.end()
            &&  Password == Login->password )
        {
            const auto& Users = Schema_->users();

            const auto UserQuery
                        = Users
                            .where( Login->user_id == Users->id );

            auto User = UserQuery.begin();

            if( User != UserQuery.end() )
            {
                return optional_user_t( *User );
            }
        }
        return optional_user_t();
    }

private:

    shared_schema_t Schema_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_SERVER_HPP_INCLUDED
