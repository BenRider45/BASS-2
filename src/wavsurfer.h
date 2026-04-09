#pragma once
#include "sharedconstants.h"
#include "wavfile.h"
#include <QDir>
#include <QString>
template <wavfileUtils::Numeric T> class WavSurfer {
public:
  QVector<WavFile<T>> RetrieveWavFiles(QString projectDirectoryPath) {

    QString pathToWavDir = getProjectAudioDirectoryPath(projectDirectoryPath);
    QDir WavDir(pathToWavDir);
    QStringList lst;
    lst << "*" + QString(".wav");
    WavDir.setNameFilters(lst);
    QStringList entryLst = WavDir.entryList();

    QVector<WavFile<T>> outputLst;
    for (auto entry : entryLst) {
      outputLst.append(WavFile<T>(WavDir.filePath(entry).toStdString()));
    }
    return outputLst;
  }

  void ImportWavFilesIntoProject(QString pathToWavFiles,
                                 QString projectDirectoryPath) {
    QDir newWavFilePath(pathToWavFiles);

    QStringList lst;
    lst << "*" + QString(".wav");
    newWavFilePath.setNameFilters(lst);
    QStringList entryLst = newWavFilePath.entryList();

    QDir projectAudioDir(getProjectAudioDirectoryPath(projectDirectoryPath));
    for (auto fileEntry : entryLst) {
      bool result = QFile::copy(newWavFilePath.absoluteFilePath(fileEntry),
                                projectAudioDir.absoluteFilePath(fileEntry));
      if (!result) {
        std::cerr << "ERROR: Unable to copy file with path "
                  << newWavFilePath.absoluteFilePath(fileEntry).toStdString()
                  << " To Path: "
                  << projectAudioDir.absoluteFilePath(fileEntry).toStdString();
      }
    }
  }

  void UpdateProjectWavMetaData();

  QString getProjectAudioDirectoryPath(QString projectDirectoryPath) {
    QDir projDir(projectDirectoryPath);
    return projDir.filePath(
        constants::SharedConstants::PROJECT_AUDIO_DIRECTORY_NAME);
  }

private:
  QDir m_parentProjectDir;
};
