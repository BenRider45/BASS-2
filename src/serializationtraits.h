#pragma once
#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <stdexcept>
// Base template (no implementation - catch errors at compile time)
template <typename T> class SerializationTraits;

// Specialization for QString
template <> class SerializationTraits<QString> {
public:
  static QJsonObject serialize(const QString &str) {
    QJsonObject obj;
    obj["value"] = str;
    return obj;
  }

  static QString deserialize(const QJsonObject &obj) {
    if (!obj.contains("value")) {
      throw std::runtime_error("QString: missing 'value' field");
    }
    return obj["value"].toString();
  }
};

// Specialization for QDir
template <> class SerializationTraits<QDir> {
public:
  static QJsonObject serialize(const QDir &dir) {
    QJsonObject obj;
    obj["path"] = dir.absolutePath();
    obj["exists"] = dir.exists();
    return obj;
  }

  static QDir deserialize(const QJsonObject &obj) {
    if (!obj.contains("path")) {
      throw std::runtime_error("QDir: missing 'path' field");
    }
    QString path = obj["path"].toString();
    QDir dir(path);

    // Optional: validate the path exists
    if (!dir.exists()) {
      qWarning() << "QDir path does not exist:" << path;
      // Can decide: throw or warn based on your requirements
    }

    return dir;
  }
};

// Specialization for QDateTime
template <> class SerializationTraits<QDateTime> {
public:
  static QJsonObject serialize(const QDateTime &dt) {
    QJsonObject obj;
    obj["iso"] = dt.toString(Qt::ISODate);
    obj["timezone"] = "UTC"; // Document assumptions
    return obj;
  }

  static QDateTime deserialize(const QJsonObject &obj) {
    if (!obj.contains("iso")) {
      throw std::runtime_error("QDateTime: missing 'iso' field");
    }
    QDateTime dt = QDateTime::fromString(obj["iso"].toString(), Qt::ISODate);

    if (!dt.isValid()) {
      throw std::runtime_error("QDateTime: invalid ISO date format");
    }

    return dt;
  }
};

// Specialization for int
template <> class SerializationTraits<int> {
public:
  static QJsonObject serialize(int value) {
    QJsonObject obj;
    obj["value"] = value;
    return obj;
  }

  static int deserialize(const QJsonObject &obj) {
    if (!obj.contains("value")) {
      throw std::runtime_error("int: missing 'value' field");
    }
    if (!obj["value"].isDouble()) {
      throw std::runtime_error("int: value is not a number");
    }
    return obj["value"].toInt();
  }
};

// Specialization for QStringList
template <> class SerializationTraits<QStringList> {
public:
  static QJsonObject serialize(const QStringList &list) {
    QJsonObject obj;
    QJsonArray arr;
    for (const auto &str : list) {
      arr.append(str);
    }
    obj["items"] = arr;
    return obj;
  }

  static QStringList deserialize(const QJsonObject &obj) {
    if (!obj.contains("items")) {
      throw std::runtime_error("QStringList: missing 'items' field");
    }
    if (!obj["items"].isArray()) {
      throw std::runtime_error("QStringList: 'items' is not an array");
    }

    QStringList list;
    for (const auto &val : obj["items"].toArray()) {
      list.append(val.toString());
    }
    return list;
  }
};

template <> class SerializationTraits<QUuid> {
public:
  static QJsonObject serialize(const QUuid &uid) {
    QJsonObject obj;
    obj["id"] = uid.toString(QUuid::WithoutBraces);
    return obj;
  }

  static QUuid deserialize(const QJsonObject &obj) {
    if (!obj.contains("id")) {
      throw std::runtime_error("QUuid: missing 'id' field");
    }

    QUuid out(obj["id"].toString());
    return out;
  }
};
