// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// qt::audio Includes
#include "qt/audio/audio_buffer.hpp"

// Standard Library Includes
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I

// T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T
#define BOOST_TEST_MODULE audio_buffer
#include <boost/test/included/unit_test.hpp>
// T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T


std::vector<std::uint8_t> Uint8TestData
    = { 255, 254, 129, 128, 127, 1, 0 };

std::vector<std::int16_t> Int16TestData
    = { 32'767, 256, 255, 1, 0, -1, -255, -256, -32'767, -32'768 };

std::vector<std::int32_t> Int32TestData
    = { 2'147'483'647, 65'536, 65'535, 32'768, 32'767, 256, 255, 1, 0, -1, -255, -256, -32'767, -32'768, -65'535, -65'536, -2'147'483'647, -2'147'483'648 };

std::vector<float>  FloatTestData
    = { 1.0, 0.99999, 0.5, 0.00001, 0.0, -0.00001, -0.5, -0.99999, -1.0 };


constexpr int colw()
{
    return 13;
}


void print_horizontal_line()
{
    std::string underline( colw()+1, '-' );
    std::cout << " ";
    for( int i=0; i<7; ++i )
    {
        std::cout << "+" << underline;
    }
    std::cout << "+" << std::endl;
}


void print_headings()
{
    std::cout
        << " |" << std::setw(colw()) << "Unscaled"
        << " |" << std::setw(colw()) << "u8-bit"
        << " |" << std::setw(colw()) << "8-bit"
        << " |" << std::setw(colw()) << "16-bit"
        << " |" << std::setw(colw()) << "32-bit"
        << " |" << std::setw(colw()) << "float"
        << " |" << std::setw(colw()) << "double"
        << " |" << std::endl;

    print_horizontal_line();

    std::cout << std::fixed << std::setprecision(colw()-4);
}


void print_scalings( const qt::audio::audio_buffer& Buffer, std::size_t s, std::size_t c )
{
    std::cout
        << " |" << std::setw(colw()) << static_cast<unsigned>( Buffer.scaled_sample<std::uint8_t>( s, c ) )
        << " |" << std::setw(colw()) << static_cast<int>( Buffer.scaled_sample<std::int8_t>( s, c ) )
        << " |" << std::setw(colw()) << Buffer.scaled_sample<std::int16_t>( s, c )
        << " |" << std::setw(colw()) << Buffer.scaled_sample<std::int32_t>( s, c )
        << " |" << std::setw(colw()) << Buffer.scaled_sample<float>( s, c )
        << " |" << std::setw(colw()) << Buffer.scaled_sample<double>( s, c )
        << " |" << std::endl;
}


BOOST_AUTO_TEST_CASE( scaled_sample_uint8_test )
{
    auto Format = qt::audio::format( qt::audio::sample_type_t::unsigned_int8, 8000, 1, "audio/pcm" );

    auto Buffer = qt::audio::audio_buffer( Format, &Uint8TestData[0], Uint8TestData.size()*sizeof(std::uint8_t) );

    std::cout << "\nTesting scaled unsigned 8-bit values" << std::endl;

    print_horizontal_line();
    print_headings();

    for( unsigned s = 0; s < Buffer.samples_per_channel(); ++s )
    {
        for( unsigned c = 0; c < Buffer.format().channel_count(); ++c )
        {
            std::cout << " |" << std::setw(colw()) << static_cast<unsigned>( Buffer.sample_cast<uint8_t>( s, c ) );

            print_scalings( Buffer, s, c );
        }
    }
    print_horizontal_line();
}


BOOST_AUTO_TEST_CASE( scaled_sample_int16_test )
{
    auto Format = qt::audio::format( qt::audio::sample_type_t::signed_int16, 44100, 1, "audio/pcm" );

    auto Buffer = qt::audio::audio_buffer( Format, &Int16TestData[0], Int16TestData.size()*sizeof(std::int16_t) );

    std::cout << "\nTesting scaled signed 16-bit values" << std::endl;

    print_horizontal_line();
    print_headings();

    for( unsigned s = 0; s < Buffer.samples_per_channel(); ++s )
    {
        for( unsigned c = 0; c < Buffer.format().channel_count(); ++c )
        {
            std::cout << " |" << std::setw(colw()) << Buffer.sample_cast<int16_t>( s, c );

            print_scalings( Buffer, s, c );
        }
    }
    print_horizontal_line();
}


BOOST_AUTO_TEST_CASE( scaled_sample_int32_test )
{
    auto Format = qt::audio::format( qt::audio::sample_type_t::signed_int32, 44100, 1, "audio/pcm" );

    auto Buffer = qt::audio::audio_buffer( Format, &Int32TestData[0], Int32TestData.size()*sizeof(std::int32_t) );

    std::cout << "\nTesting scaled signed 32-bit values" << std::endl;

    print_horizontal_line();
    print_headings();

    for( unsigned s = 0; s < Buffer.samples_per_channel(); ++s )
    {
        for( unsigned c = 0; c < Buffer.format().channel_count(); ++c )
        {
            std::cout << " |" << std::setw(colw()) << Buffer.sample_cast<int32_t>( s, c );

            print_scalings( Buffer, s, c );
        }
    }
    print_horizontal_line();
}


BOOST_AUTO_TEST_CASE( scaled_sample_float_test )
{
    auto Format = qt::audio::format( qt::audio::sample_type_t::floating_point, 44100, 1, "audio/pcm" );

    auto Buffer = qt::audio::audio_buffer( Format, &FloatTestData[0], FloatTestData.size()*sizeof(float) );

    std::cout << "\nTesting scaled float values" << std::endl;

    print_horizontal_line();
    print_headings();

    for( unsigned s = 0; s < Buffer.samples_per_channel(); ++s )
    {
        for( unsigned c = 0; c < Buffer.format().channel_count(); ++c )
        {
            std::cout << " |" << std::setw(colw()) << Buffer.sample_cast<float>( s, c );

            print_scalings( Buffer, s, c );
        }
    }
    print_horizontal_line();
}

