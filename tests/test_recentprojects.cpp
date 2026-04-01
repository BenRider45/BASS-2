#include "metamanager.h"
#include "recentProject.h"
#include <QDateTime>
#include <QDir>
#include <QUuid>
#include <gtest/gtest.h>

class RecentProjectsTest : public testing::Test {
protected:
  void SetUp() override {
    QDir tempDir = std::filesystem::temp_directory_path();
    folderName = "BASS_UNITTESTING_RECENTPROJECTS";
    QDir(tempDir.filePath(folderName)).removeRecursively();
    tempDir.mkdir(folderName);
    projDirPath = tempDir.filePath(folderName);
  }

  void TearDown() override {
    bool result = QDir(projDirPath).removeRecursively();
    ASSERT_TRUE(result) << "Failed to clean up test directory";
  }

  QString folderName;
  QString projDirPath;
};

// Test basic RecentProject object creation and destruction
TEST_F(RecentProjectsTest, CreateAndDestroyRecentProject) {
  QDir projectDir(projDirPath + "/TestProject");
  QDateTime accessTime = QDateTime::currentDateTime();
  QString projectName = "Test Project";
  QUuid projectID = QUuid::createUuid();

  {
    RecentProject project(projectName, projectID, accessTime, projectDir);
    EXPECT_EQ(project._projectName, projectName);
    EXPECT_EQ(project._projectID, projectID);
    EXPECT_EQ(project._projectDir.path(), projectDir.path());
    EXPECT_EQ(project._lastAccessed, accessTime);
    EXPECT_FALSE(project._isDefault);
  }
}

// Test default constructor
TEST_F(RecentProjectsTest, DefaultConstructor) {
  RecentProject project;
  EXPECT_TRUE(project._isDefault);
  EXPECT_TRUE(project._projectName.isEmpty());
  EXPECT_TRUE(project._projectID.isNull());
  EXPECT_TRUE(project._lastAccessed.isNull());
}

// Test copy constructor
TEST_F(RecentProjectsTest, CopyConstructor) {
  QDir projectDir(projDirPath + "/CopyTest");
  QDateTime accessTime = QDateTime::currentDateTime();
  QString projectName = "Copy Test Project";
  QUuid projectID = QUuid::createUuid();

  RecentProject original(projectName, projectID, accessTime, projectDir);
  RecentProject copy(original);

  EXPECT_EQ(copy._projectName, original._projectName);
  EXPECT_EQ(copy._projectID, original._projectID);
  EXPECT_EQ(copy._projectDir.path(), original._projectDir.path());
  EXPECT_EQ(copy._lastAccessed, original._lastAccessed);
  EXPECT_FALSE(copy._isDefault);
}

// Test writing and reading a single RecentProject
TEST_F(RecentProjectsTest, WriteAndReadSingleRecentProject) {
  QDir projectDir(projDirPath + "/SingleProject");
  QDateTime accessTime = QDateTime::currentDateTime();
  QString projectName = "Single Project";
  QUuid projectID = QUuid::createUuid();

  RecentProject project(projectName, projectID, accessTime, projectDir);

  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  Result<bool> writeRes =
      MetaManager::writeData<RecentProject>(metaFilePath, "project1", project);
  EXPECT_TRUE(writeRes.isSuccess());

  Result<RecentProject> readRes =
      MetaManager::retrieveData<RecentProject>(metaFilePath, "project1");
  EXPECT_TRUE(readRes.isSuccess());

  RecentProject readProject = readRes.value();
  EXPECT_EQ(readProject._projectName, projectName);
  EXPECT_EQ(readProject._projectID, projectID);
  EXPECT_EQ(readProject._projectDir.path(), projectDir.path());
  EXPECT_EQ(readProject._lastAccessed.toString(), accessTime.toString());
}

TEST_F(RecentProjectsTest, WriteAndReadMultipleRecentProjects) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QVector<RecentProject> projects;
  for (int i = 0; i < 5; ++i) {
    QString name = QString("Project_%1").arg(i);
    QUuid id = QUuid::createUuid();
    QDateTime time = QDateTime::currentDateTime().addDays(-i);
    QDir dir(projDirPath + QString("/Project_%1").arg(i));

    projects.append(RecentProject(name, id, time, dir));
  }

  for (int i = 0; i < projects.size(); ++i) {
    QString key = QString("project_%1").arg(i);
    Result<bool> writeRes =
        MetaManager::writeData<RecentProject>(metaFilePath, key, projects[i]);
    EXPECT_TRUE(writeRes.isSuccess()) << "Failed to write project " << i << ": "
                                      << writeRes.error().toStdString();
  }

  for (int i = 0; i < projects.size(); ++i) {
    QString key = QString("project_%1").arg(i);
    Result<RecentProject> readRes =
        MetaManager::retrieveData<RecentProject>(metaFilePath, key);
    EXPECT_TRUE(readRes.isSuccess()) << "Failed to read project " << i << ": "
                                     << readRes.error().toStdString();

    RecentProject readProject = readRes.value();
    EXPECT_EQ(readProject._projectName, projects[i]._projectName);
    EXPECT_EQ(readProject._projectID, projects[i]._projectID);
    EXPECT_EQ(readProject._projectDir.path(), projects[i]._projectDir.path());
  }
}

// Test extracting a list of recent projects
TEST_F(RecentProjectsTest, ExtractRecentProjectListFromFile) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  // Create and write projects
  QVector<RecentProject> originalProjects;
  for (int i = 0; i < 3; ++i) {
    QString name = QString("ExtractTest_Project_%1").arg(i);
    QUuid id = QUuid::createUuid();
    QDateTime time = QDateTime::currentDateTime().addSecs(-i * 3600);
    QDir dir(projDirPath + QString("/ExtractTest_%1").arg(i));

    RecentProject proj(name, id, time, dir);
    originalProjects.append(proj);

    QString key = QString("project_%1").arg(i);
    MetaManager::writeData<RecentProject>(metaFilePath, key, proj);
  }

  // Extract the list
  Result<QVector<RecentProject>> extractedList =
      MetaManager::RecentProjectUtils::extractRecentProjectList(metaFilePath);
  EXPECT_TRUE(extractedList.isSuccess());
  EXPECT_EQ(extractedList.value().size(), originalProjects.size());

  // Verify each project in the list
  for (int i = 0; i < extractedList.value().size(); ++i) {
    RecentProject extracted = extractedList.value().at(i);
    EXPECT_EQ(extracted._projectName, originalProjects[i]._projectName);
    EXPECT_EQ(extracted._projectID, originalProjects[i]._projectID);
    EXPECT_EQ(extracted._projectDir.path(),
              originalProjects[i]._projectDir.path());
  }
}

// Test that modifying a read RecentProject doesn't affect the stored one
TEST_F(RecentProjectsTest, IndependentCopyAfterRead) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QString originalName = "Original Project";
  QUuid originalID = QUuid::createUuid();
  QDir originalDir(projDirPath + "/Original");
  QDateTime originalTime = QDateTime::currentDateTime();

  RecentProject original(originalName, originalID, originalTime, originalDir);
  MetaManager::writeData<RecentProject>(metaFilePath, "project", original);

  // Read it back
  Result<RecentProject> readRes =
      MetaManager::retrieveData<RecentProject>(metaFilePath, "project");
  EXPECT_TRUE(readRes.isSuccess());

  RecentProject readCopy = readRes.value();

  // Modify the read copy
  readCopy._projectName = "Modified Name";

  // Read again and verify original is unchanged
  Result<RecentProject> readRes2 =
      MetaManager::retrieveData<RecentProject>(metaFilePath, "project");
  EXPECT_TRUE(readRes2.isSuccess());
  EXPECT_EQ(readRes2.value()._projectName, originalName);
  EXPECT_NE(readRes2.value()._projectName, "Modified Name");
}

// Test with empty project directory
TEST_F(RecentProjectsTest, EmptyProjectDirectory) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  // Extract from empty file
  Result<QVector<RecentProject>> extractedList =
      MetaManager::RecentProjectUtils::extractRecentProjectList(metaFilePath);

  EXPECT_TRUE(extractedList.isSuccess());
  EXPECT_EQ(extractedList.value().size(), 0);
}

// Test overwriting an existing recent project
TEST_F(RecentProjectsTest, OverwriteRecentProject) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QString key = "project";

  // Write first version
  RecentProject first("First Version", QUuid::createUuid(),
                      QDateTime::currentDateTime(),
                      QDir(projDirPath + "/First"));
  MetaManager::writeData<RecentProject>(metaFilePath, key, first);

  // Overwrite with second version
  RecentProject second("Second Version", QUuid::createUuid(),
                       QDateTime::currentDateTime(),
                       QDir(projDirPath + "/Second"));
  MetaManager::writeData<RecentProject>(metaFilePath, key, second);

  // Verify second version is stored
  Result<RecentProject> readRes =
      MetaManager::retrieveData<RecentProject>(metaFilePath, key);
  EXPECT_TRUE(readRes.isSuccess());
  EXPECT_EQ(readRes.value()._projectName, "Second Version");
}

// Test RecentProject with special characters in name
TEST_F(RecentProjectsTest, SpecialCharactersInProjectName) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QString specialName = "Project-With_Special.Chars (v2)";
  QUuid id = QUuid::createUuid();
  QDateTime time = QDateTime::currentDateTime();
  QDir dir(projDirPath + "/Special");

  RecentProject project(specialName, id, time, dir);
  MetaManager::writeData<RecentProject>(metaFilePath, "special", project);

  Result<RecentProject> readRes =
      MetaManager::retrieveData<RecentProject>(metaFilePath, "special");
  EXPECT_TRUE(readRes.isSuccess());
  EXPECT_EQ(readRes.value()._projectName, specialName);
}

TEST_F(RecentProjectsTest, MetaManagerUtilQVectorList) {
  QString metaFileName = "recent_projects";
  MetaManager::createMetaFile(projDirPath, metaFileName);
  QString metaFilePath =
      MetaManager::getMetaFilePath(projDirPath, metaFileName);

  QVector<RecentProject> projects;
  for (int i = 0; i < 5; ++i) {
    QString name = QString("Project_%1").arg(i);
    QUuid id = QUuid::createUuid();
    QDateTime time = QDateTime::currentDateTime().addDays(-i);
    QDir dir(projDirPath + QString("/Project_%1").arg(i));

    projects.append(RecentProject(name, id, time, dir));
  }

  for (int i = 0; i < projects.size(); ++i) {
    QString key = QString("project_%1").arg(i);
    Result<bool> writeRes =
        MetaManager::writeData<RecentProject>(metaFilePath, key, projects[i]);
    EXPECT_TRUE(writeRes.isSuccess()) << "Failed to write project " << i << ": "
                                      << writeRes.error().toStdString();
  }

  Result<QVector<RecentProject>> recent_projs =
      MetaManager::RecentProjectUtils::extractRecentProjectList(metaFilePath);

  EXPECT_TRUE(recent_projs.isSuccess());
}
