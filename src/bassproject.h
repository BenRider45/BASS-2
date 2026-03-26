#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QUuid>

namespace bassproject {

struct ProjectMetaPackage {
  QDir projectDir;
  QUuid projectID;
  QDateTime timeCreated;
  QDateTime lastAccessed;
  QString projectName;
  QString birdName;
};

} // namespace bassproject

Q_DECLARE_METATYPE(bassproject::ProjectMetaPackage)

class BassProject {
public:
  explicit BassProject();
  explicit BassProject(bassproject::ProjectMetaPackage metaPackage);
  ~BassProject() =
      default; // Update project's metafiles on destruction if needed
  bool wasDefaultConstructed();
  bassproject::ProjectMetaPackage _projMetaData;

private:
  bool _defaultConstructed;
};
// namespace bassproject
#endif // BASSPROJECT_H
