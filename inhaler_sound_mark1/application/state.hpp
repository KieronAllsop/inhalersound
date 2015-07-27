// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef APPLICATION_STATE_HPP_INCLUDED
#define APPLICATION_STATE_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Standard Library Includes
// None

// Boost Library Includes
#include <boost/signals2/signal.hpp>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace application {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

enum class state
{
    login_as_data_technician,
    login_as_diagnosing_doctor,
    login_as_system_administrator,
    get_patient,
    explore_patient
};


constexpr const char* c_str( state& Value )
{
    switch( Value )
    {
        case state::login_as_data_technician        : return "login_as_data_technician";
        case state::login_as_diagnosing_doctor      : return "login_as_diagnosing_doctor";
        case state::login_as_system_administrator   : return "login_as_system_administrator";
        case state::get_patient                     : return "get_patient";
        case state::explore_patient                 : return "explore_patient";
    }
    return nullptr;
}


using signal_state_complete = boost::signals2::signal< void ( const state& ) >;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // application
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // APPLICATION_STATE_HPP_INCLUDED
