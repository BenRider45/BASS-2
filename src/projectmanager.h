#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "bassproject.h"
#include "recentProject.h"
#include "recentprojectslistmodel.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QString>
class ProjectManager : public QObject {
  Q_OBJECT

  Q_PROPERTY(bassproject::projectMetaPackage projectMetadata READ
                 projectMetadata NOTIFY projectMetadataChanged)

  Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY isInitializedChanged)
  Q_PROPERTY(bool projectAttached READ projectAttached WRITE setProjectAttached
                 NOTIFY projectAttachedChanged)
  Q_PROPERTY(RecentProjectsModel *recentProjects READ recentProjects NOTIFY
                 recentProjectsModelChanged);

public:
  explicit ProjectManager(QObject *parent = nullptr);
  ~ProjectManager();
  QString birdName() const;
  QString projectDir() const;
  QString wavDir() const;
  bool isInitialized() const;
  bool projectAttached();
  QVariantMap currentProjectData() const;
  bassproject::projectMetaPackage projectMetadata() const;

  RecentProjectsModel *recentProjects();
  void buildProjectDirectory(const QString &ProjDir);
  Q_INVOKABLE void initProject(const QString &projectDir,
                               const QString &projectName,
                               const QString &birdName);

  Q_INVOKABLE void loadProject(const QString &projDir);
  Q_INVOKABLE void loadRecentProject(const QString &UID);
  Q_INVOKABLE void importWavFiles(const QString &wavDir);
  void setProjectAttached(bool val) {
    qDebug() << "Got to setProjectsAttached\n";
    m_projectAttached = val;
    qDebug() << "value set\n";
    emit projectAttachedChanged();
    qDebug() << "Signal Emitted\n";
  }

signals:
  void projectLoading(QString projDir);
  void isInitializedChanged();
  void projectMetadataChanged();
  void projectAttachedChanged();
  void projectLoaded();
  void projectOpened();
  void currentProjectChanged();
  void error(QString errorMessage);
  void recentProjectsModelChanged();

private:
  QString m_birdName;
  QString m_projectDir;
  QString m_wavDir;
  RecentProjectsModel m_recentProjects;
  bool m_isInitialized = false;
  bool m_projectAttached = false;
  std::unique_ptr<BassProject> m_currentProject;
  bool searchForProject();
  void updateRecentProjects(RecentProject projData);
  void updateRecentProjectsFile();
  void setCurrentProject(std::unique_ptr<BassProject> bassProj);
  QJsonObject
  fromProjectMetaToJson(bassproject::projectMetaPackage metaPackage);
};

#endif
