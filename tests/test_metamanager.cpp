#include "metamanager.h"
#include <QDir>
#include <gtest/gtest.h>
class MetaManagerTest : public testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(MetaManagerTest, initsFromstdFilesystem) {

  QDir tempDir = std::filesystem::temp_directory_path();

  MetaManager test(tempDir, QString("file_name"));
  EXPECT_EQ(test.VerifyMetaFileExistence(), true);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
