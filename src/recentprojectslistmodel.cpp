#include "recentprojectslistmodel.h"
#include "metamanager.h"
#include "recentProject.h"
#include "sharedconstants.h"
#include <QStandardPaths>
RecentProjectsModel::RecentProjectsModel(QObject *parent)
    : QAbstractListModel(parent) {}

int RecentProjectsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return m_recentProjects.size();
}

QVariant RecentProjectsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_recentProjects.size()) {
    return {};
  }

  const auto &entry = m_recentProjects.at(index.row());

  switch (role) {
  case projectNameRole:
    return entry._projectName;
  case projectDirRole:
    return entry._projectDir.absolutePath();
  case projectUIDRole:
    return entry._projectID;
  case projectLastAccessedRole:
    return entry._lastAccessed;
  }
  return {};
}

RecentProject RecentProjectsModel::getProject(QModelIndex idx) {
  QString projectName = data(idx, projectNameRole).value<QString>();
  QDir projectDir = QDir(data(idx, projectDirRole).value<QString>());
  QUuid projectID = QUuid(data(idx, projectUIDRole).value<QUuid>());
  QDateTime projectLA =
      QDateTime(data(idx, projectLastAccessedRole).value<QDateTime>());
  RecentProject proj(projectName, projectID, projectLA, projectDir);
  std::cerr << proj << "\n";
  return proj;
}
QHash<int, QByteArray> RecentProjectsModel::roleNames() const {
  return {{projectNameRole, "projectName"},
          {projectDirRole, "projectDir"},
          {projectUIDRole, "projectUID"},
          {projectLastAccessedRole, "projectLastAccessed"}};
}

Result<bool> RecentProjectsModel::loadRecentProjects(
    const QVector<RecentProject> &recentProjectsList) {

  for (auto proj : recentProjectsList) {
    Result<bool> res = addProject(proj);
    if (!res.isSuccess()) {
      return Result<bool>::err(QString("ERROR Loading Recent Projects"), 1);
    }
  }
  return Result<bool>::ok(true);
}

Result<bool> RecentProjectsModel::addProject(RecentProject &newProject) {
  int newIndex = m_recentProjects.size();
  beginInsertRows(QModelIndex(), newIndex, newIndex);
  m_recentProjects.append(newProject);
  endInsertRows();
  return Result<bool>::ok(true);
}

int RecentProjectsModel::getIndexByUID(const QUuid &uid) {

  for (int i = 0; i < m_recentProjects.size(); i++) {
    RecentProject proj = m_recentProjects.at(i);
    if (proj._projectID == uid) {
      return i;
    }
  }
  return -1;
}
void RecentProjectsModel::setProjectName(QString projectName) {
  std::cerr << "set ProjectName!!\n";
}
Result<bool> RecentProjectsModel::updateRecentProjectsFile() {

  QDir appDataLoc =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  QDir RecentProjectsMetaDataDir(appDataLoc.filePath(
      constants::SharedConstants::RECENT_PROJECT_APPDATA_DIRECTORY));

  QString RecentProjectsMetaFilePath = MetaManager::getMetaFilePath(
      RecentProjectsMetaDataDir,
      constants::SharedConstants::RECENT_PROJ_METAFILE_NAME);
  std::cerr << "Got to updateRecentProjects\n";
  for (auto proj : m_recentProjects) {

    std::cerr << proj << "\n";
    Result<bool> writeRes = MetaManager::writeData<RecentProject>(
        RecentProjectsMetaFilePath,
        proj._projectID.toString(QUuid::WithoutBraces), proj);
    if (!writeRes.isSuccess()) {
      return Result<bool>::err(
          QString("ERROR: Issue updating RecentProjectsFile in "
                  "RecentProjectsModel\n"),
          3);
    }
  }
  return Result<bool>::ok(true);
}

void RecentProjectsModel::onProjectOpened(RecentProject &openedProject) {
  std::cerr << "Inside of onProjectOpened in RecentProjectsModel\n";

  int indx = getIndexByUID(openedProject._projectID);
  openedProject._lastAccessed = QDateTime::currentDateTime();
  if (indx == -1) {
    addProject(openedProject);
    return;
  }
  std::cerr << "touching project at index: " << indx << "\n";

  QModelIndex modIdx = index(indx, 0);
  std::cerr << "Old Time:"
            << data(modIdx, projectLastAccessedRole).toString().toStdString()
            << "\n";

  m_recentProjects[indx]._lastAccessed = QDateTime::currentDateTime();
  dataChanged(modIdx, modIdx, QList<int>(projectLastAccessedRole));
  std::cerr << data(modIdx, projectLastAccessedRole).toString().toStdString();
}

QString RecentProjectsModel::projectName() const { return QString(); }

QDir RecentProjectsModel::projectDir() const { return QDir(); }

QUuid RecentProjectsModel::projectUID() const { return QUuid(); }

QDateTime RecentProjectsModel::projectLastAccessed() const {
  return QDateTime();
}
