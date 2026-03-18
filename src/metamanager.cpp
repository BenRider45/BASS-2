#include "metamanager.h"
#include "sharedconstants.h"
#include <ranges>
namespace {

void debug_printJsonObject(QJsonObject obj) {
  QJsonDocument doc(obj);
  QString jsonString = doc.toJson();
  qDebug() << jsonString;
}

} // namespace

MetaManager::MetaManager(QDir directory, QString metaFileName) {

  _metaFileName =
      metaFileName + constants::SharedConstants::META_FILE_EXTENSION;
  _dir = directory;

  _metaFilePath = _dir.filePath(_metaFileName);

  if (!VerifyMetaFileExistence()) {
    createMetaFile(_metaFileName);
  }
}

bool MetaManager::VerifyMetaFileExistence() {
  return _dir.exists(_metaFileName);
}

QJsonObject MetaManager::extractMetaDataContent() {

  QByteArray data;

  qDebug() << "Extracting Meta Data from " << _metaFilePath;
  QFile file = QFile(_metaFilePath);
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

bool MetaManager::createMetaFile(QString metaFileName) {

  QDir dir(_dir);
  QJsonDocument doc;
  QFile file = QFile(dir.filePath(_metaFileName));

  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    qDebug() << "File written to " << dir.absolutePath() << "\n";
    file.close();
    return true;
  } else {
    return false;
  }
}

bool MetaManager::modMetaFile(const QStringList &keys,
                              const QVariantList &values,
                              bool addValuesIfNeeded) {

  qDebug() << "modMetaFile entered\n";
  assert(keys.length() == values.length());
  QJsonDocument doc;

  QFile file(_metaFilePath);

  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    doc = QJsonDocument::fromJson(data);
  }
  file.close();
  QJsonObject obj = doc.object();
  QJsonObject dataObj =
      obj[constants::SharedConstants::PROJECT_DATA].toObject();

  for (auto &&[k, v] : std::views::zip(keys, values)) {
    if (addValuesIfNeeded || dataObj.keys().contains(k)) {
      qDebug() << "Adding values " << v << "for keys " << k << "\n";
      dataObj[k] = v.toJsonValue();
    }
  }
  qDebug() << "OBJECT AFTER ADDING STUFF\n";
  debug_printJsonObject(dataObj);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  obj[constants::SharedConstants::PROJECT_DATA] = dataObj;
  QJsonDocument newDoc(obj);
  file.write(newDoc.toJson());
  file.close();
  return true;
}
