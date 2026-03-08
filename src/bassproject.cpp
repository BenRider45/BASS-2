#include "bassproject.h"
#include <QDateTime>
BassProject::BassProject(std::filesystem::path projDir, QString projName,
                         QObject *parent)
    : QObject(parent) {
  projectMetaData["proj_name"] = projName;
  projectMetaData["proj_directory"] = QString::fromUtf8(projDir.u8string());
  projectMetaData["date_created"] = QDateTime::currentDateTimeUtc().toString();

  initProject();
}

void BassProject::initProject() {
  // writes initial json bytedocument, creates directory structure, ensures
  // folder with at least one wav files exists within project
  //
  //
  //
  bool Successful = false;

  if (Successful) {
    emit projectInitialized(projectMetaData);
  }
}
