#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QString>
#include <QUuid>

class BassProject {
public:
  explicit BassProject(QJsonObject pMetaData);
  ~BassProject(); // Update project's metafiles

private:
  QDir _projectDir;
  QUuid _projectID;
  QDateTime _timeCreated;
  QDateTime _lastAccessed;
};

#endif // BASSPROJECT_H
