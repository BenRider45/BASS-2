#pragma once
#include "dependencies/AudioFile/AudioFile.h"
#include <QFileInfo>
#include <QJsonObject>
#include <QString>
#include <QVector>
#include <string>
#include <type_traits>
namespace wavfileUtils {
template <typename T>
concept Numeric = requires(T a) { std::is_arithmetic<T>(); };

template <Numeric T> using WavBuffer = std::vector<T>;

template <Numeric T> struct WavFileData {
  WavBuffer<T> WavData;
};

} // namespace wavfileUtils

// namespace wavfileUtils
template <wavfileUtils::Numeric T> class WavFile {
public:
  WavFile(std::string filePath)
      : m_AudioFile(AudioFile<T>(filePath)), m_filePath(filePath) {};
  WavFile() = default;
  WavFile(const WavFile<T> &other)
      : m_filePath(other.m_filePath), m_AudioFile(other.m_AudioFile) {};

  std::string m_filePath;

  double GetLengthInSeconds() const {
    return m_AudioFile.getLengthInSeconds();
  };

  double getSampleRate() const { return m_AudioFile.getSampleRate(); }
  double getDeltaTPerSample() const {
    return 1.0 / m_AudioFile.getSampleRate();
  }
  bool operator==(const WavFile<T> &other) const {
    return m_filePath == other.m_filePath;
  }
  QString getFileName() {
    if (m_filePath != "") {
      std::cerr << "Getting File Name : "
                << QFileInfo(QString::fromStdString(m_filePath))
                       .fileName()
                       .toStdString()
                << "\n";

      return QFileInfo(QString::fromStdString(m_filePath)).fileName();
    }
    return "";
  }
  AudioFile<T> m_AudioFile;
};

Q_DECLARE_METATYPE(WavFile<uint32_t>);
