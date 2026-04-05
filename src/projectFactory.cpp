#include "projectFactory.h"
#include "bassproject.h"
#include "metamanager.h"
#include "sharedconstants.h"
namespace ProjectFactory {
std::unique_ptr<BassProject> createProjectFromMetaData(QDir projectDirectory) {
  assert(MetaManager::VerifyMetaFileExistence(projectDirectory));

  QString metaFilePath = MetaManager::getMetaFilePath(
      projectDirectory, constants::SharedConstants::PROJECT_META_FILE_NAME);

  Result<QUuid> ID = MetaManager::retrieveData<QUuid>(
      metaFilePath, constants::SharedConstants::PROJECT_ID);

  Result<QDateTime> timeCreated = MetaManager::retrieveData<QDateTime>(
      metaFilePath, constants::SharedConstants::PROJECT_CREATE_DATE);

  Result<QDateTime> lastAccessed = MetaManager::retrieveData<QDateTime>(
      metaFilePath, constants::SharedConstants::PROJECT_LAST_ACCESSED);

  Result<QString> projectName = MetaManager::retrieveData<QString>(
      metaFilePath, constants::SharedConstants::PROJECT_NAME);

  Result<QString> birdName = MetaManager::retrieveData<QString>(
      metaFilePath, constants::SharedConstants::PROJECT_BIRD_NAME);
  bassproject::projectMetaPackage projMeta(
      projectDirectory, ID.value(), timeCreated.value(), lastAccessed.value(),
      projectName.value(), birdName.value());

  assert(ID.isSuccess() && timeCreated.isSuccess() &&
         lastAccessed.isSuccess() && projectName.isSuccess() &&
         birdName.isSuccess());

  std::unique_ptr<BassProject> output = std::make_unique<BassProject>(projMeta);
  return output;
}

std::unique_ptr<BassProject>
createProjectFromNew(bassproject::projectMetaPackage projMeta) {
  qDebug("Got to createProjectFromNew\n");

  qDebug() << "projMeta.projectDir = " << projMeta.projectDir << "\n";
  assert(!MetaManager::VerifyMetaFileExistence(projMeta.projectDir));

  MetaManager::createMetaFile(
      projMeta.projectDir, constants::SharedConstants::PROJECT_META_FILE_NAME);

  assert(MetaManager::VerifyMetaFileExistence(projMeta.projectDir));
  qDebug("metaFileCreated\n");
  QString metaFilePath = MetaManager::getMetaFilePath(
      projMeta.projectDir, constants::SharedConstants::PROJECT_META_FILE_NAME);

  buildProjectDirectoryStructure(projMeta.projectDir);

  //  QStringList keys;
  //  keys << constants::SharedConstants::PROJECT_DIR
  //       << constants::SharedConstants::PROJECT_ID
  //       << constants::SharedConstants::PROJECT_CREATE_DATE
  //       << constants::SharedConstants::PROJECT_LAST_ACCESSED
  //       << constants::SharedConstants::PROJECT_NAME
  //       << constants::SharedConstants::PROJECT_BIRD_NAME;
  //
  //  QVariantList values;
  //  values << projMeta.projectDir.path() << projMeta.projectID
  //         << projMeta.timeCreated << projMeta.lastAccessed
  //         << QString(projMeta.projectName) << QString(projMeta.birdName);
  //  qDebug() << "In createProjectFromNew:\n";
  //  qDebug() << "Project name: " << projMeta.projectName.toStdString()
  //           << "Bird Name : " << projMeta.birdName.toStdString();

  MetaManager::writeData<QString>(metaFilePath,
                                  constants::SharedConstants::PROJECT_DIR,
                                  projMeta.projectDir.path());

  MetaManager::writeData<QUuid>(
      metaFilePath, constants::SharedConstants::PROJECT_ID, projMeta.projectID);

  MetaManager::writeData<QDateTime>(
      metaFilePath, constants::SharedConstants::PROJECT_CREATE_DATE,
      projMeta.timeCreated);

  MetaManager::writeData<QDateTime>(
      metaFilePath, constants::SharedConstants::PROJECT_LAST_ACCESSED,
      projMeta.lastAccessed);

  MetaManager::writeData<QString>(metaFilePath,
                                  constants::SharedConstants::PROJECT_NAME,
                                  QString(projMeta.projectName));

  MetaManager::writeData<QString>(metaFilePath,
                                  constants::SharedConstants::PROJECT_BIRD_NAME,
                                  QString(projMeta.birdName));

  return std::make_unique<BassProject>(projMeta);
}

void buildProjectDirectoryStructure(QDir projectDirectory) {
  projectDirectory.mkpath("Audio");
  projectDirectory.mkpath("Cache");
  projectDirectory.mkpath("Models");
  projectDirectory.mkpath("Annotations");
  projectDirectory.mkpath("Notes");
  return;
}

} // namespace ProjectFactory
