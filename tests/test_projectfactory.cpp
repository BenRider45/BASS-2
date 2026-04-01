#include "metamanager.h"
#include "sharedconstants.h"
#include <bassproject.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <projectFactory.h>
class projectFactoryTest : public testing::Test {
protected:
  void SetUp() override {
    tempDir = std::filesystem::temp_directory_path();

    folderName = "BASS_UNITTESTING_PROJECTFACTORY";
    QDir(tempDir.filePath(folderName)).removeRecursively();
    tempDir.mkdir(folderName);
    projDirPath = tempDir.filePath(folderName);
    std::cerr << "test dir: " << projDirPath.toStdString() << "\n";
    projUid = QUuid::createUuid();

    projMeta.projectDir = QDir(projDirPath);
    projMeta.projectID = projUid;
    testProjName = "TestProjName";
    birdName = "Tweety";

    lastaccessed = QDateTime::currentDateTimeUtc();
    projMeta.projectName = testProjName;
    projMeta.birdName = birdName;
    projMeta.lastAccessed = lastaccessed;
    projMeta.timeCreated = lastaccessed;
  }

  void TearDown() override {

    bool result = QDir(projDirPath).removeRecursively();
    std::cerr << "result: " << result << "\n";
    assert(result);
  }
  QDir tempDir;
  QUuid projUid;
  QString folderName;
  QString testProjName;
  QString birdName;
  QDateTime lastaccessed;
  QString projDirPath;
  bassproject::projectMetaPackage projMeta;
};

TEST_F(projectFactoryTest, makeProjectObjectFromNewDirectoryFormatResult) {

  std::unique_ptr<BassProject> proj =
      ProjectFactory::createProjectFromNew(projMeta);

  QStringList entryLst = projMeta.projectDir.entryList();

  for (const auto &entry : entryLst) {
    std::cerr << " Entry: " << entry.toStdString() << "\n";
  }
  int EXPECTED_NUMBER_OF_FILES = 6;
  EXPECT_EQ(entryLst.size(), EXPECTED_NUMBER_OF_FILES + 2);

  proj->updateprojectMetaPackage();

  EXPECT_EQ(proj->_projMetaData.projectDir, projMeta.projectDir);
  EXPECT_EQ(proj->_projMetaData.projectID, projMeta.projectID);
  EXPECT_EQ(proj->_projMetaData.timeCreated, projMeta.timeCreated);
  EXPECT_EQ(proj->_projMetaData.lastAccessed, projMeta.lastAccessed);
  EXPECT_EQ(proj->_projMetaData.projectName, projMeta.projectName);
  EXPECT_EQ(proj->_projMetaData.birdName, projMeta.birdName);
}

TEST_F(projectFactoryTest, makeProjectObjectFromNewDirectoryMetaDataResult) {

  // is metafile valid format? (can we extract all data)
  // is data in metafile correct?
  std::unique_ptr<BassProject> proj =
      ProjectFactory::createProjectFromNew(projMeta);

  bassproject::projectMetaPackage metaData = proj->_projMetaData;

  ASSERT_EQ(metaData.projectDir, projDirPath);
  ASSERT_EQ(metaData.projectID, projUid);
  ASSERT_EQ(metaData.projectName, testProjName);
  ASSERT_EQ(metaData.timeCreated, lastaccessed);
  ASSERT_EQ(metaData.timeCreated, lastaccessed);
  ASSERT_EQ(metaData.birdName, birdName);
}

TEST_F(projectFactoryTest, makeProjectObjectFromMetaData) {
  MetaManager::createMetaFile(
      projDirPath, constants::SharedConstants::PROJECT_META_FILE_NAME);
  QString metaFilePath = MetaManager::getMetaFilePath(
      projDirPath, constants::SharedConstants::PROJECT_META_FILE_NAME);

  MetaManager::writeData(metaFilePath, constants::SharedConstants::PROJECT_DIR,
                         projMeta.projectDir);
  MetaManager::writeData(metaFilePath, constants::SharedConstants::PROJECT_NAME,
                         projMeta.projectName);
  MetaManager::writeData(metaFilePath, constants::SharedConstants::PROJECT_ID,
                         projMeta.projectID);
  MetaManager::writeData(metaFilePath,
                         constants::SharedConstants::PROJECT_CREATE_DATE,
                         projMeta.timeCreated);
  MetaManager::writeData(metaFilePath,
                         constants::SharedConstants::PROJECT_LAST_ACCESSED,
                         projMeta.lastAccessed);
  MetaManager::writeData(metaFilePath,
                         constants::SharedConstants::PROJECT_BIRD_NAME,
                         projMeta.birdName);
  ASSERT_TRUE(MetaManager::VerifyMetaFileExistence(projDirPath));

  std::unique_ptr<BassProject> proj =
      ProjectFactory::createProjectFromMetaData(projDirPath);

  bassproject::projectMetaPackage metaData = proj->_projMetaData;

  ASSERT_TRUE(metaData.projectDir == projDirPath);
  ASSERT_TRUE(metaData.projectID == projUid);
  ASSERT_TRUE(metaData.projectName == testProjName);
  std::cerr << "metaData.timeCreated: "
            << metaData.timeCreated.time().toString().toStdString() << "\n";
  std::cerr << "metaData.lastaccessed: "
            << metaData.lastAccessed.time().toString().toStdString() << "\n";

  std::cerr << "lastaccessed: " << lastaccessed.time().toString().toStdString()
            << "\n";
  ASSERT_TRUE(metaData.timeCreated == lastaccessed);
  ASSERT_TRUE(metaData.lastAccessed == lastaccessed);
  ASSERT_TRUE(metaData.birdName == birdName);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
