// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_AUDIO_VOCABULARY_KIND_HPP_INCLUDED
#define QT_AUDIO_VOCABULARY_KIND_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// None

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt {
namespace audio {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

enum class vocabulary_kind
{
    silence,
    word
};


//! \class  vocabulary_kind.hpp
//! \author Kieron Allsop
//!
//! \brief  To define the "type" of vocabulary for use in labelling
//!
class labelled_vocabulary
{

public:

    labelled_vocabulary( const std::string& Label, std::size_t Start, std::size_t End )
    : Kind_( vocabulary_kind::word )
    , Label_( Label )
    , Start_( Start )
    , End_( End )
    {
    }

    labelled_vocabulary( std::size_t Start, std::size_t End )
    : Kind_( vocabulary_kind::silence )
    , Start_( Start )
    {
    }
    // Observers here

    const std::string& label_name() const
    {
        return Label_;
    }

    const std::size_t& label_start() const
    {
        return Start_;
    }

    const std::size_t& label_end() const
    {
        return End_;
    }

private:

    vocabulary_kind Kind_;
    std::string Label_;
    std::size_t Start_;
    std::size_t End_;

};


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // audio
} // qt
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_AUDIO_VOCABULARY_KIND_HPP_INCLUDED
