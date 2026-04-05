#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QUuid>
namespace bassproject {

struct projectMetaPackage {

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
  void updateProjectMetaPackage();
  void syncprojectMetaPackage();

  friend std::ostream &operator<<(std::ostream &os, const BassProject &proj) {
    os << "PROJECT:\n"
       << "Name: " << proj._projMetaData.projectName.toStdString() << "\n ID: "
       << proj._projMetaData.projectID.toString(QUuid::WithoutBraces)
              .toStdString()
       << "\n DIR: "
       << proj._projMetaData.projectDir.absolutePath().toStdString()
       << "\n LAST_ACCESSED: "
       << proj._projMetaData.lastAccessed.toString().toStdString();
    return os; // Enables operator cascading (e.g., std::cout << d1 << d2;)
  }

private:
  bool _defaultConstructed;
};
// namespace bassproject
#endif // BASSPROJECT_H
