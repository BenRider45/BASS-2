#pragma once
#include "sharedconstants.h"
#include "wavfile.h"
#include <QDir>
#include <QString>

namespace WavSurferUtils {
template <wavfileUtils::Numeric T> using WavFileList = QVector<WavFile<T>>;

}

template <wavfileUtils::Numeric T> class WavSurfer {
public:
  WavSurferUtils::WavFileList<T>
  RetrieveWavFiles(QString projectDirectoryPath) {

    QString pathToWavDir = getProjectAudioDirectoryPath(projectDirectoryPath);
    QDir WavDir(pathToWavDir);
    QStringList lst;
    lst << "*" + QString(".wav");
    WavDir.setNameFilters(lst);
    QStringList entryLst = WavDir.entryList();

    WavSurferUtils::WavFileList<T> outputLst;
    for (auto entry : entryLst) {
      outputLst.append(WavFile<T>(WavDir.filePath(entry).toStdString()));
    }
    return outputLst;
  }

  WavSurferUtils::WavFileList<T>
  getWavFilesFromNonProjectDir(QString Directory) {
    QDir WavDir(Directory);
    QStringList lst;
    lst << "*" + QString(".wav");
    WavDir.setNameFilters(lst);
    QStringList entryLst = WavDir.entryList();

    WavSurferUtils::WavFileList<T> outputLst;
    for (auto entry : entryLst) {
      outputLst.append(WavFile<T>(WavDir.filePath(entry).toStdString()));
    }
    return outputLst;
  }

  WavSurferUtils::WavFileList<T>
  ImportWavFilesIntoProject(QString pathToWavFiles,
                            QString projectDirectoryPath) {
    WavSurferUtils::WavFileList<T> originalWavFileList =
        RetrieveWavFiles(projectDirectoryPath);

    QDir newWavFilePath(pathToWavFiles);
    for (auto entry : newWavFilePath.entryList()) {
      qDebug() << "UnFilteredEntryList Entry: " << entry << "\n";
    }
    QStringList lst;
    lst << "*" + QString(".wav");
    qDebug() << "lst: " << lst << "\n";
    newWavFilePath.setNameFilters(lst);
    QStringList entryLst = newWavFilePath.entryList();
    qDebug() << "Importing " << entryLst.size()
             << " files to project at: " << projectDirectoryPath << "\n";
    QDir projectAudioDir(getProjectAudioDirectoryPath(projectDirectoryPath));
    for (auto fileEntry : entryLst) {
      bool result = QFile::copy(newWavFilePath.absoluteFilePath(fileEntry),
                                projectAudioDir.absoluteFilePath(fileEntry));
      if (!result) {
        std::cerr << "ERROR: Unable to copy file with path "
                  << newWavFilePath.absoluteFilePath(fileEntry).toStdString()
                  << " To Path: "
                  << projectAudioDir.absoluteFilePath(fileEntry).toStdString()
                  << "\n";
      }
    }
    WavSurferUtils::WavFileList<T> newWavFileList =
        RetrieveWavFiles(projectDirectoryPath);

    WavSurferUtils::WavFileList<T> output;
    for (auto const &entry : newWavFileList) {
      if (!originalWavFileList.contains(entry)) {
        output.append(entry);
      }
    }

    return output;
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
