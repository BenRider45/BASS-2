#include <QDir>
#include <QString>

class WavSurfer {
public:
  WavSurfer(QDir parentProjectDir) : m_parentProjectDir(parentProjectDir) {};

private:
  QDir m_parentProjectDir;
};
