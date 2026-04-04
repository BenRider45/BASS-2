#include "wavfile.h"
#include <QDir>
#include <QString>
#include <vector>
class WavSurfer {
public:
  WavSurfer(QDir parentProjectDir) : m_parentProjectDir(parentProjectDir) {};

  std::vector<WavFile<uint32_t>> RetrieveWavFiles();

  void ImportWavFilesIntoProject(QString fileDirectory);

  void UpdateWavMetaData();

private:
  QDir m_parentProjectDir;
};
