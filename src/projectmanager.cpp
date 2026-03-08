#include "projectmanager.h"
#include <QDateTime>
#include <QDir>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QUuid>
#include <ranges>

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
    projContainer[PROJECT_ID] =
        QUuid::createUuid().toString(QUuid::WithoutBraces);
    QJsonObject defaultProj;

    projContainer[PROJECT_DATA] = defaultProj;
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

  if (findMetaFile(projectDir)) {
    emit error("Cannot create a project within another project!");
    return;
  }
  QStringList keys;
  QStringList values;

  keys << PROJECT_NAME << PROJECT_DIR << PROJECT_BIRD_NAME
       << PROJECT_CREATE_DATE << PROJECT_LAST_ACCESSED;
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

  QString metaDataPath = getMetaDataPath(projDir);
  // find in recent projects, if in recent projects update last accessed date
  QStringList keys;
  QStringList vals;
  keys << PROJECT_LAST_ACCESSED;
  vals << QDateTime::currentDateTimeUtc().toString();
  modMetaFile(metaDataPath, keys, vals, false);

  QJsonObject metaData = extractMetaDataContent(metaDataPath);

  emit projectLoading(QString(metaData[PROJECT_DATA][PROJECT_NAME].toString()));
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
    if (QUuid(obj[PROJECT_ID].toString()) == QUuid(UID)) {
      qDebug() << "Found project " << UID << "in recent projects\n"
               << "Project lives at "
               << obj[PROJECT_DATA][PROJECT_DIR].toString() << "\n";
      loadProject(obj[PROJECT_DATA][PROJECT_DIR].toString());
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
    if (projData[PROJECT_ID].toString() == obj[PROJECT_ID].toString()) {
      valueFound = true;
      qDebug("Updateing project!!!");
      debug_printJsonObject(projData[PROJECT_DATA].toObject());
      debug_printJsonObject(obj[PROJECT_DATA].toObject());
      obj[PROJECT_DATA] = projData[PROJECT_DATA].toObject();

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

bool ProjectManager::modMetaFile(const QString &path, const QStringList &key,
                                 const QStringList &value,
                                 bool addValueIfNeeded) {
  qDebug() << "modMetaFile entered\n";
  if (key.length() != value.length()) {
    emit error("Number of keys and values must be the same");
    return false;
  }
  QJsonDocument doc;

  QFile file(path);
  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    doc = QJsonDocument::fromJson(data);
  }
  file.close();
  QJsonObject obj = doc.object();
  QJsonObject dataObj = obj[PROJECT_DATA].toObject();

  qDebug() << "OBJECT BEFORE ADDING STUFF\n";
  debug_printJsonObject(dataObj);
  for (auto &&[k, v] : std::views::zip(key, value)) {
    if (addValueIfNeeded || dataObj.keys().contains(k)) {
      qDebug() << "Adding Value " << v << "for key " << k << "\n";
      dataObj[k] = v;
    }
  }
  qDebug() << "OBJECT AFTER ADDING STUFF\n";
  debug_printJsonObject(dataObj);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  obj[PROJECT_DATA] = dataObj;
  QJsonDocument newDoc(obj);
  file.write(newDoc.toJson());
  file.close();
  return true;
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

bool ProjectManager::findMetaFile(const QString &projDir) {
  QDir dir(projDir);
  return dir.exists(META_FILE_NAME);
}

QString ProjectManager::buildProjectMetaFile(const QString &projDir) {
  QDir dir(projDir);
  QJsonObject obj;
  QJsonObject container;
  container[PROJECT_ID] = QUuid::createUuid().toString(QUuid::WithoutBraces);
  obj[PROJECT_DIR] = projDir;
  container[PROJECT_DATA] = obj;
  QJsonDocument doc(container);
  QFile file = QFile(dir.filePath(META_FILE_NAME));
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    qDebug() << "File written to " << dir.absolutePath() << "\n";
  } else {
    emit error("Failed building meta file");
  }

  file.close();
  return dir.filePath(META_FILE_NAME);
}

QString ProjectManager::getMetaDataPath(const QString &projDir) {
  QByteArray data;
  //  if (!dir.exists(META_FILE_NAME)) {
  //    emit error("Cannot find Metadata file for project" + metaFilePath);
  //  }
  QDir dir(projDir);
  QString metaPath = dir.filePath(META_FILE_NAME);
  //  QFile file = QFile(metaFilePath);
  //  if (file.open(QIODevice::ReadOnly)) {
  //    QByteArray data = file.readAll();
  //    QJsonDocument doc = QJsonDocument::fromJson(data);
  //    return doc.object();
  //  } else {
  //    emit error("Could not open Meta File");
  //    return QJsonObject();
  //  }
  return dir.exists(metaPath) ? metaPath : QString();
}

QJsonObject
ProjectManager::extractMetaDataContent(const QString &metaFilePath) {
  QByteArray data;
  qDebug() << "Extracting Meta Data from " << metaFilePath;
  QFile file = QFile(metaFilePath);
  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
  } else {
    emit error("Could not open Meta File :(");
    return QJsonObject();
  }
}
