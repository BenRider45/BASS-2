#include "metamanager.h"
#include "sharedconstants.h"
#include <iostream>
#include <ranges>
namespace {

void debug_printJsonObject(QJsonObject obj) {
  QJsonDocument doc(obj);
  QString jsonString = doc.toJson();
  qDebug() << jsonString;
}

} // namespace

QJsonObject MetaManager::extractMetaDataContent(QString metaFilePath) {

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

bool MetaManager::createMetaFile(QDir directory, QString metaFileName) {

  QDir dir(directory);
  assert(!VerifyMetaFileExistence(directory));
  QJsonDocument doc;
  QFile file = QFile(dir.filePath(
      metaFileName + constants::SharedConstants::META_FILE_EXTENSION));

  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    qDebug() << "File written to " << dir.absolutePath() << "\n";
    file.close();
    return true;
  } else {
    return false;
  }
}

bool MetaManager::modMetaFile(QString metaFilePath, const QStringList &keys,
                              const QVariantList &values,
                              bool addValuesIfNeeded) {

  assert(keys.length() == values.length());
  QJsonDocument doc;

  QFile file(metaFilePath);

  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    doc = QJsonDocument::fromJson(data);
  }
  file.close();
  QJsonObject obj = doc.object();

  for (auto &&[k, v] : std::views::zip(keys, values)) {
    if (addValuesIfNeeded || obj.keys().contains(k)) {
      std::cerr << "Adding values " << v.toString().toCFString() << "for keys "
                << k.toCFString() << "\n";
      obj[k] = v.toJsonValue();
    }
  }
  qDebug() << "OBJECT AFTER ADDING STUFF\n";
  debug_printJsonObject(obj);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  QJsonDocument newDoc(obj);
  file.write(newDoc.toJson());
  file.close();
  return true;
}

bool MetaManager::VerifyMetaFileExistence(QDir directory) {
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

QString MetaManager::getMetaFilePath(QDir path, QString fileName) {
  // Get meta file path of specific name
  assert(
      path.exists(fileName + constants::SharedConstants::META_FILE_EXTENSION));
  return path.filePath(fileName +
                       constants::SharedConstants::META_FILE_EXTENSION);
}
