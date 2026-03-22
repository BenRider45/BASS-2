#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

namespace MetaManager {
//  MetaManager(MetaManager &&) = default;
//  MetaManager(const MetaManager &) = default;
//  MetaManager &operator=(MetaManager &&) = default;
//  MetaManager &operator=(const MetaManager &) = default;
//  ~MetaManager();

template <typename T>
concept Variantable = requires(T a) { std::is_convertible<QVariant, T>(); };

template <typename T> struct RetrievedObject {
  T obj;
  bool success;
};

bool VerifyMetaFileExistence(QDir dir);
QJsonObject extractMetaDataContent(QString metaFilePath);
QString getMetaFilePath(QDir path, QString metaFileName);
bool modMetaFile(QString metaFilePath, const QStringList &keys,
                 const QVariantList &values, bool addValuesIfNeeded = false);

bool createMetaFile(QDir directory, QString metaFileName);

template <Variantable T>
RetrievedObject<T> retrieveData(QString MetaFilePath, QString key) {
  QJsonObject metaData = extractMetaDataContent(MetaFilePath);
  bool success = metaData.keys().contains(key);
  RetrievedObject<T> retrievedObj;
  retrievedObj.success = success;
  if (!success) {
    retrievedObj.obj = T();
  } else {
    retrievedObj.obj = qvariant_cast<T>(metaData[key].toVariant());
  }
  return retrievedObj;
}
}; // namespace MetaManager
