#include "metamanager.h"
#include "recentProject.h"
#include <QDir>
#include <QStringList>
#include <gtest/gtest.h>
#include <iostream>
class TestMetaClass {
public:
  TestMetaClass(int foo, QString MetaFilename, QString DirName)
      : _foo(foo), _MetaFileName(MetaFilename), _DirName(DirName) {}

  int _foo;
  QString _MetaFileName;
  QString _DirName;
};
class MetaManagerTest : public testing::Test {
protected:
  void SetUp() override {
    std::cerr << "Got to setup" << "\n";
    QDir tempDir = std::filesystem::temp_directory_path();

    folderName = "BASS_UNITTESTING_METAMANAGER";
    QDir(tempDir.filePath(folderName)).removeRecursively();
    tempDir.mkdir(folderName);
    projDirPath = tempDir.filePath(folderName);
    std::cerr << "projDirPath: " << projDirPath.toStdString() << "\n";
  }

  void TearDown() override {

    bool result = QDir(projDirPath).removeRecursively();
    std::cerr << "result: " << result << "\n";
    assert(result);
  }
  QString folderName;
  QDir tempDir;
  QString projDirPath;
};

TEST_F(MetaManagerTest, initsFromstdFilesystem) {
  const ::testing::TestInfo *const test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  const char *test_name = test_info->name();

  TestMetaClass tmc(5, test_name, "DirName");

  MetaManager::createMetaFile(projDirPath, tmc._MetaFileName);
  EXPECT_EQ(MetaManager::VerifyMetaFileExistence(projDirPath), true);
}

TEST_F(MetaManagerTest, readWriteArraytoMetaFile) {
  QStringList thingsToWrite = {"Things", "to", "Write", "into", "One", "Array"};
  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QStringList keys = {"Array"};
  QVariantList values = {thingsToWrite};

  Result<bool> writeRes =
      MetaManager::writeData<QStringList>(metaFilePath, "Array", thingsToWrite);

  if (!writeRes.isSuccess()) {
    std::cerr << writeRes.error().toStdString();
  } else {
    std::cerr << "writeRes.isSuccess()\n";
  }
  Result<QStringList> array =
      MetaManager::retrieveData<QStringList>(metaFilePath, keys[0]);
  if (!array.isSuccess()) {
    std::cerr << array.error().toStdString();
  }
  EXPECT_TRUE(array.isSuccess());
  EXPECT_EQ(array.value().length(), thingsToWrite.length());
}

TEST_F(MetaManagerTest, readWriteQDir) {
  QDir dir(QDir(projDirPath).filePath("Test/Path/dirPath"));

  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QStringList keys = {"Dir"};
  QVariantList values = {dir.path()};

  MetaManager::writeData<QDir>(metaFilePath, "Dir", dir);

  Result<QDir> dirresult =
      MetaManager::retrieveData<QDir>(metaFilePath, keys[0]);

  EXPECT_TRUE(dirresult.isSuccess());
  qDebug() << dir.path();
  qDebug() << dirresult.value().path();
  EXPECT_TRUE(dirresult.value().path() == dir.path());
}

TEST_F(MetaManagerTest, readWriteQUuid) {
  QUuid id = QUuid::createUuid();
  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QString key = "id";
  Result<bool> writeRes = MetaManager::writeData<QUuid>(metaFilePath, key, id);

  EXPECT_TRUE(writeRes.isSuccess());
  Result<QUuid> idResult = MetaManager::retrieveData<QUuid>(metaFilePath, key);
  EXPECT_TRUE(idResult.isSuccess());
  EXPECT_TRUE(id == idResult.value());
}

TEST_F(MetaManagerTest, readWriteQString) {
  QString str("String!");
  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QString key = "str";
  Result<bool> writeRes =
      MetaManager::writeData<QString>(metaFilePath, key, str);

  EXPECT_TRUE(writeRes.isSuccess());
  Result<QString> strResult =
      MetaManager::retrieveData<QString>(metaFilePath, key);

  EXPECT_TRUE(strResult.isSuccess());
  EXPECT_TRUE(str == strResult.value());
}

TEST_F(MetaManagerTest, readWriteQDateTime) {
  QDateTime date = QDateTime::currentDateTime();
  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString key = "date";

  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);
  Result<bool> writeRes =
      MetaManager::writeData<QDateTime>(metaFilePath, key, date);

  EXPECT_TRUE(writeRes.isSuccess());

  Result<QDateTime> dateResult =
      MetaManager::retrieveData<QDateTime>(metaFilePath, key);

  EXPECT_TRUE(dateResult.isSuccess());
  std::cerr << dateResult.value().toString().toStdString() << "\n";
  std::cerr << date.toString().toStdString() << "\n";
  EXPECT_TRUE(dateResult.value().toString() == date.toString());
}

TEST_F(MetaManagerTest, readWriteRecentProject) {
  QDir dir("Test/Directory/yeah");
  QDateTime time = QDateTime::currentDateTime();
  QString name = "Awesome Test Project";
  QUuid uid = QUuid::createUuid();

  RecentProject recentProj(name, uid, time, dir);
  QString metaFilename = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFilename);

  QString key = "RecentProject";
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFilename);
  Result<bool> writeRes =
      MetaManager::writeData<RecentProject>(metaFilePath, key, recentProj);

  EXPECT_TRUE(writeRes.isSuccess());

  Result<RecentProject> recentProjResult =
      MetaManager::retrieveData<RecentProject>(metaFilePath, key);

  EXPECT_TRUE(recentProjResult.isSuccess());

  EXPECT_TRUE(recentProjResult.value()._projectDir == dir);
  EXPECT_TRUE(recentProjResult.value()._projectID == uid);
  EXPECT_TRUE(recentProjResult.value()._projectName == name);
  EXPECT_TRUE(recentProjResult.value()._lastAccessed == time);
}
