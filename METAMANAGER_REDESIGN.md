# MetaManager Redesign: Implementation Guide for Option 4

## Overview

This document outlines the step-by-step implementation of a robust MetaManager redesign that combines:
- **Serialization Traits**: Type-specific serialize/deserialize logic
- **Result Type**: Better error handling and error information
- **Type Safety**: Compile-time validation of convertibility

This approach eliminates silent failures and provides clear error messages when something goes wrong.

---

## Architecture

### Current Problems
- Type conversion silently fails with no indication beyond a flag
- No validation that stored data matches expected type
- Inconsistent error handling (asserts, error keys, flags)
- No way to add type-specific logic (validation, format conversion, etc.)

### New Design
```
┌─────────────────────────────────────────────────────────────┐
│ Caller: retrieveData<QString>(path, "proj_name")            │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
         ┌───────────────────────────┐
         │  retrieveData<T>()        │
         │  - Read JSON              │
         │  - Check key exists       │
         │  - Call deserializer      │
         └────────────┬──────────────┘
                      │
                      ▼
         ┌───────────────────────────┐
         │ SerializationTraits<T>    │
         │ - Validate data           │
         │ - Convert format          │
         │ - Return T or throw       │
         └────────────┬──────────────┘
                      │
                      ▼
         ┌───────────────────────────┐
         │  Result<T>                │
         │  - success flag           │
         │  - T value OR error msg   │
         └────────────┬──────────────┘
                      │
                      ▼
         ┌───────────────────────────┐
         │ Caller handles result     │
         │ - Check isSuccess()       │
         │ - Get value() or error()  │
         └───────────────────────────┘
```

---

## Step-by-Step Implementation

### Step 1: Create the Result Type Wrapper

**File**: `src/result.h`

```cpp
#pragma once
#include <QString>

template <typename T>
class Result {
public:
  // Constructors
  explicit Result(T value)
    : success_(true), value_(value), error_("") {}

  explicit Result(QString error)
    : success_(false), value_(T()), error_(error) {}

  // Query methods
  [[nodiscard]] bool isSuccess() const { return success_; }
  [[nodiscard]] bool isError() const { return !success_; }

  // Access methods
  T& value() {
    Q_ASSERT(success_);
    return value_;
  }

  const T& value() const {
    Q_ASSERT(success_);
    return value_;
  }

  [[nodiscard]] QString error() const { return error_; }

  // Factory methods
  static Result<T> ok(T value) {
    return Result<T>(value);
  }

  static Result<T> err(QString message) {
    return Result<T>(message);
  }

  // Monadic operations (optional but useful)
  template <typename U, typename Fn>
  Result<U> map(Fn f) const {
    if (isError()) {
      return Result<U>::err(error_);
    }
    try {
      return Result<U>::ok(f(value_));
    } catch (const std::exception& e) {
      return Result<U>::err(QString::fromStdString(e.what()));
    }
  }

private:
  bool success_;
  T value_;
  QString error_;
};
```

**Why this approach:**
- Explicit success/error state
- Clear API: `isSuccess()`, `value()`, `error()`
- No null pointers or default-constructed values with false hope
- Monadic `map()` allows chaining operations

---

### Step 2: Create Serialization Traits

**File**: `src/serialization_traits.h`

```cpp
#pragma once
#include <QDir>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>

// Base template (no implementation - catch errors at compile time)
template <typename T>
class SerializationTraits;

// Specialization for QString
template <>
class SerializationTraits<QString> {
public:
  static QJsonObject serialize(const QString& str) {
    QJsonObject obj;
    obj["value"] = str;
    return obj;
  }

  static QString deserialize(const QJsonObject& obj) {
    if (!obj.contains("value")) {
      throw std::runtime_error("QString: missing 'value' field");
    }
    return obj["value"].toString();
  }
};

// Specialization for QDir
template <>
class SerializationTraits<QDir> {
public:
  static QJsonObject serialize(const QDir& dir) {
    QJsonObject obj;
    obj["path"] = dir.absolutePath();
    obj["exists"] = dir.exists();
    return obj;
  }

  static QDir deserialize(const QJsonObject& obj) {
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
template <>
class SerializationTraits<QDateTime> {
public:
  static QJsonObject serialize(const QDateTime& dt) {
    QJsonObject obj;
    obj["iso"] = dt.toString(Qt::ISODate);
    obj["timezone"] = "UTC";  // Document assumptions
    return obj;
  }

  static QDateTime deserialize(const QJsonObject& obj) {
    if (!obj.contains("iso")) {
      throw std::runtime_error("QDateTime: missing 'iso' field");
    }
    QDateTime dt = QDateTime::fromString(
      obj["iso"].toString(),
      Qt::ISODate
    );

    if (!dt.isValid()) {
      throw std::runtime_error(
        "QDateTime: invalid ISO date format"
      );
    }

    return dt;
  }
};

// Specialization for int
template <>
class SerializationTraits<int> {
public:
  static QJsonObject serialize(int value) {
    QJsonObject obj;
    obj["value"] = value;
    return obj;
  }

  static int deserialize(const QJsonObject& obj) {
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
template <>
class SerializationTraits<QStringList> {
public:
  static QJsonObject serialize(const QStringList& list) {
    QJsonObject obj;
    QJsonArray arr;
    for (const auto& str : list) {
      arr.append(str);
    }
    obj["items"] = arr;
    return obj;
  }

  static QStringList deserialize(const QJsonObject& obj) {
    if (!obj.contains("items")) {
      throw std::runtime_error("QStringList: missing 'items' field");
    }
    if (!obj["items"].isArray()) {
      throw std::runtime_error("QStringList: 'items' is not an array");
    }

    QStringList list;
    for (const auto& val : obj["items"].toArray()) {
      list.append(val.toString());
    }
    return list;
  }
};
```

**Key design decisions:**
1. **Explicit field names** in JSON (not relying on implicit conversion)
2. **Validation** before returning (catch type mismatches)
3. **Exceptions** for errors (clearer than returning empty values)
4. **Documentation** in the JSON (e.g., `"timezone": "UTC"`)

---

### Step 3: Update MetaManager Header

**File**: `src/metamanager.h` (modified)

```cpp
#pragma once
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include "result.h"
#include "serialization_traits.h"

namespace MetaManager {

// Type concept for serializable types
template <typename T>
concept Serializable = requires(T a, QJsonObject obj) {
  { SerializationTraits<T>::serialize(a) } -> std::same_as<QJsonObject>;
  { SerializationTraits<T>::deserialize(obj) } -> std::same_as<T>;
};

// File operations
bool VerifyMetaFileExistence(QDir dir);
QJsonObject extractMetaDataContent(QString metaFilePath);
QString getMetaFilePath(QDir path, QString metaFileName);
bool createMetaFile(QDir directory, QString metaFileName);

// Write data using serialization traits
template <Serializable T>
Result<bool> writeData(QString metaFilePath, QString key, const T& value) {
  QFile file(metaFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    return Result<bool>::err(
      QString("Cannot open file for reading: %1").arg(metaFilePath));
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  if (!doc.isObject()) {
    return Result<bool>::err("File does not contain a JSON object");
  }

  try {
    QJsonObject obj = doc.object();
    obj[key] = QJsonValue::fromVariant(
      QVariant::fromValue(SerializationTraits<T>::serialize(value))
    );

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      return Result<bool>::err(
        QString("Cannot open file for writing: %1").arg(metaFilePath));
    }

    QJsonDocument newDoc(obj);
    file.write(newDoc.toJson());
    file.close();

    return Result<bool>::ok(true);
  } catch (const std::exception& e) {
    return Result<bool>::err(
      QString("Serialization failed: %1").arg(e.what()));
  }
}

// Read data using serialization traits
template <Serializable T>
Result<T> retrieveData(QString metaFilePath, QString key) {
  QFile file(metaFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    return Result<T>::err(
      QString("Cannot open file: %1").arg(metaFilePath));
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) {
    return Result<T>::err("File does not contain a JSON object");
  }

  QJsonObject metaData = doc.object();

  if (!metaData.contains(key)) {
    return Result<T>::err(
      QString("Key not found: '%1'").arg(key));
  }

  if (!metaData[key].isObject()) {
    return Result<T>::err(
      QString("Value for key '%1' is not an object").arg(key));
  }

  try {
    T obj = SerializationTraits<T>::deserialize(
      metaData[key].toObject()
    );
    return Result<T>::ok(obj);
  } catch (const std::exception& e) {
    return Result<T>::err(
      QString("Failed to deserialize '%1': %2")
        .arg(key, QString::fromStdString(e.what())));
  }
}

} // namespace MetaManager
```

**Key improvements:**
1. Generic `writeData()` uses serialization traits
2. Generic `retrieveData()` with proper error handling
3. `Serializable` concept ensures only compatible types are used
4. All errors produce `Result<T>` with detailed messages

---

### Step 4: Update MetaManager Implementation

**File**: `src/metamanager.cpp` (simplified)

```cpp
#include "metamanager.h"
#include "sharedconstants.h"
#include <QJsonDocument>
#include <iostream>

namespace MetaManager {

QJsonObject extractMetaDataContent(QString metaFilePath) {
  QFile file(metaFilePath);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open meta file:" << metaFilePath;
    return QJsonObject();
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  file.close();

  return doc.isObject() ? doc.object() : QJsonObject();
}

bool createMetaFile(QDir directory, QString metaFileName) {
  if (VerifyMetaFileExistence(directory)) {
    qWarning() << "Meta file already exists in" << directory.path();
    return false;
  }

  QString filePath = directory.filePath(
    metaFileName + constants::SharedConstants::META_FILE_EXTENSION
  );

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning() << "Cannot create meta file:" << filePath;
    return false;
  }

  QJsonDocument doc(QJsonObject());
  file.write(doc.toJson());
  file.close();

  return true;
}

bool VerifyMetaFileExistence(QDir directory) {
  QStringList filters;
  filters << "*" + constants::SharedConstants::META_FILE_EXTENSION;
  directory.setNameFilters(filters);
  return directory.entryList().length() > 0;
}

QString getMetaFilePath(QDir path, QString fileName) {
  QString fullName = fileName +
    constants::SharedConstants::META_FILE_EXTENSION;

  if (!path.exists(fullName)) {
    qWarning() << "Meta file not found:" << fullName;
  }

  return path.filePath(fullName);
}

} // namespace MetaManager
```

**Changes:**
1. Removed all `assert()` statements
2. Return proper error states
3. Cleaner error messages via `qWarning()`
4. Simplified logic (complexity moved to traits)

---

### Step 5: Update Usage Sites

**Before:**
```cpp
MetaManager::RetrievedObject<QString> result =
  MetaManager::retrieveData<QString>(metaPath, "proj_name");

if (!result.success) {
  // What failed? No idea.
  return;
}

QString name = result.obj;
```

**After:**
```cpp
auto result = MetaManager::retrieveData<QString>(metaPath, "proj_name");

if (!result.isSuccess()) {
  errorinator.logError(
    QString("Failed to load project name: %1").arg(result.error()));
  return;
}

QString name = result.value();
```

Or with chaining:
```cpp
auto result = MetaManager::retrieveData<QString>(metaPath, "proj_name")
  .map([](const QString& name) {
    return name.trimmed().isEmpty() ? "Untitled" : name;
  });

if (result.isSuccess()) {
  projectName = result.value();
}
```

---

## Testing Strategy

### Unit Tests

**File**: `tests/test_metamanager_redesigned.cpp`

```cpp
#include "metamanager.h"
#include "result.h"
#include <gtest/gtest.h>
#include <QTemporaryDir>

class MetaManagerRedesignTest : public testing::Test {
protected:
  void SetUp() override {
    tempDir.setAutoRemove(true);
    ASSERT_TRUE(tempDir.isValid());
  }

  QTemporaryDir tempDir;
};

TEST_F(MetaManagerRedesignTest, WriteAndReadString) {
  QString metaFile = tempDir.filePath("test.twty");
  ASSERT_TRUE(MetaManager::createMetaFile(tempDir.path(), "test"));

  // Write
  auto writeResult = MetaManager::writeData<QString>(
    metaFile, "name", "MyProject");
  EXPECT_TRUE(writeResult.isSuccess());

  // Read
  auto readResult = MetaManager::retrieveData<QString>(metaFile, "name");
  EXPECT_TRUE(readResult.isSuccess());
  EXPECT_EQ(readResult.value(), "MyProject");
}

TEST_F(MetaManagerRedesignTest, ReadMissingKey) {
  QString metaFile = tempDir.filePath("test.twty");
  MetaManager::createMetaFile(tempDir.path(), "test");

  auto result = MetaManager::retrieveData<QString>(metaFile, "missing");
  EXPECT_FALSE(result.isSuccess());
  EXPECT_TRUE(result.error().contains("not found"));
}

TEST_F(MetaManagerRedesignTest, ReadWrongType) {
  QString metaFile = tempDir.filePath("test.twty");
  MetaManager::createMetaFile(tempDir.path(), "test");

  // Write an int
  auto write = MetaManager::writeData<int>(metaFile, "count", 42);
  EXPECT_TRUE(write.isSuccess());

  // Try to read as QString
  auto read = MetaManager::retrieveData<QString>(metaFile, "count");
  EXPECT_FALSE(read.isSuccess());
  EXPECT_TRUE(read.error().contains("Deserialization failed"));
}

TEST_F(MetaManagerRedesignTest, WriteAndReadQDir) {
  QString metaFile = tempDir.filePath("test.twty");
  MetaManager::createMetaFile(tempDir.path(), "test");

  QDir originalDir(tempDir.path());

  auto write = MetaManager::writeData<QDir>(metaFile, "proj_dir", originalDir);
  EXPECT_TRUE(write.isSuccess());

  auto read = MetaManager::retrieveData<QDir>(metaFile, "proj_dir");
  EXPECT_TRUE(read.isSuccess());
  EXPECT_EQ(read.value().path(), originalDir.absolutePath());
}

TEST_F(MetaManagerRedesignTest, WriteAndReadQDateTime) {
  QString metaFile = tempDir.filePath("test.twty");
  MetaManager::createMetaFile(tempDir.path(), "test");

  QDateTime now = QDateTime::currentDateTimeUtc();

  auto write = MetaManager::writeData<QDateTime>(
    metaFile, "timestamp", now);
  EXPECT_TRUE(write.isSuccess());

  auto read = MetaManager::retrieveData<QDateTime>(metaFile, "timestamp");
  EXPECT_TRUE(read.isSuccess());
  // QDateTime comparison (within second tolerance for serialization)
  EXPECT_TRUE(
    read.value().secsTo(now) < 1 || now.secsTo(read.value()) < 1
  );
}

TEST_F(MetaManagerRedesignTest, InvalidJsonFile) {
  QString metaFile = tempDir.filePath("invalid.twty");
  QFile file(metaFile);
  file.open(QIODevice::WriteOnly);
  file.write("not valid json");
  file.close();

  auto result = MetaManager::retrieveData<QString>(metaFile, "key");
  EXPECT_FALSE(result.isSuccess());
}
```

---

## Migration Path

### Phase 1: Parallel Implementation
1. Add `result.h` and `serialization_traits.h` to codebase
2. Keep old `RetrievedObject` temporarily
3. Implement new `retrieveData()` alongside old one
4. Update tests incrementally

### Phase 2: Gradual Migration
1. Update one usage site at a time
2. Test thoroughly after each update
3. Keep error handling consistent with `Errorinator`

### Phase 3: Cleanup
1. Remove old `retrieveData()` and `RetrievedObject`
2. Remove old tests
3. Update documentation

### Version Changes in JSON
If you need to handle old JSON format:

```cpp
namespace MetaManager {

Result<QString> migrateStringFormat(const QJsonValue& old) {
  // Handle old plain-string format
  if (old.isString()) {
    QJsonObject obj;
    obj["value"] = old.toString();
    return Result<QString>::ok(obj);
  }

  // Assume new format
  if (old.isObject()) {
    return Result<QString>::ok(old.toObject());
  }

  return Result<QString>::err("Invalid string format");
}

}
```

---

## Summary

| Aspect | Before | After |
|--------|--------|-------|
| Error visibility | Silent failures | Detailed messages |
| Type safety | Runtime (qvariant_cast) | Compile-time (Serializable concept) |
| Validation | None | Per-type validation |
| Error handling | Asserts + flags | Result type |
| Extensibility | Add qvariant_cast | Add trait specialization |
| Testing | Difficult | Straightforward |

---

## Files to Create/Modify

```
NEW:
  src/result.h                          (Result<T> type)
  src/serialization_traits.h            (Type-specific logic)
  tests/test_metamanager_redesigned.cpp (New tests)

MODIFY:
  src/metamanager.h                     (New functions, updated signatures)
  src/metamanager.cpp                   (Simplified, no asserts)

DELETE (after migration):
  Old tests referencing RetrievedObject
  RetrievedObject from metamanager.h
```

---

## Next Steps

1. Create `result.h` with Result<T> class
2. Create `serialization_traits.h` with specializations
3. Update `metamanager.h` with new function signatures
4. Update `metamanager.cpp` to remove asserts
5. Write unit tests in `test_metamanager_redesigned.cpp`
6. Gradually migrate usage sites (ProjectManager, etc.)
