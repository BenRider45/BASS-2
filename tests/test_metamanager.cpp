#include "metamanager.h"
#include <QDir>
#include <QStringList>
#include <gtest/gtest.h>
class TestMetaClass : public MetaManager {
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

  tmc.createMetaFile(projDirPath, tmc._MetaFileName);
  EXPECT_EQ(tmc.VerifyMetaFileExistence(projDirPath), true);
}

TEST_F(MetaManagerTest, readWriteItemstoMetaFile) {

  TestMetaClass tmc(5, "TestMetaName", "DirName");

  tmc.createMetaFile(projDirPath, tmc._MetaFileName);
  QString metaFilePath = tmc.getMetaFilePath(projDirPath, tmc._MetaFileName);

  QStringList keys;
  keys << "name" << "number";
  QVariantList values;
  values << tmc._MetaFileName << tmc._foo;

  tmc.modMetaFile(metaFilePath, keys, values, true);
  QJsonObject fileData = tmc.extractMetaDataContent(metaFilePath);

  QJsonDocument doc(fileData);

  QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
  std::cerr << jsonData.constData() << "\n";
  EXPECT_EQ(fileData.count(), 2);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
