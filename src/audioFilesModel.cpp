#include "audioFilesModel.h"
#include "sharedconstants.h"
#include <QFile>
#include <QFileInfo>

AudioFilesModel::AudioFilesModel(QObject *parent)
    : QAbstractListModel(parent) {}

int AudioFilesModel::rowCount(const QModelIndex &parent) const {

  if (parent.isValid()) {
    return 0;
  }
  return m_wavFiles.size();
}

QHash<int, QByteArray> AudioFilesModel::roleNames() const {
  return {{fileNameRole, "fileName"},
          {fileLengthRole, "fileLength"},
          {isCurrentSelectedFileRole, "isCurrentSelectedFile"}};
}
QVariant AudioFilesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_wavFiles.size()) {

    return {};
  }

  const auto &entry = m_wavFiles.at(index.row());
  switch (role) {
  case fileNameRole: {
    QFileInfo file(QString::fromStdString(entry.m_filePath));
    return file.fileName();
  }
  case fileLengthRole:
    return entry.GetLengthInSeconds();
  case isCurrentSelectedFileRole:
    return index.row() == m_currentSelectedFileRowIdx;
  }
  return {};
}

void AudioFilesModel::clearModel() {
  beginResetModel();
  m_wavFiles.clear();
  m_currentSelectedFileRowIdx = -1;
  endResetModel();
}

void AudioFilesModel::setCurrentFile(int idx) {
  setData(index(idx, 0), true, isCurrentSelectedFileRole);
  // TODO make this one dataChanged Call for efficiency
  setData(index(m_currentSelectedFileRowIdx, 0), false);

  m_currentSelectedFileRowIdx = idx;
  dataChanged(index(0, 0), index(m_wavFiles.size() - 1, 0));

  qDebug() << "m_currentSelectedFileRowIdx now: "
           << m_currentSelectedFileRowIdx;
  emit currentFileIndexChanged();
}

Result<bool> AudioFilesModel::loadAudioFiles(
    const QVector<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> &AudioFiles) {

  std::cerr
      << "Got to loading AudioFiles in AudioFIles Model\n input arr length:"
      << AudioFiles.size() << "\n";
  for (auto file : AudioFiles) {
    Result<bool> addRes = addAudioFile(file);
    if (!addRes.isSuccess()) {
      return Result<bool>::err(
          QString("could not add Audio File to AudioFilesModel"), 1);
    }
  }
  return Result<bool>::ok(true);
}

Result<bool> AudioFilesModel::addAudioFile(

    const WavFile<SharedTypeDefs::WAVFILE_SAMPLE> &audioFile) {
  int newIndex = m_wavFiles.size();
  beginInsertRows(QModelIndex(), newIndex, newIndex);
  m_wavFiles.append(audioFile);
  endInsertRows();
  return Result<bool>::ok(true);
}
