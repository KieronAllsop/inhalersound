#ifndef UTILS_H
#define UTILS_H

#include <QtCore/qglobal.h>
#include <QDebug>

QT_FORWARD_DECLARE_CLASS(QAudioFormat)

//-----------------------------------------------------------------------------
// Miscellaneous utility functions
//-----------------------------------------------------------------------------

qint64 audioDuration(const QAudioFormat &format, qint64 bytes);
qint64 audioLength(const QAudioFormat &format, qint64 microSeconds);

QString formatToString(const QAudioFormat &format);

qreal nyquistFrequency(const QAudioFormat &format);

// Scale PCM value to [-1.0, 1.0]
qreal pcmToReal(qint16 pcm);

// Scale real value in [-1.0, 1.0] to PCM
qint16 realToPcm(qreal real);

// Check whether the audio format is PCM
bool isPCM(const QAudioFormat &format);

// Check whether the audio format is signed, little-endian, 16-bit PCM
bool isPCMS16LE(const QAudioFormat &format);

// Compile-time calculation of powers of two

template<int N> class PowerOfTwo
{ public: static const int Result = PowerOfTwo<N-1>::Result * 2; };

template<> class PowerOfTwo<0>
{ public: static const int Result = 1; };


//-----------------------------------------------------------------------------
// Debug output
//-----------------------------------------------------------------------------

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator<<(const T&) { return *this; }
};

inline NullDebug nullDebug() { return NullDebug(); }

#ifdef LOG_SHOW_FORM
#   define SHOW_FORM_DEBUG qDebug()
#else
#   define SHOW_FORM_DEBUG nullDebug()
#endif

#endif // UTILS_H
