#include "show_form.h"
#include "utils.h"
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

//#define PAINT_EVENT_TRACE
#ifdef PAINT_EVENT_TRACE
#   define SHOW_FORM_PAINT_DEBUG qDebug()
#else
#   define SHOW_FORM_PAINT_DEBUG nullDebug()
#endif

show_form::show_form(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferPosition(0)
    ,   m_bufferLength(0)
    ,   m_audioPosition(0)
    ,   m_active(false)
    ,   m_tileLength(0)
    ,   m_tileArrayStart(0)
    ,   m_windowPosition(0)
    ,   m_windowLength(0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(50);
}

show_form::~show_form()
{
    deletePixmaps();
}

void show_form::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    if (m_active) {
        SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent"
                             << "windowPosition" << m_windowPosition
                             << "windowLength" << m_windowLength;
        qint64 pos = m_windowPosition;
        const qint64 windowEnd = m_windowPosition + m_windowLength;
        int destLeft = 0;
        int destRight = 0;
        while (pos < windowEnd) {
            const TilePoint point = tilePoint(pos);
            SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "pos" << pos
                                 << "tileIndex" << point.index
                                 << "positionOffset" << point.positionOffset
                                 << "pixelOffset" << point.pixelOffset;

            if (point.index != NullIndex) {
                const Tile &tile = m_tiles[point.index];
                if (tile.painted) {
                    const qint64 sectionLength = qMin((m_tileLength - point.positionOffset),
                                                     (windowEnd - pos));
                    Q_ASSERT(sectionLength > 0);

                    const int sourceRight = tilePixelOffset(point.positionOffset + sectionLength);
                    destRight = windowPixelOffset(pos - m_windowPosition + sectionLength);

                    QRect destRect = rect();
                    destRect.setLeft(destLeft);
                    destRect.setRight(destRight);

                    QRect sourceRect(QPoint(), m_pixmapSize);
                    sourceRect.setLeft(point.pixelOffset);
                    sourceRect.setRight(sourceRight);

                    SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "tileIndex" << point.index
                                         << "source" << point.pixelOffset << sourceRight
                                         << "dest" << destLeft << destRight;

                    painter.drawPixmap(destRect, *tile.pixmap, sourceRect);

                    destLeft = destRight;

                    if (point.index < m_tiles.count()) {
                        pos = tilePosition(point.index + 1);
                        SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "pos ->" << pos;
                    } else {
                        // Reached end of tile array
                        SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "reached end of tile array";
                        break;
                    }
                } else {
                    // Passed last tile which is painted
                    SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "tile" << point.index << "not painted";
                    break;
                }
            } else {
                // pos is past end of tile array
                SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "pos" << pos << "past end of tile array";
                break;
            }
        }

        SHOW_FORM_PAINT_DEBUG << "Waveform::paintEvent" << "final pos" << pos << "final x" << destRight;
    }
}

void show_form::resizeEvent(QResizeEvent *event)
{
    if (event->size() != event->oldSize())
        createPixmaps(event->size());
}

void show_form::initialize(const QAudioFormat &format, qint64 audioBufferSize, qint64 windowDurationUs)
{
    SHOW_FORM_DEBUG << "Waveform::initialize"
                   << "audioBufferSize" << audioBufferSize
                   << "windowDurationUs" << windowDurationUs;

    reset();

    m_format = format;

    // Calculate tile size
    m_tileLength = audioBufferSize;

    // Calculate window size
    m_windowLength = audioLength(m_format, windowDurationUs);

    // Calculate number of tiles required
    int nTiles;
    if (m_tileLength > m_windowLength) {
        nTiles = 2;
    } else {
        nTiles = m_windowLength / m_tileLength + 1;
        if (m_windowLength % m_tileLength)
            ++nTiles;
    }

    SHOW_FORM_DEBUG << "Waveform::initialize"
                   << "tileLength" << m_tileLength
                   << "windowLength" << m_windowLength
                   << "nTiles" << nTiles;

    m_pixmaps.fill(0, nTiles);
    m_tiles.resize(nTiles);

    createPixmaps(rect().size());

    m_active = true;
}

void show_form::reset()
{
    SHOW_FORM_DEBUG << "Waveform::reset";

    m_bufferPosition = 0;
    m_buffer = QByteArray();
    m_audioPosition = 0;
    m_format = QAudioFormat();
    m_active = false;
    deletePixmaps();
    m_tiles.clear();
    m_tileLength = 0;
    m_tileArrayStart = 0;
    m_windowPosition = 0;
    m_windowLength = 0;
}

void show_form::bufferChanged(qint64 position, qint64 length, const QByteArray &buffer)
{
    SHOW_FORM_DEBUG << "Waveform::bufferChanged"
                   << "audioPosition" << m_audioPosition
                   << "bufferPosition" << position
                   << "bufferLength" << length;
    m_bufferPosition = position;
    m_bufferLength = length;
    m_buffer = buffer;
    paintTiles();
}

void show_form::audioPositionChanged(qint64 position)
{
    SHOW_FORM_DEBUG << "Waveform::audioPositionChanged"
                   << "audioPosition" << position
                   << "bufferPosition" << m_bufferPosition
                   << "bufferLength" << m_bufferLength;

    if (position >= m_bufferPosition) {
        if (position + m_windowLength > m_bufferPosition + m_bufferLength)
            position = qMax(qint64(0), m_bufferPosition + m_bufferLength - m_windowLength);
        m_audioPosition = position;
        setWindowPosition(position);
    }
}

void show_form::deletePixmaps()
{
    QPixmap *pixmap;
    foreach (pixmap, m_pixmaps)
        delete pixmap;
    m_pixmaps.clear();
}

void show_form::createPixmaps(const QSize &widgetSize)
{
    m_pixmapSize = widgetSize;
    m_pixmapSize.setWidth(qreal(widgetSize.width()) * m_tileLength / m_windowLength);

    SHOW_FORM_DEBUG << "Waveform::createPixmaps"
                   << "widgetSize" << widgetSize
                   << "pixmapSize" << m_pixmapSize;

    Q_ASSERT(m_tiles.count() == m_pixmaps.count());

    // (Re)create pixmaps
    for (int i=0; i<m_pixmaps.size(); ++i) {
        delete m_pixmaps[i];
        m_pixmaps[i] = 0;
        m_pixmaps[i] = new QPixmap(m_pixmapSize);
    }

    // Update tile pixmap pointers, and mark for repainting
    for (int i=0; i<m_tiles.count(); ++i) {
        m_tiles[i].pixmap = m_pixmaps[i];
        m_tiles[i].painted = false;
    }
}

void show_form::setWindowPosition(qint64 position)
{
    SHOW_FORM_DEBUG << "Waveform::setWindowPosition"
                   << "old" << m_windowPosition << "new" << position
                   << "tileArrayStart" << m_tileArrayStart;

    const qint64 oldPosition = m_windowPosition;
    m_windowPosition = position;

    if ((m_windowPosition >= oldPosition) &&
        (m_windowPosition - m_tileArrayStart < (m_tiles.count() * m_tileLength))) {
        // Work out how many tiles need to be shuffled
        const qint64 offset = m_windowPosition - m_tileArrayStart;
        const int nTiles = offset / m_tileLength;
        shuffleTiles(nTiles);
    } else {
        resetTiles(m_windowPosition);
    }

    if (!paintTiles() && m_windowPosition != oldPosition)
        update();
}

qint64 show_form::tilePosition(int index) const
{
    return m_tileArrayStart + index * m_tileLength;
}

show_form::TilePoint show_form::tilePoint(qint64 position) const
{
    TilePoint result;
    if (position >= m_tileArrayStart) {
        const qint64 tileArrayEnd = m_tileArrayStart + m_tiles.count() * m_tileLength;
        if (position < tileArrayEnd) {
            const qint64 offsetIntoTileArray = position - m_tileArrayStart;
            result.index = offsetIntoTileArray / m_tileLength;
            Q_ASSERT(result.index >= 0 && result.index <= m_tiles.count());
            result.positionOffset = offsetIntoTileArray % m_tileLength;
            result.pixelOffset = tilePixelOffset(result.positionOffset);
            Q_ASSERT(result.pixelOffset >= 0 && result.pixelOffset <= m_pixmapSize.width());
        }
    }

    return result;
}

int show_form::tilePixelOffset(qint64 positionOffset) const
{
    Q_ASSERT(positionOffset >= 0 && positionOffset <= m_tileLength);
    const int result = (qreal(positionOffset) / m_tileLength) * m_pixmapSize.width();
    return result;
}

int show_form::windowPixelOffset(qint64 positionOffset) const
{
    Q_ASSERT(positionOffset >= 0 && positionOffset <= m_windowLength);
    const int result = (qreal(positionOffset) / m_windowLength) * rect().width();
    return result;
}

bool show_form::paintTiles()
{
    SHOW_FORM_DEBUG << "Waveform::paintTiles";
    bool updateRequired = false;

    for (int i=0; i<m_tiles.count(); ++i) {
        const Tile &tile = m_tiles[i];
        if (!tile.painted) {
            const qint64 tileStart = m_tileArrayStart + i * m_tileLength;
            const qint64 tileEnd = tileStart + m_tileLength;
            if (m_bufferPosition <= tileStart && m_bufferPosition + m_bufferLength >= tileEnd) {
                paintTile(i);
                updateRequired = true;
            }
        }
    }

    if (updateRequired)
        update();

    return updateRequired;
}

void show_form::paintTile(int index)
{
    const qint64 tileStart = m_tileArrayStart + index * m_tileLength;

    SHOW_FORM_DEBUG << "Waveform::paintTile"
                   << "index" << index
                   << "bufferPosition" << m_bufferPosition
                   << "bufferLength" << m_bufferLength
                   << "start" << tileStart
                   << "end" << tileStart + m_tileLength;

    Q_ASSERT(m_bufferPosition <= tileStart);
    Q_ASSERT(m_bufferPosition + m_bufferLength >= tileStart + m_tileLength);

    Tile &tile = m_tiles[index];
    Q_ASSERT(!tile.painted);

    const qint16* base = reinterpret_cast<const qint16*>(m_buffer.constData());
    const qint16* buffer = base + ((tileStart - m_bufferPosition) / 2);
    const int numSamples = m_tileLength / (2 * m_format.channelCount());

    QPainter painter(tile.pixmap);

    painter.fillRect(tile.pixmap->rect(), Qt::black);

    QPen pen(Qt::white);
    painter.setPen(pen);

    // Calculate initial PCM value
    qint16 previousPcmValue = 0;
    if (buffer > base)
        previousPcmValue = *(buffer - m_format.channelCount());

    // Calculate initial point
    const qreal previousRealValue = pcmToReal(previousPcmValue);
    const int originY = ((previousRealValue + 1.0) / 2) * m_pixmapSize.height();
    const QPoint origin(0, originY);

    QLine line(origin, origin);

    for (int i=0; i<numSamples; ++i) {
        const qint16* ptr = buffer + i * m_format.channelCount();

        const int offset = reinterpret_cast<const char*>(ptr) - m_buffer.constData();
        Q_ASSERT(offset >= 0);
        Q_ASSERT(offset < m_bufferLength);
        Q_UNUSED(offset);

        const qint16 pcmValue = *ptr;
        const qreal realValue = pcmToReal(pcmValue);

        const int x = tilePixelOffset(i * 2 * m_format.channelCount());
        const int y = ((realValue + 1.0) / 2) * m_pixmapSize.height();

        line.setP2(QPoint(x, y));
        painter.drawLine(line);
        line.setP1(line.p2());
    }

    tile.painted = true;
}

void show_form::shuffleTiles(int n)
{
    SHOW_FORM_DEBUG << "Waveform::shuffleTiles" << "n" << n;

    while (n--) {
        Tile tile = m_tiles.first();
        tile.painted = false;
        m_tiles.erase(m_tiles.begin());
        m_tiles += tile;
        m_tileArrayStart += m_tileLength;
    }

    SHOW_FORM_DEBUG << "Waveform::shuffleTiles" << "tileArrayStart" << m_tileArrayStart;
}

void show_form::resetTiles(qint64 newStartPos)
{
    SHOW_FORM_DEBUG << "Waveform::resetTiles" << "newStartPos" << newStartPos;

    QVector<Tile>::iterator i = m_tiles.begin();
    for ( ; i != m_tiles.end(); ++i)
        i->painted = false;

    m_tileArrayStart = newStartPos;
}

