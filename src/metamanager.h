#pragma once
#include "recentProject.h"
#include "result.h"
#include "serializationtraits.h"
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <exception>
#include <iostream>
#include <stdio.h>
namespace MetaManager {
//  MetaManager(MetaManager &&) = default;
//  MetaManager(const MetaManager &) = default;
//  MetaManager &operator=(MetaManager &&) = default;
//  MetaManager &operator=(const MetaManager &) = default;
//  ~MetaManager();

template <typename T>
concept Serializable = requires(T a, QJsonObject obj) {
  { SerializationTraits<T>::serialize(a) } -> std::same_as<QJsonObject>;
  { SerializationTraits<T>::deserialize(obj) } -> std::same_as<T>;
};
// TODO implement this functionality
enum class WRITEMODE { ADD_VALUE_IF_NEEDED, OVERWRITE_ONLY };

bool VerifyMetaFileExistence(QDir dir);
QJsonObject extractMetaDataContent(QString metaFilePath);
QString getMetaFilePath(QDir path, QString metaFileName);

bool createMetaFile(QDir directory, QString metaFileName);

template <Serializable T>
Result<bool> writeData(QString metaFilePath, QString key, const T &value,
                       WRITEMODE mode = WRITEMODE::ADD_VALUE_IF_NEEDED) {
  QFile file(metaFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    return Result<bool>::err(
        QString("Cannot open file for reading: %1").arg(metaFilePath), 1);
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  if (!doc.isObject()) {
    return Result<bool>::err("File does not contain a JSON object", 1);
  }

  try {
    QJsonObject obj = doc.object();
    std::cerr << "Writing to key: " << key.toStdString() << "\n";
    obj[key] = QJsonValue::fromVariant(
        QVariant::fromValue(SerializationTraits<T>::serialize(value)));
    if (!file.open(QIODevice::WriteOnly)) {
      return Result<bool>::err(
          QString("Cannot open file for writing: %1").arg(metaFilePath), 2);
    }
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    std::cerr << "Writing value " << data.toStdString() << "\n";
    file.write(data);
    file.close();
    return Result<bool>::ok(true);
  } catch (const std::exception &e) {
    return Result<bool>::err(QString("Serialization Failed: %1").arg(e.what()),
                             1);
  }
}

template <Serializable T>
Result<T> retrieveData(QString metaFilePath, QString key) {
  QFile file(metaFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    return Result<T>::err(QString("Cannot open file: %1").arg(metaFilePath), 2);
  }

  QByteArray data = file.readAll();

  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) {
    return Result<T>::err(QString("File does not contain Json Object"), 1);
  }

  QJsonObject metaData = doc.object();
  std::cerr << "key amt: " << metaData.count() << "\n";
  for (auto k : metaData.keys()) {
    std::cerr << "Key: " << k.toStdString() << "\n";
  }
  if (!metaData.contains(key)) {
    return Result<T>::err(QString("Key not found: '%1'").arg(key), 1);
  }

  if (!metaData[key].isObject()) {
    return Result<T>::err(
        QString("Value for key '%1' is not an object").arg(key), 1);
  }

  try {
    T obj = SerializationTraits<T>::deserialize(metaData[key].toObject());
    return Result<T>::ok(obj);
  } catch (const std::exception &e) {
    return Result<T>::err(QString("Failed to deserialize '%1': %2")
                              .arg(key, QString::fromStdString(e.what())),
                          1);
  }
}

namespace RecentProjectUtils {

inline Result<QVector<RecentProject>>

extractRecentProjectList(QString recentProjectFilePath) {
  QFile file(recentProjectFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    std::cerr << "ERROR: Could not open file in extractRecentProjectList\n";
  }

  QByteArray data = file.readAll();

  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);

  QJsonObject metaData = doc.object();
  QVector<RecentProject> output;
  for (const auto key : metaData.keys()) {
    try {
      RecentProject proj = SerializationTraits<RecentProject>::deserialize(
          metaData[key].toObject());
      output.append(proj);
    } catch (const std::exception &e) {
      return Result<QVector<RecentProject>>::err(
          QString("Error, could not deserialize '%1' as RecentProject: '%2'")
              .arg(key, QString::fromStdString(e.what())),
          1);
    }
  }
  return Result<QVector<RecentProject>>::ok(output);
}
} // namespace RecentProjectUtils
// template <Variantable T>
// RetrievedObject<T> retrieveData(QString MetaFilePath, QString key) {
//   QJsonObject metaData = extractMetaDataContent(MetaFilePath);
//   bool success = metaData.keys().contains(key);
//   RetrievedObject<T> retrievedObj;
//   retrievedObj.success = success;
//   if (!success) {
//     retrievedObj.obj = T();
//   } else {
//     qDebug() << "metaData[key]:" << metaData[key].toString() << "\n";
//     retrievedObj.obj = qvariant_cast<T>(metaData[key].toVariant());
//   }
//   return retrievedObj;
// }
}; // namespace MetaManager
