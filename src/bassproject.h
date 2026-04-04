#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QUuid>
namespace bassproject {

// TODO Make this a class
struct projectMetaPackage {
  Q_OBJECT
  Q_PROPERTY(QString projectDir READ projectDirStr NOTIFY projectDirStrChanged)
  Q_PROPERTY(QDateTime projectLastAccessed READ lastAccessed NOTIFY
                 lastAccessedChanged)
  Q_PROPERTY(QString projectName READ projectName NOTIFY projectName)
  Q_PROPERTY(QString birdName READ birdName NOTIFY birdNameChanged)

public:
  QDir projectDir() const { return m_projectDir; };
  QUuid projectID() const { return m_projectID; };
  QDateTime timeCreated() const { return m_timeCreated; };
  QDateTime lastAccessed() const { return m_lastAccessed; };
  QString projectName() const { return m_projectName; };
  QString birdName() const { return m_birdName; };
  QString projectDirStr() const { return m_projectDirStr; };

signals:
  void projectDirStrChanged() {};
  void lastAccessedChanged() {};
  void projectNameChanged() {};
  void birdNameChanged() {};

private:
  QDir m_projectDir;
  QUuid m_projectID;
  QDateTime m_timeCreated;
  QDateTime m_lastAccessed;
  QString m_projectName;
  QString m_birdName;
  QString m_projectDirStr = m_projectDir.absolutePath();
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

  friend std::ostream &operator<<(std::ostream &os, const BassProject &proj) {
    os << "PROJECT:\n"
       << "Name: " << proj._projMetaData.projectName().toStdString()
       << "\n ID: "
       << proj._projMetaData.projectID()
              .toString(QUuid::WithoutBraces)
              .toStdString()
       << "\n DIR: "
       << proj._projMetaData.projectDir().absolutePath().toStdString()
       << "\n LAST_ACCESSED: "
       << proj._projMetaData.lastAccessed().toString().toStdString();
    return os; // Enables operator cascading (e.g., std::cout << d1 << d2;)
  }

private:
  bool _defaultConstructed;
};
// namespace bassproject
#endif // BASSPROJECT_H
