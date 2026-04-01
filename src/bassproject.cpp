#include "bassproject.h"
#include "metamanager.h"
#include "result.h"
#include "sharedconstants.h"
#include <QDateTime>

BassProject::BassProject(bassproject::projectMetaPackage metaPackage)
    : _projMetaData(metaPackage), _defaultConstructed(false) {
  // Do initializtion of all sub-modules here
}

BassProject::BassProject() : _defaultConstructed(true) {}

bool BassProject::wasDefaultConstructed() { return _defaultConstructed; }

void BassProject::updateprojectMetaPackage() {
  QDir origProjDir = _projMetaData.projectDir;

  Result<QString> projDir = MetaManager::retrieveData<QString>(
      _projMetaData.projectDir.absolutePath(),
      constants::SharedConstants::PROJECT_DIR);

  Result<QUuid> projID =
      MetaManager::retrieveData<QUuid>(_projMetaData.projectDir.absolutePath(),
                                       constants::SharedConstants::PROJECT_ID);

  Result<QDateTime> projTimeCreated = MetaManager::retrieveData<QDateTime>(
      _projMetaData.projectDir.absolutePath(),
      constants::SharedConstants::PROJECT_CREATE_DATE);

  Result<QDateTime> projLastAccessed = MetaManager::retrieveData<QDateTime>(
      _projMetaData.projectDir.absolutePath(),
      constants::SharedConstants::PROJECT_DIR);

  Result<QString> projName = MetaManager::retrieveData<QString>(
      _projMetaData.projectDir.absolutePath(),
      constants::SharedConstants::PROJECT_NAME);

  Result<QString> projBirdName = MetaManager::retrieveData<QString>(
      _projMetaData.projectDir.absolutePath(),
      constants::SharedConstants::PROJECT_BIRD_NAME);

  if (!projDir.isSuccess() || !projID.isSuccess() ||
      !projTimeCreated.isSuccess() || !projLastAccessed.isSuccess() ||
      !projName.isSuccess() || !projBirdName.isSuccess()) {
    qDebug()
        << "ERROR: Could not read Project Directory for project in directory: "
        << origProjDir.absolutePath() << "\n";
    return;
  } else {
    _projMetaData.projectDir = QDir(projDir.value());
    _projMetaData.projectID = QUuid(projID.value());
    _projMetaData.timeCreated = QDateTime(projTimeCreated.value());
    _projMetaData.lastAccessed = QDateTime(projLastAccessed.value());
    _projMetaData.projectName = QString(projName.value());
    _projMetaData.birdName = QString(projBirdName.value());
  }
}
