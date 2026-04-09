#pragma once
#include "dependencies/AudioFile/AudioFile.h"
#include <QJsonObject>
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

private:
  AudioFile<T> m_AudioFile;
};

Q_DECLARE_METATYPE(WavFile<uint32_t>);
