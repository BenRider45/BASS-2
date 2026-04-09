#include "bassproject.h"
#include "projectFactory.h"
#include "sharedconstants.h"
#include "wavsurfer.h"
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
class WavSurferTest : public testing::Test {
protected:
  void SetUp() override {
    QDir tempDir = std::filesystem::temp_directory_path();
    QDir testPath(std::filesystem::current_path());
    folderName = "BASS_UNITTESTING_WAVSURFER";

    std::cerr << "Current Path: " << testPath.absolutePath().toStdString()
              << "\n";
    tempDir.mkdir(folderName);

    projDirPath = tempDir.filePath(folderName);
    testDataDir = testPath;
    testDataDir.cdUp();
    testDataDir.cdUp();
    assert(testDataDir.cd(QString("tests/test_data/wavsurfer")));

    testWavDataPath = testDataDir.absolutePath();

    QStringList lst;
    lst << "*" + QString(".wav");
    testDataDir.setNameFilters(lst);
    testDataEntryList = testDataDir.entryList();

    for (const auto &entry : testDataEntryList) {
      std::cerr << entry.toStdString() << "\n";
    }
  }

  void TearDown() override {
    bool result = QDir(projDirPath).removeRecursively();
    assert(result);
  }

  QString folderName;
  QString testWavDataPath;
  QString projDirPath;
  QDir testDataDir;
  QStringList testDataEntryList;
};

TEST_F(WavSurferTest, moveWavFilesFromDirectoryToProject) {
  bassproject::projectMetaPackage projMeta;
  projMeta.projectDir = projDirPath;
  projMeta.projectID = QUuid::createUuid();
  projMeta.timeCreated = QDateTime::currentDateTime();
  projMeta.lastAccessed = QDateTime::currentDateTime();
  projMeta.projectName = "Test_Project";
  projMeta.birdName = "Tweety";

  std::unique_ptr<BassProject> proj =
      ProjectFactory::createProjectFromNew(projMeta);

  WavSurfer<SharedTypeDefs::WAVFILE_SAMPLE> surfer;

  surfer.ImportWavFilesIntoProject(testWavDataPath, projDirPath);

  QVector<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> fileLst =
      surfer.RetrieveWavFiles(projDirPath);
  ASSERT_TRUE(testDataEntryList.size() > 0);

  ASSERT_EQ(fileLst.size(), testDataEntryList.size());
}
