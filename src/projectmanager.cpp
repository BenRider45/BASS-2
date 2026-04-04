
#include "projectmanager.h"
#include "bassproject.h"
#include "metamanager.h"
#include "projectFactory.h"
#include "recentProject.h"
#include "sharedconstants.h"
#include <QDateTime>
#include <QDir>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QUuid>

namespace {
bool RecentProjectsDirty = false;

void debug_printJsonObject(QJsonObject obj) {
  QJsonDocument doc(obj);
  QString jsonString = doc.toJson();
  qDebug() << jsonString;
}

void debug_printJsonArray(QJsonArray arr) {
  QJsonDocument doc(arr);
  QString jsonString = doc.toJson();
  qDebug() << jsonString;
}

} // namespace

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent), m_recentProjects(RecentProjectsModel(this)) {
  QDir recentProjectDataLoc =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  if (!recentProjectDataLoc.exists()) {
    qDebug() << "Creating AppDataLocation\n";
    recentProjectDataLoc.mkpath(".");
  }

  if (!recentProjectDataLoc.exists(
          constants::SharedConstants::RECENT_PROJECT_APPDATA_DIRECTORY)) {
    qWarning("Recent Project Data directory not found in local data: Creating "
             "new\n");
    RecentProjectsDirty = true;
    recentProjectDataLoc.mkpath(
        constants::SharedConstants::RECENT_PROJECT_APPDATA_DIRECTORY);
    recentProjectDataLoc = recentProjectDataLoc.filePath(
        constants::SharedConstants::RECENT_PROJECT_APPDATA_DIRECTORY);
  } else {
    recentProjectDataLoc = recentProjectDataLoc.filePath(
        constants::SharedConstants::RECENT_PROJECT_APPDATA_DIRECTORY);
  }

  if (!MetaManager::VerifyMetaFileExistence(recentProjectDataLoc)) {
    qWarning("Recent Project Data Meta File not Found, creating ");
    MetaManager::createMetaFile(
        recentProjectDataLoc,
        constants::SharedConstants::RECENT_PROJ_METAFILE_NAME);
  }

  QString recentProjMetaFilePath = MetaManager::getMetaFilePath(
      recentProjectDataLoc,
      constants::SharedConstants::RECENT_PROJ_METAFILE_NAME);

  Result<QVector<RecentProject>> recentProjects =
      MetaManager::RecentProjectUtils::extractRecentProjectList(
          recentProjMetaFilePath);
  assert(recentProjects.isSuccess());
  m_recentProjects.loadRecentProjects(recentProjects.value());
  emit recentProjectsModelChanged();
  qDebug() << "Successfully read in recent project data from:\n"
           << recentProjectDataLoc.absolutePath() << "\n";

  m_isInitialized = true;
}

ProjectManager::~ProjectManager() {
  qDebug() << "Shutting down Project Manager!!!\n";
  if (!RecentProjectsDirty) {
    qDebug() << "Recent Projects Not Dirty\n";
  }
  updateRecentProjectsFile();
  return;
}

void ProjectManager::updateRecentProjectsFile() {
  Result<bool> writeRes = m_recentProjects.updateRecentProjectsFile();
  if (!writeRes.isSuccess()) {
    std::cerr << writeRes.error().toStdString();
  }

  RecentProjectsDirty = false;
}

bassproject::projectMetaPackage ProjectManager::projectMetadata() const {
  return m_currentProject->_projMetaData;
}

QString ProjectManager::birdName() const { return m_birdName; }
QString ProjectManager::projectDir() const { return m_projectDir; }
QString ProjectManager::wavDir() const { return m_wavDir; }

RecentProjectsModel *ProjectManager::recentProjects() {
  return &m_recentProjects;
}

bool ProjectManager::isInitialized() const { return m_isInitialized; }
bool ProjectManager::projectAttached() {
  qDebug("projectAttached");
  return m_projectAttached;
}

void ProjectManager::importWavFiles(const QString &wavDir) {
  qDebug() << "Importing Wav Files from : " << wavDir << "\n";
}

void ProjectManager::initProject(const QString &projectDir,
                                 const QString &projectName,
                                 const QString &birdName) {

  if (MetaManager::VerifyMetaFileExistence(projectDir)) {
    emit error("Cannot create a project within another project!");
    return;
  }
  qDebug() << "Creating Project at directory" << projectDir << "\n"
           << "With name : " << projectName << "\n"
           << "With bird name :" << birdName << "\n";

  QStringList keys;
  QVariantList values;
  bassproject::projectMetaPackage projMetaPackage;
  projMetaPackage.projectDir = QDir(projectDir);
  projMetaPackage.projectID = QUuid::createUuid();
  projMetaPackage.timeCreated = QDateTime::currentDateTime();
  projMetaPackage.lastAccessed = QDateTime::currentDateTime();
  projMetaPackage.projectName = projectName;
  projMetaPackage.birdName = birdName;
  ProjectFactory::createProjectFromNew(projMetaPackage);
  loadProject(projectDir);
}

void ProjectManager::loadProject(const QString &projDir) {
  qDebug() << "Loading Project with config at: " << projDir << "\n";
  // check for metadatafile if doesnt exist throw
  // if found, update access date value, allocate class QJSONObject with project
  // data
  std::unique_ptr<BassProject> bassProj =
      ProjectFactory::createProjectFromMetaData(projDir);

  QString metaDataPath = MetaManager::getMetaFilePath(
      projDir, constants::SharedConstants::PROJECT_META_FILE_NAME);
  // find in recent projects, if in recent projects update last accessed date
  qDebug() << "metaDataPath: " << metaDataPath.toStdString() << "\n";

  bassProj->_projMetaData.lastAccessed = QDateTime::currentDateTime();

  MetaManager::writeData<QDateTime>(
      metaDataPath, constants::SharedConstants::PROJECT_LAST_ACCESSED,
      QDateTime::currentDateTime());

  RecentProject proj(
      bassProj->_projMetaData.projectName, bassProj->_projMetaData.projectID,
      bassProj->_projMetaData.lastAccessed, bassProj->_projMetaData.projectDir);

  emit projectLoading(QString(proj._projectName));
  updateRecentProjects(proj);
  updateRecentProjectsFile();

  setCurrentProject(std::move(bassProj));

  setProjectAttached(true);
  emit projectLoaded();
}
void ProjectManager::setCurrentProject(std::unique_ptr<BassProject> bassProj) {
  std::cerr << "got to setCurrentProject\n";
  m_currentProject = std::move(bassProj);
  std::cerr << *m_currentProject.get() << "\n";

  std::cerr << "Std::move didnt blow up\n";

  emit projectMetadataChanged();
}
void ProjectManager::loadRecentProject(const QString &UID) {
  qDebug() << "Loading Recent Project" << "\n";
  qDebug() << "Looking for UUID" << UID << "\n";
  QUuid uUid(UID);
  qDebug() << "uUid: " << uUid << "\n";
  QModelIndexList results = m_recentProjects.match(
      m_recentProjects.index(0, 0), RecentProjectsModel::projectUIDRole, uUid,
      1, Qt::MatchContains);
  int idx = m_recentProjects.getIndexByUID(uUid);

  qDebug() << "idx :" << idx << "\n";
  if (results.isEmpty()) {
    emit error("Cannot Find Recent Project Project with UUID" + UID + "\n");
    return;
  }

  qDebug() << "Results: " << results.first() << "\n";
  qDebug() << "Result.length" << results.size() << "\n";
  RecentProject proj = m_recentProjects.getProject(results.first());
  std::cerr << proj << "n";
  loadProject(proj._projectDir.absolutePath());
  return;
}

void ProjectManager::updateRecentProjects(RecentProject projData) {
  m_recentProjects.onProjectOpened(projData);
}

QJsonObject ProjectManager::fromProjectMetaToJson(
    bassproject::projectMetaPackage metaPackage) {

  QJsonObject obj;
  obj[constants::SharedConstants::PROJECT_DIR] =
      QVariant::fromValue(metaPackage.projectDir.absolutePath()).toString();
  obj[constants::SharedConstants::PROJECT_ID] =
      QVariant::fromValue(metaPackage.projectID).toJsonValue();
  obj[constants::SharedConstants::PROJECT_CREATE_DATE] =
      QVariant::fromValue(metaPackage.timeCreated).toJsonValue();
  obj[constants::SharedConstants::PROJECT_LAST_ACCESSED] =
      QVariant::fromValue(metaPackage.lastAccessed).toJsonValue();
  obj[constants::SharedConstants::PROJECT_NAME] =
      QVariant::fromValue(metaPackage.projectName).toJsonValue();

  obj[constants::SharedConstants::PROJECT_BIRD_NAME] =
      QVariant::fromValue(metaPackage.birdName).toJsonValue();
  return obj;
}

// QString ProjectManager::buildProjectMetaFile(const QString &projDir) {
//   QDir dir(projDir);
//   assert(MetaManager::createMetaFile(
//       projDir, constants::SharedConstants::PROJECT_META_FILE_NAME));
//   QJsonObject obj;
//   QJsonObject container;
//
//   QStringList keys;
//   keys << constants::SharedConstants::PROJECT_ID
//        << constants::SharedConstants::PROJECT_DATA;
//
//   container[constants::SharedConstants::PROJECT_ID] =
//       QUuid::createUuid().toString(QUuid::WithoutBraces);
//   obj[constants::SharedConstants::PROJECT_DIR] = projDir;
//   container[constants::SharedConstants::PROJECT_DATA] = obj;
//   QVariantList values;
//   values << QUuid::createUuid().toString(QUuid::WithoutBraces);
//   values.append(container);
//
//   MetaManager::modMetaFile(
//       MetaManager::getMetaFilePath(
//           projDir, constants::SharedConstants::PROJECT_META_FILE_NAME),
//       keys, values, true);
//
//   return dir.filePath(constants::SharedConstants::PROJECT_META_FILE_NAME +
//                       constants::SharedConstants::META_FILE_EXTENSION);
// }
