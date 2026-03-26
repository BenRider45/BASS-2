#include "projectmanager.h"
#include "bassproject.h"
#include "projectFactory.h"
#include "sharedconstants.h"
#include <QDateTime>
#include <QDir>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QUuid>
namespace {
const QString RECENT_PROJECT_DATA = "RECENT_PROJECT_DATA";
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

ProjectManager::ProjectManager(QObject *parent) : QObject(parent) {
  QDir recentProjectDataLoc =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  if (!recentProjectDataLoc.exists()) {
    qDebug() << "Creating AppDataLocation\n";
    recentProjectDataLoc.mkpath(".");
  }

  if (!recentProjectDataLoc.exists(RECENT_PROJECT_DATA)) {
    qWarning(
        "Recent Project Data file not found in local data: Creating new\n");
    RecentProjectsDirty = true;
    QJsonArray SkeletonRecProject;

    QJsonDocument recProjDataBlank(SkeletonRecProject);

    QFile file(recentProjectDataLoc.filePath(RECENT_PROJECT_DATA));
    if (file.open(QIODevice::WriteOnly)) {
      file.write(recProjDataBlank.toJson());
      qDebug() << "File written to " << recentProjectDataLoc.absolutePath()
               << "\n";
      file.close();
    }
  }

  QFile recentProjData(recentProjectDataLoc.filePath(RECENT_PROJECT_DATA));
  if (!recentProjData.open(QIODeviceBase::ReadOnly)) {
    qWarning("Error Opening RecentProjectData File\n");
    return;
  }
  QByteArray recProjDataByteArray = recentProjData.readAll();
  //  char *data = recProjDataByteArray.data();
  //  while (*data) {
  //    qDebug() << "[" << *data << "]" << "\n";
  //    ++data;
  //  }
  recentProjData.close();
  QJsonParseError err;
  QJsonDocument recProjDataDoc =
      QJsonDocument::fromJson(recProjDataByteArray, &err);

  if (err.error != QJsonParseError::NoError) {
    qDebug() << "Failed to parse JSON doc:" << err.errorString();
    // Handle the error (e.g., return, throw an exception, etc.)
  } else {
    qDebug() << "JSON document created successfully.";

    // Check if the document contains an object or an array
    //
    qDebug() << "isArray?: " << recProjDataDoc.isArray()
             << "\n isObject?: " << recProjDataDoc.isObject() << "\n";
  }

  m_recentProjects = recProjDataDoc.array();
  emit recentProjectsChanged();
  qDebug() << "Successfully read in recent project data from:\n"
           << recentProjectDataLoc.absolutePath() << "\n";
  qDebug() << m_recentProjects;
  QJsonDocument doc(m_recentProjects);

  QString jsonString = doc.toJson(QJsonDocument::Indented);
  qDebug() << jsonString.toStdString();
  m_isInitialized = true;
}

ProjectManager::~ProjectManager() {
  qDebug() << "Shutting this hoe down!!\n";
  if (!RecentProjectsDirty) {
    qDebug() << "Recent Projects Not Dirty\n";
    return;
  }
  updateRecentProjectsFile();
  return;
}

void ProjectManager::updateRecentProjectsFile() {
  qDebug() << "Got to updating file\n";
  QDir recentProjectDataLoc =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  QFile file(recentProjectDataLoc.filePath(RECENT_PROJECT_DATA));

  QJsonDocument doc(m_recentProjects);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "File Didint Open:(";
    emit error("File didnt open ");
  }

  file.write(doc.toJson());
  file.close();
  RecentProjectsDirty = false;
}

bassproject::ProjectMetaPackage ProjectManager::projectMetadata() const {
  return m_currentProject->_projMetaData;
}

QString ProjectManager::birdName() const { return m_birdName; }
QString ProjectManager::projectDir() const { return m_projectDir; }
QString ProjectManager::wavDir() const { return m_wavDir; }

QVariantList ProjectManager::recentProjects() const {
  return m_recentProjects.toVariantList();
}

bool ProjectManager::isInitialized() const { return m_isInitialized; }
bool ProjectManager::projectAttached() const { return m_projectAttached; }

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
  QStringList keys;
  QVariantList values;
  bassproject::ProjectMetaPackage projMetaPackage;
  projMetaPackage.projectDir = projectDir;
  projMetaPackage.projectID = QUuid::createUuid();
  projMetaPackage.timeCreated = QDateTime::currentDateTime();
  projMetaPackage.lastAccessed = QDateTime::currentDateTime();
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

  QJsonObject metaData = fromProjectMetaToJson(bassProj->_projMetaData);
  debug_printJsonObject(metaData);
  emit projectLoading(
      QString(metaData[constants::SharedConstants::PROJECT_NAME].toString()));
  updateRecentProjects(metaData);
  updateRecentProjectsFile();

  setProjectAttached(true);
  setCurrentProject(std::move(bassProj));
  emit projectLoaded();
}
void ProjectManager::setCurrentProject(std::unique_ptr<BassProject> bassProj) {
  m_currentProject = std::move(bassProj);
  emit projectMetadataChanged();
}
void ProjectManager::loadRecentProject(const QString &UID) {
  qDebug() << "Loading Recent Project";
  qDebug() << "Looking for UUID" << UID;
  for (auto val : m_recentProjects) {
    QJsonObject obj = val.toObject();
    if (QUuid(obj[constants::SharedConstants::PROJECT_ID].toString()) ==
        QUuid(UID)) {
      qDebug() << "Found project " << UID << "in recent projects\n"
               << "Project lives at "
               << obj[constants::SharedConstants::PROJECT_DIR].toString()
               << "\n";
      loadProject(obj[constants::SharedConstants::PROJECT_DIR].toString());
      return;
    }
  }
  emit error("Cannot Find Recent Project Project with UUID" + UID + "\n");
}

void ProjectManager::updateRecentProjects(QJsonObject projData) {
  qDebug() << "updateRecentProjects\n ";
  RecentProjectsDirty = true;
  bool valueFound = false;
  for (int i = 0; i < m_recentProjects.size(); i++) {
    qDebug() << "i = " << i << "\n";
    qDebug() << "m_recentProjects.at(i).isArray(): "
             << m_recentProjects.at(i).isArray();

    qDebug() << "m_recentProjects.at(i).isObject(): "
             << m_recentProjects.at(i).isObject();
    QJsonObject obj = m_recentProjects.at(i).toObject();
    if (projData[constants::SharedConstants::PROJECT_ID].toString() ==
        obj[constants::SharedConstants::PROJECT_ID].toString()) {
      valueFound = true;
      qDebug("Updateing project!!!");
      obj = projData;

      m_recentProjects.replace(i, obj);
      return;
    }
  }
  if (!valueFound) {
    qDebug() << "Appending projData\n";
    m_recentProjects.append(projData);
    return;
  }
}

QJsonObject ProjectManager::fromProjectMetaToJson(
    bassproject::ProjectMetaPackage metaPackage) {

  QJsonObject obj;
  obj[constants::SharedConstants::PROJECT_DIR] =
      QVariant::fromValue(metaPackage.projectDir).toJsonValue();
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
