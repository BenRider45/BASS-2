#include "projectmanager.h"
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
    QJsonObject projContainer;
    projContainer[constants::SharedConstants::PROJECT_ID] =
        QUuid::createUuid().toString(QUuid::WithoutBraces);
    QJsonObject defaultProj;

    projContainer[constants::SharedConstants::PROJECT_DATA] = defaultProj;
    SkeletonRecProject.append(projContainer);

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
    qDebug() << "Failed to create JSON doc:" << err.errorString();
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

QString ProjectManager::birdName() const { return m_birdName; }
QString ProjectManager::projectDir() const { return m_projectDir; }
QString ProjectManager::wavDir() const { return m_wavDir; }

QVariantList ProjectManager::recentProjects() const {
  return m_recentProjects.toVariantList();
}

bool ProjectManager::isInitialized() const { return m_isInitialized; }
bool ProjectManager::projectAttached() const { return m_projectAttached; }
QVariantMap ProjectManager::currentProjectData() const {
  return m_currentProjectData.toVariantMap();
}

void ProjectManager::importWavFiles(const QString &wavDir) {
  qDebug() << "Importing Wav Files from : " << wavDir << "\n";
}

void ProjectManager::initProject(const QString &projectDir,
                                 const QString &projectName,
                                 const QString &birdName) {

  if (VerifyMetaFileExistence(projectDir)) {
    emit error("Cannot create a project within another project!");
    return;
  }
  QStringList keys;
  QVariantList values;

  keys << constants::SharedConstants::PROJECT_NAME
       << constants::SharedConstants::PROJECT_DIR
       << constants::SharedConstants::PROJECT_BIRD_NAME
       << constants::SharedConstants::PROJECT_CREATE_DATE
       << constants::SharedConstants::PROJECT_LAST_ACCESSED;
  values << projectName << projectDir << birdName
         << QDateTime::currentDateTimeUtc().toString()
         << QDateTime::currentDateTimeUtc().toString();

  QString MetaFilePath = buildProjectMetaFile(projectDir);

  qDebug() << "Got to init projet\n";

  if (!modMetaFile(MetaFilePath, keys, values, true)) {
    emit error("Error in building .twty file");
  }
  // buildProjectDirectory(projectDir)
  loadProject(projectDir);
}

void ProjectManager::loadProject(const QString &projDir) {
  qDebug() << "Loading Project with config at: " << projDir << "\n";
  // check for metadatafile if doesnt exist throw
  // if found, update access date value, allocate class QJSONObject with project
  // data

  QString metaDataPath = getMetaFilePath(
      projDir, constants::SharedConstants::PROJECT_META_FILE_NAME);
  // find in recent projects, if in recent projects update last accessed date
  qDebug() << "metaDataPath: " << metaDataPath.toStdString() << "\n";
  QStringList keys;
  QVariantList vals;
  keys << constants::SharedConstants::PROJECT_LAST_ACCESSED;
  vals << QDateTime::currentDateTimeUtc().toString();
  modMetaFile(metaDataPath, keys, vals, false);

  QJsonObject metaData = extractMetaDataContent(metaDataPath);
  debug_printJsonObject(metaData);
  emit projectLoading(QString(metaData[constants::SharedConstants::PROJECT_DATA]
                                      [constants::SharedConstants::PROJECT_NAME]
                                          .toString()));
  updateRecentProjects(metaData);
  updateRecentProjectsFile();
  setCurrentProjectData(metaData);
  setProjectAttached(true);
  emit projectLoaded();
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
               << obj[constants::SharedConstants::PROJECT_DATA]
                     [constants::SharedConstants::PROJECT_DIR]
                         .toString()
               << "\n";
      loadProject(obj[constants::SharedConstants::PROJECT_DATA]
                     [constants::SharedConstants::PROJECT_DIR]
                         .toString());
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
      debug_printJsonObject(
          projData[constants::SharedConstants::PROJECT_DATA].toObject());
      debug_printJsonObject(
          obj[constants::SharedConstants::PROJECT_DATA].toObject());
      obj[constants::SharedConstants::PROJECT_DATA] =
          projData[constants::SharedConstants::PROJECT_DATA].toObject();

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

void ProjectManager::buildProjectDirectory(const QString &projdir) {
  QDir projDir(projdir);
  projDir.mkpath("Audio");
  projDir.mkpath("Cache");
  projDir.mkpath("Models");
  projDir.mkpath("Annotations");
  projDir.mkpath("Notes");
  return;
}

QString ProjectManager::buildProjectMetaFile(const QString &projDir) {
  QDir dir(projDir);
  assert(createMetaFile(projDir,
                        constants::SharedConstants::PROJECT_META_FILE_NAME));
  QJsonObject obj;
  QJsonObject container;

  QStringList keys;
  keys << constants::SharedConstants::PROJECT_ID
       << constants::SharedConstants::PROJECT_DATA;

  container[constants::SharedConstants::PROJECT_ID] =
      QUuid::createUuid().toString(QUuid::WithoutBraces);
  obj[constants::SharedConstants::PROJECT_DIR] = projDir;
  container[constants::SharedConstants::PROJECT_DATA] = obj;
  QVariantList values;
  values << QUuid::createUuid().toString(QUuid::WithoutBraces);
  values.append(container);

  modMetaFile(
      getMetaFilePath(m_projectDir,
                      constants::SharedConstants::PROJECT_META_FILE_NAME),
      keys, values, true);

  return dir.filePath(constants::SharedConstants::PROJECT_META_FILE_NAME);
}
