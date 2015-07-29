// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
G G G
#ifndef INHALER_PATIENT_RETIEVER_HPP_INCLUDED
#define INHALER_PATIENT_RETIEVER_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
I I I

// Standard Library Includes
#include <vector>
#include <string>
#include <functional>
#include <fstream>

// Boost Includes
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

// Inhaler Includes
#include "inhaler/server.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
n n n


class patient_retriever
{
public:

     // Type Interface
     using shared_schema_t       = inhaler::server::shared_schema_t;
     using date_t                = boost::posix_time::ptime;
     using timestamp_t           = boost::posix_time::ptime;
     using patient_t             = data_model::patient;
     using optional_patient_t    = boost::optional<patient_t>;

public:

     // Construct with a shared Schema
     explicit patient_retriever( const shared_schema_t& Schema )
     : Schema_( Schema )
     {
     }

     const optional_patient_t& get_patient
         (   const std::string&  Forename,
             const std::string&  Surname,
             const date_t&       DateOfBirth,
             const std::string&  Postcode )
     {
         const auto& Patients = Schema_->patients();

         const auto Query
                     = Patients
                         .where(    Patients->forename      == Forename
                                 && Patients->surname       == Surname
                                 && Patients->date_of_birth == DateOfBirth
                                 && Patients->postcode      == Postcode );

         auto Patient = Query.begin();

         if( Patient == Query.end())
         {
             return optional_patient_t();
         }
         return *Patient;
     }

private:

     shared_schema_t         Schema_;
};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
n n n


// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
G G G
#endif // INHALER_PATIENT_RETIEVER_HPP_INCLUDED