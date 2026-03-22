#include "metamanager.h"
#include <QDir>
#include <QStringList>
#include <gtest/gtest.h>

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

TEST_F(MetaManagerTest, readWriteItemstoMetaFile) {

  TestMetaClass tmc(5, "TestMetaName", "DirName");

  MetaManager::createMetaFile(projDirPath, tmc._MetaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, tmc._MetaFileName);

  QStringList keys;
  keys << "name" << "number";
  QVariantList values;
  values << tmc._MetaFileName << tmc._foo;

  MetaManager::modMetaFile(metaFilePath, keys, values, true);
  QJsonObject fileData = MetaManager::extractMetaDataContent(metaFilePath);

  QJsonDocument doc(fileData);

  QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
  std::cerr << jsonData.constData() << "\n";
  EXPECT_EQ(fileData.count(), 2);
}

TEST_F(MetaManagerTest, readWriteArraytoMetaFile) {
  QStringList thingsToWrite = {"Things", "to", "Write", "into", "One", "Array"};
  QString metaFileName = "meta_file";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QStringList keys = {"Array"};
  QVariantList values = {thingsToWrite};

  MetaManager::modMetaFile(metaFilePath, keys, values, true);

  MetaManager::RetrievedObject<QStringList> array =
      MetaManager::retrieveData<QStringList>(metaFilePath, keys[0]);
  EXPECT_TRUE(array.success);
  EXPECT_EQ(array.obj.length(), thingsToWrite.length());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
