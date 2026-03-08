#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QString>
#include <filesystem>

class ProjectManager : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString birdName READ birdName NOTIFY projectLoaded)
  Q_PROPERTY(QString projectDir READ projectDir NOTIFY projectLoaded)
  Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY isInitializedChanged)
  Q_PROPERTY(bool projectAttached READ projectAttached WRITE setProjectAttached
                 NOTIFY projectAttachedChanged)
  Q_PROPERTY(QVariantMap currentProjectData READ currentProjectData NOTIFY
                 currentProjectChanged)
  Q_PROPERTY(QVariantList recentProjects READ recentProjects NOTIFY
                 recentProjectsChanged)
  Q_PROPERTY(QString PROJECT_DIR MEMBER PROJECT_DIR CONSTANT);
  Q_PROPERTY(QString PROJECT_BIRD_NAME MEMBER PROJECT_BIRD_NAME CONSTANT);
  Q_PROPERTY(QString PROJECT_NAME MEMBER PROJECT_NAME CONSTANT);
  Q_PROPERTY(
      QString PROJECT_LAST_ACCESSED MEMBER PROJECT_LAST_ACCESSED CONSTANT);
  Q_PROPERTY(QString PROJECT_CREATE_DATE MEMBER PROJECT_CREATE_DATE CONSTANT);
  Q_PROPERTY(QString PROJECT_ID MEMBER PROJECT_ID CONSTANT);
  Q_PROPERTY(QString PROJECT_DATA MEMBER PROJECT_DATA CONSTANT);

public:
  explicit ProjectManager(QObject *parent = nullptr);
  ~ProjectManager();
  QString birdName() const;
  QString projectDir() const;
  QString wavDir() const;
  bool isInitialized() const;
  bool projectAttached() const;
  QVariantMap currentProjectData() const;
  QVariantList recentProjects() const;
  void buildProjectDirectory(const QString &ProjDir);
  Q_INVOKABLE void initProject(const QString &projectDir,
                               const QString &projectName,
                               const QString &birdName);

  Q_INVOKABLE void loadProject(const QString &projDir);
  Q_INVOKABLE void loadRecentProject(const QString &UID);
  Q_INVOKABLE void importWavFiles(const QString &wavDir);
  void setProjectAttached(bool val) {
    m_projectAttached = val;
    emit projectAttachedChanged();
  }
  void setCurrentProjectData(QJsonObject ProjData) {
    m_currentProjectData = ProjData;
    emit currentProjectChanged();
  }

  const QString META_FILE_NAME = "conf.twty";
  const QString PROJECT_DIR = "proj_dir";
  const QString PROJECT_BIRD_NAME = "proj_bird";
  const QString PROJECT_NAME = "proj_name";
  const QString PROJECT_LAST_ACCESSED = "last_access";
  const QString PROJECT_CREATE_DATE = "create_date";
  const QString PROJECT_ID = "ID";
  const QString PROJECT_DATA = "proj_data";
signals:
  void projectLoading(QString projDir);
  void isInitializedChanged();
  void projectAttachedChanged();
  void projectLoaded();
  void projectOpened();
  void currentProjectChanged();
  void recentProjectsChanged();
  void error(QString errorMessage);

private:
  QString m_birdName;
  QString m_projectDir;
  QString m_wavDir;
  QJsonObject m_currentProjectData;
  QJsonArray m_recentProjects;
  bool m_isInitialized = false;
  bool m_projectAttached = false;
  void buildProjectDirectory(std::filesystem::path projdir);
  bool searchForProject();
  void updateRecentProjects(QJsonObject projData);
  void updateRecentProjectsFile();
  QString buildProjectMetaFile(const QString &projDir);

  bool findMetaFile(const QString &ProjDir);
  bool modMetaFile(const QString &path, const QStringList &key,
                   const QStringList &value, bool addValueIfNeeded = false);
  QString getMetaDataPath(const QString &projectDirectory);
  QJsonObject extractMetaDataContent(const QString &metaFilePath);
};

#endif
