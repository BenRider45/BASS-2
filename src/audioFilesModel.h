#pragma once
#include "result.h"
#include "sharedconstants.h"
#include "wavfile.h"
#include <QAbstractListModel>
#include <QDateTime>
#include <QHash>
#include <QString>
class AudioFilesModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(
      int currentFileIndex READ currentFileIndex NOTIFY currentFileIndexChanged)
public:
  enum Roles {
    fileNameRole = Qt::UserRole + 1,
    fileLengthRole,
    isCurrentSelectedFileRole
  };

  explicit AudioFilesModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  QHash<int, QByteArray> roleNames() const override;

  int currentFileIndex() const { return m_currentSelectedFileRowIdx; }
  Q_INVOKABLE void setCurrentFile(const int idx);

  Result<bool> loadAudioFiles(
      const QVector<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> &AudioFiles);

  Result<bool>
  addAudioFile(const WavFile<SharedTypeDefs::WAVFILE_SAMPLE> &audioFile);

  // TODO implement
  std::unique_ptr<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>>
  getAudioFile(int idx);
  void clearModel();
signals:
  void currentFileIndexChanged();

private:
  QVector<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> m_wavFiles;
  int m_currentSelectedFileRowIdx = -1;
};
