#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QUuid>
namespace bassproject {

struct projectMetaPackage {
  Q_GADGET
  Q_PROPERTY(QString projectDir MEMBER projectDirStr)
  Q_PROPERTY(QDateTime projectLastAccessed MEMBER lastAccessed)
  Q_PROPERTY(QString projectName MEMBER projectName)
  Q_PROPERTY(QString birdName MEMBER birdName)

public:
  QDir projectDir;
  QUuid projectID;
  QDateTime timeCreated;
  QDateTime lastAccessed;
  QString projectName;
  QString birdName;
  QString projectDirStr = projectDir.absolutePath();
};

} // namespace bassproject

Q_DECLARE_METATYPE(bassproject::projectMetaPackage)

class BassProject {
public:
  explicit BassProject();
  explicit BassProject(bassproject::projectMetaPackage metaPackage);
  ~BassProject() =
      default; // Update project's metafiles on destruction if needed
  bool wasDefaultConstructed();
  bassproject::projectMetaPackage _projMetaData;
  void updateprojectMetaPackage();
  void syncprojectMetaPackage();

private:
  bool _defaultConstructed;
};
// namespace bassproject
#endif // BASSPROJECT_H
