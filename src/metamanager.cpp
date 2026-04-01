#include "metamanager.h"
#include "sharedconstants.h"
#include <QVariant>
#include <iostream>
namespace {

void debug_printJsonObject(QJsonObject obj) {
  QJsonDocument doc(obj);
  QString jsonString = doc.toJson();
  qDebug() << jsonString;
}

} // namespace
namespace MetaManager {

QJsonObject extractMetaDataContent(QString metaFilePath) {

  QByteArray data;
  qDebug() << "Extracting Meta Data from " << metaFilePath;
  QFile file = QFile(metaFilePath);
  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
  } else {
    QJsonObject errOut;
    errOut["ERROR"] = "ERROR OPENING META FILE";
    return errOut;
  }
}

bool createMetaFile(QDir directory, QString metaFileName) {

  QDir dir(directory);
  assert(!VerifyMetaFileExistence(directory));
  qDebug() << "attempting to createMetaFile with name: "
           << metaFileName + constants::SharedConstants::META_FILE_EXTENSION
           << "\n at directory: " << directory.absolutePath();
  QJsonObject emptyObj;
  QJsonDocument doc(emptyObj);
  qDebug() << "dir.filePath returns"
           << dir.filePath(metaFileName +
                           constants ::SharedConstants::META_FILE_EXTENSION)
           << "\n";
  QFile file = QFile(dir.filePath(
      metaFileName + constants::SharedConstants::META_FILE_EXTENSION));

  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    qDebug() << "File written to " << dir.absolutePath() << "\n";
    file.close();
    return true;
  } else {
    qDebug() << "ERROR: Could not open file (in MetaManager::createMetaFile)\n";
    return false;
  }
}

bool VerifyMetaFileExistence(QDir directory) {
  QStringList lst;
  lst << "*" + constants::SharedConstants::META_FILE_EXTENSION;
  std::cerr << "lst: " << lst[0].toStdString() << "\n";
  directory.setNameFilters(lst);
  QStringList entryLst = directory.entryList();

  std::cerr << "entryLst.length(): " << entryLst.length() << "\n";
  for (auto item : entryLst) {
    std::cerr << "Item: " << item.toStdString() << "\n";
  }
  return entryLst.length() > 0;
}

QString getMetaFilePath(QDir path, QString fileName) {
  // Get meta file path of specific name
  std::cerr << "Getting file with name : " << fileName.toStdString()
            << "\n From path : " << path.absolutePath().toStdString() << "\n";
  assert(
      path.exists(fileName + constants::SharedConstants::META_FILE_EXTENSION));
  return path.filePath(fileName +
                       constants::SharedConstants::META_FILE_EXTENSION);
}

} // namespace MetaManager
  //
