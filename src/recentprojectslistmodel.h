#pragma once
#include "recentProject.h"
#include "result.h"
#include <QAbstractListModel>
#include <QDateTime>
#include <QHash>
#include <QString>
class RecentProjectsModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QString projectName READ projectName WRITE setProjectName NOTIFY
                 projectNameChanged)
  Q_PROPERTY(QDir projectDir READ projectDir NOTIFY projectDirChanged)
  Q_PROPERTY(QUuid projectUID READ projectUID NOTIFY projectUIDChanged)
  Q_PROPERTY(QDateTime projectLastAccessed READ projectLastAccessed NOTIFY
                 projectLastAccessedChanged)
public:
  enum Roles {
    projectNameRole = Qt::UserRole + 1,
    projectDirRole,
    projectUIDRole,
    projectLastAccessedRole
  };

  explicit RecentProjectsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  QHash<int, QByteArray> roleNames() const override;

  int getIndexByUID(const QUuid &uid);
  Result<bool>
  loadRecentProjects(const QVector<RecentProject> &recentProjectList);

  Result<bool> setCurrentProject(const QUuid &uid);

  Result<bool> addProject(RecentProject &newRecentProject);

  void setProjectName(QString projectName);
  void onProjectOpened(RecentProject &openedProject);

  Result<bool> updateRecentProjectsFile();
  QString projectName() const;
  QDir projectDir() const;
  QUuid projectUID() const;
  QDateTime projectLastAccessed() const;

  RecentProject getProject(QModelIndex idx);
signals:
  void projectNameChanged();
  void projectDirChanged();
  void projectUIDChanged();
  void projectLastAccessedChanged();

private:
  QVector<RecentProject> m_recentProjects;
};
