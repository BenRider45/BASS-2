#pragma once

#include <QDateTime>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUuid>
#include <QVariantList>

class RecentProject {
public:
  RecentProject() : _isDefault(true) {}
  RecentProject(QString projectName, QUuid projectID, QDateTime lastAccessed,
                QDir projectDir)

      : _projectName(projectName), _projectID(projectID),
        _lastAccessed(lastAccessed), _projectDir(projectDir),
        _isDefault(false) {}

  RecentProject(const RecentProject &other)
      : _isDefault(false), _projectName(other._projectName),
        _projectDir(other._projectDir), _projectID(other._projectID),
        _lastAccessed(other._lastAccessed) {}

  ~RecentProject() = default;
  friend std::ostream &operator<<(std::ostream &os, const RecentProject &proj) {
    os << "PROJECT:\n"
       << "Name: " << proj._projectName.toStdString() << "\n ID: "
       << proj._projectID.toString(QUuid::WithoutBraces).toStdString()
       << "\n DIR: " << proj._projectDir.absolutePath().toStdString()
       << "\n LAST_ACCESSED: " << proj._lastAccessed.toString().toStdString();
    return os; // Enables operator cascading (e.g., std::cout << d1 << d2;)
  }

  QString _projectName;
  QUuid _projectID;
  QDateTime _lastAccessed;
  QDir _projectDir;
  bool _isDefault;
};

Q_DECLARE_METATYPE(RecentProject);

// namespace RecentProjectUtils
