#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
{
}

bool AudioPlayer::isPlaying() const { return m_isPlaying; }
int AudioPlayer::position() const { return m_position; }

void AudioPlayer::playAll()
{
    m_isPlaying = true;
    emit isPlayingChanged();
}

void AudioPlayer::playSelected(int startFrame, int endFrame)
{
    Q_UNUSED(startFrame);
    Q_UNUSED(endFrame);
    m_isPlaying = true;
    emit isPlayingChanged();
}

void AudioPlayer::playFromCursor(int frame)
{
    Q_UNUSED(frame);
    m_isPlaying = true;
    emit isPlayingChanged();
}

void AudioPlayer::stop()
{
    m_isPlaying = false;
    emit isPlayingChanged();
}
