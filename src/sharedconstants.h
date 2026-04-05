// sharedconstants.h
#ifndef SHAREDCONSTANTS_H
#define SHAREDCONSTANTS_H
#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
namespace constants {
Q_NAMESPACE
class SharedConstants : public QObject {
  Q_OBJECT
public:
  explicit SharedConstants(QObject *parent = nullptr) {};
  Q_PROPERTY(QString prop MEMBER prop CONSTANT)
  Q_PROPERTY(QString META_FILE_EXTENSION MEMBER META_FILE_EXTENSION CONSTANT)
  Q_PROPERTY(
      QString PROJECT_META_FILE_NAME MEMBER PROJECT_META_FILE_NAME CONSTANT)
  Q_PROPERTY(QString PROJECT_DIR MEMBER PROJECT_DIR CONSTANT);
  Q_PROPERTY(QString PROJECT_BIRD_NAME MEMBER PROJECT_BIRD_NAME CONSTANT);
  Q_PROPERTY(QString PROJECT_NAME MEMBER PROJECT_NAME CONSTANT);
  Q_PROPERTY(
      QString PROJECT_LAST_ACCESSED MEMBER PROJECT_LAST_ACCESSED CONSTANT);
  Q_PROPERTY(QString PROJECT_CREATE_DATE MEMBER PROJECT_CREATE_DATE CONSTANT);
  Q_PROPERTY(QString PROJECT_ID MEMBER PROJECT_ID CONSTANT);
  Q_PROPERTY(QString PROJECT_DATA MEMBER PROJECT_DATA CONSTANT);

  static QObject *singletonProvider(QQmlEngine *, QJSEngine *) {
    return new SharedConstants();
  }

  static inline const QString prop{"MY_PROPERTY_TEXT"};
  static inline const QString META_FILE_EXTENSION{".twty"};
  static inline const QString PROJECT_META_FILE_NAME{"conf"};
  static inline const QString PROJECT_DIR{"proj_dir"};
  static inline const QString PROJECT_BIRD_NAME{"proj_bird"};
  static inline const QString PROJECT_NAME{"proj_name"};
  static inline const QString PROJECT_LAST_ACCESSED{"last_access"};
  static inline const QString PROJECT_CREATE_DATE{"create_date"};
  static inline const QString PROJECT_ID{"ID"};
  static inline const QString PROJECT_DATA{"proj_data"};
  static inline const QString RECENT_PROJ_METAFILE_NAME{"BASS_RECENT_PROJECTS"};
  static inline const QString RECENT_PROJECT_APPDATA_DIRECTORY{
      "BASS_RECENT_PROJECT_DATA"};
  static inline const QString EMPTY_PROJECT_NAME{"EMPTY_PROJECT_NAME"};
  static inline const QString DEFAULT_BIRD_NAME{"Tweety Bird"};
};
} // namespace constants
#endif // SHAREDCONSTANTS_H
