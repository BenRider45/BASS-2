
#include <QObject>
#include <QQmlEngine>

namespace Errorinator {
enum class ErrorSeverity {
  WARNING,
  ISSUE,
  CATASTROPHIC

};

}
class ErrorInator : public QObject {

  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

public:
  void logError(Errorinator::ErrorSeverity Severity, QString Message) {
    switch (Severity) {
    case Errorinator::ErrorSeverity::WARNING:
      emit warning(Message);
      break;
    case Errorinator::ErrorSeverity::ISSUE:
      emit issue(Message);
      break;
    case Errorinator::ErrorSeverity::CATASTROPHIC:
      emit catastrophic(Message);
      break;
    }
  };
signals:
  void issue(QString ErrorMessage);
  void warning(QString WarningMessage);
  void catastrophic(QString CataMessage);

private:
};
