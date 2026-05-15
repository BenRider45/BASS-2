#ifndef ANNOTATIONMODEL_H
#define ANNOTATIONMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct AnnotationFrame {
  int startFrame = -1;
  int endFrame = -1;
  QString label = "";
  int syllableId = -1;
  bool isPartial = false;
};

class AnnotationModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  enum Roles {
    StartFrameRole = Qt::UserRole + 1,
    EndFrameRole,
    LabelRole,
    SyllableIdRole,
    isPartialRole
  };

  explicit AnnotationModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  int count() const;

  Q_INVOKABLE int addFrame(const int start, const int end, const QString &label,
                           bool isPartial);
  Q_INVOKABLE int beginFrame(const int start);
  Q_INVOKABLE void completeFrame(const int index, const int end);

  Q_INVOKABLE void removeFrame(const int index);
  Q_INVOKABLE void editLabel(const int index, const QString &newLabel);
  Q_INVOKABLE void save();
  Q_INVOKABLE void load(const QString &path);

signals:
  void currentFileChanged();
  void countChanged();

private:
  QVector<AnnotationFrame> m_frames;
};

#endif // ANNOTATIONMODEL_H
