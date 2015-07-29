#ifndef WAVE_FILE_H
#define WAVE_FILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>

class wave_file : public QFile
{
public:
    wave_file(QObject *parent = 0);

    using QFile::open;
    bool open(const QString &fileName);
    const QAudioFormat &fileFormat() const;
    qint64 headerLength() const;

private:
    bool readHeader();

private:
    QAudioFormat m_fileFormat;
    qint64 m_headerLength;
};

#endif // WAVE_FILE_H
