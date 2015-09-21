// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef INHALER_LABEL_FILE_EDITOR_HPP_INCLUDED
#define INHALER_LABEL_FILE_EDITOR_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// Inhaler Includes
#include "inhaler/server.hpp"
#include "inhaler/data_retriever.hpp"
#include "inhaler/patient_wave_details.hpp"

// qt/audio Includes
#include "qt/audio/vocabulary_kind.hpp"

// Quince Includes
#include <quince/quince.h>

// Standard Library Includes
#include <tuple>
#include <string>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace inhaler {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


//! \class  label_file_editor.hpp
//! \author Kieron Allsop
//!
//! \brief  To interface between the view with which the user interacts when
//!         they want to label a WAV file and the database which store the
//!         data
//!
class label_file_editor
{
public:

    using shared_schema_t           = inhaler::server::shared_schema_t;
    using patient_t                 = shared_schema_t::element_type::patient_t;
    using data_retriever_t          = inhaler::data_retriever;
    using shared_data_retriever_t   = std::shared_ptr<inhaler::data_retriever>;
    using patient_wave_details_t    = data_retriever_t::patient_wave_details_t;
    using result_t                  = std::tuple<int, std::int64_t, std::int64_t, std::string>;
    using vocabulary_t              = qt::audio::labelled_vocabulary;
    using wave_labels_t             = std::vector<vocabulary_t>;


public:

    explicit label_file_editor( const shared_schema_t& Schema,
                                const patient_t& Patient,
                                const patient_wave_details_t& CurrentWave )
    : Schema_       ( Schema )
    , Patient_      ( Patient )
    , CurrentWave_  ( CurrentWave )
    {
    }


    quince::serial get_wave_id()
    {
        const auto& PatientWaves = Schema_->patientwaves();

        const quince::query< quince::serial >
                Query
                    = PatientWaves
                        .where
                            (       PatientWaves->patient_id         == Patient_.id
                                &&  PatientWaves->inhaler_type       == CurrentWave_.inhaler_model()
                                &&  PatientWaves->file_name          == CurrentWave_.name()
                                &&  PatientWaves->creation_timestamp == CurrentWave_.modified_time()  )
                        .select
                            (   PatientWaves->id   );

       auto WaveID = Query.begin();
       return *WaveID;
    }


    wave_labels_t retrieve_wave_labels()
    {
        const auto& PatientWaves = Schema_->patientwaves();
        const auto& WaveLabelFiles = Schema_->wavelabelfiles();
        const quince::query< result_t >
                Query
                    = PatientWaves
                        .inner_join
                            ( WaveLabelFiles,
                                PatientWaves->id == WaveLabelFiles->patientwave_id )
                        .where
                            (       PatientWaves->patient_id         == Patient_.id
                                &&  PatientWaves->inhaler_type       == CurrentWave_.inhaler_model()
                                &&  PatientWaves->file_name          == CurrentWave_.name()
                                &&  PatientWaves->creation_timestamp == CurrentWave_.modified_time()  )
                        .select
                            (   WaveLabelFiles->label_number,
                                WaveLabelFiles->start_sample,
                                WaveLabelFiles->end_sample,
                                WaveLabelFiles->event   );

        for( const auto& LabelRowTuple: Query )
        {
            auto StartSample    = std::get<1>(LabelRowTuple);
            auto EndSample      = std::get<2>(LabelRowTuple);
            auto Label          = std::get<3>(LabelRowTuple);

            auto Item = qt::audio::labelled_vocabulary( Label, StartSample, EndSample );
            LabelData_.emplace_back( Item );
        }
        return LabelData_;
    }


    void add_wave_labels( const wave_labels_t& Labels )
    {
        auto WaveID = get_wave_id();
        int Element = 0;

        for( const auto& Label: Labels )
        {
            Schema_->wavelabelfiles()
                .insert
                    ( { WaveID,
                        Element,
                        Label.label_start(),
                        Label.label_end(),
                        Label.label_name() } );
            ++Element;
        }
    }


    void delete_all_wave_labels()
    {
        auto WaveID = get_wave_id();
        const auto& WaveLabelFiles = Schema_->wavelabelfiles();

        Schema_->wavelabelfiles()
            .where
                ( WaveLabelFiles->patientwave_id == WaveID )
            .remove();
    }

private:

    shared_schema_t         Schema_;
    patient_t               Patient_;
    wave_labels_t           LabelData_;
    patient_wave_details_t  CurrentWave_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // inhaler
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // INHALER_LABEL_FILE_EDITOR_HPP_INCLUDED
