#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QQmlEngine>

class AudioPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int position READ position NOTIFY playbackPositionChanged)

public:
    explicit AudioPlayer(QObject *parent = nullptr);

    bool isPlaying() const;
    int position() const;

    Q_INVOKABLE void playAll();
    Q_INVOKABLE void playSelected(int startFrame, int endFrame);
    Q_INVOKABLE void playFromCursor(int frame);
    Q_INVOKABLE void stop();

signals:
    void isPlayingChanged();
    void playbackPositionChanged(int frame);

private:
    bool m_isPlaying = false;
    int m_position = 0;
};

#endif // AUDIOPLAYER_H
