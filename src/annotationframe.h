#pragma once
#include <QMetaType>
#include <assert.h>
#include <string>
namespace AnnotationFrameUtils {
struct FrameInterval {
  int startFrame = 0;
  int endFrame = 1;
};

}; // namespace AnnotationFrameUtils
Q_DECLARE_METATYPE(AnnotationFrameUtils::FrameInterval);
class AnnotationFrame {
public:
  AnnotationFrame(AnnotationFrameUtils::FrameInterval interval,
                  std::string syllableId)
      : m_interval(interval), m_syllableID(syllableId) {};

  bool setInterval(int newFrameStart, int newFrameEnd) {
    if (newFrameStart >= newFrameEnd) {
      return false;
    }
    m_interval.startFrame = newFrameStart;
    m_interval.endFrame = newFrameEnd;
    return true;
  }

private:
  std::string m_syllableID;
  AnnotationFrameUtils::FrameInterval m_interval;
  std::string m_fileName;
};
