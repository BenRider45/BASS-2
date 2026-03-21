#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class MetaManager {
public:
  //  MetaManager(MetaManager &&) = default;
  //  MetaManager(const MetaManager &) = default;
  //  MetaManager &operator=(MetaManager &&) = default;
  //  MetaManager &operator=(const MetaManager &) = default;
  //  ~MetaManager();

  bool VerifyMetaFileExistence(QDir dir);
  QJsonObject extractMetaDataContent(QString metaFilePath);
  QString getMetaFilePath(QDir path, QString metaFileName);
  bool modMetaFile(QString metaFilePath, const QStringList &keys,
                   const QVariantList &values, bool addValuesIfNeeded = false);

  bool createMetaFile(QDir directory, QString metaFileName);
};
