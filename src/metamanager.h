#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

class MetaManager {
public:
  MetaManager(QDir directory, QString metaFileName);
  //  MetaManager(MetaManager &&) = default;
  //  MetaManager(const MetaManager &) = default;
  //  MetaManager &operator=(MetaManager &&) = default;
  //  MetaManager &operator=(const MetaManager &) = default;
  //  ~MetaManager();

  bool VerifyMetaFileExistence();
  QJsonObject extractMetaDataContent();
  bool modMetaFile(const QStringList &keys, const QVariantList &values,
                   bool addValuesIfNeeded = false);

private:
  QDir _dir;
  QString _metaFilePath;
  QString _metaFileName;
  QJsonObject _currentMetaProjectData;
  bool project_data_dirty;
  bool createMetaFile(QString metaFileName);
};
