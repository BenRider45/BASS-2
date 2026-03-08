#ifndef BASSPROJECT_H
#define BASSPROJECT_H

#include <QJsonObject>
#include <QObject>
#include <QQMLEngine>
#include <QString>
#include <filesystem>
class BassProject : public QObject {
  Q_OBJECT
  Q_PROPERTY(QJsonObject projectMetaData READ projectMetaData NOTIFY
                 projectMetaDataChanged)
public:
  explicit BassProject(std::filesystem::path projDir, QString projName,
                       QObject *parent = nullptr);
  explicit BassProject(QJsonObject pdata);
  QJsonObject projectMetaData;
signals:
  void projectMetaDataChanged();
  void projectInitialized(QJsonObject projectMetaData);

private:
  void initProject();
  std::filesystem::path _projectDir;
  std::filesystem::path _wavDir;
  std::filesystem::path _cacheDir;
  std::filesystem::path _logDir;
};

#endif // BASSPROJECT_H
