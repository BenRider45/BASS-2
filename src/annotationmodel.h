#pragma once
#include <QAbstractListModel>
#include <QString>
#include <QUUid>
#include <QVector>
struct AnnotationFrame {
  int startFrame = -1;
  int endFrame = -1;
  QString label = "";
  QUuid syllableId;
  bool isPartial = false;
  QString fileName = "";
  // double x_0;
  // double x_scale;
};
// Q_DECLARE_METATYPE(AnnotationFrame)
class AnnotationModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  enum Roles {
    StartFrameRole = Qt::UserRole + 1,
    EndFrameRole,
    LabelRole,
    FileNameRole,
    SyllableIdRole,
    isPartialRole
  };
  explicit AnnotationModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  int count() const;
  QVector<AnnotationFrame> getData();
  Q_INVOKABLE int addFrame(const int start, const int end, const QString &label,
                           bool isPartial, QString fileName);
  Q_INVOKABLE int beginFrame(const int start, QString fileName);
  Q_INVOKABLE void completeFrame(const int index, const int end);

  Q_INVOKABLE bool removeFrame(const int index);
  Q_INVOKABLE void editLabel(const int index, const QString &newLabel);
  Q_INVOKABLE void save();
  Q_INVOKABLE void load(const QString &path);
  Q_INVOKABLE QModelIndexList
  getCurrentFilesAnnotationList(QString fileName) const;
  void clearModel();
  Q_INVOKABLE int getStartingFrame(const QModelIndex &annotation);
  Q_INVOKABLE int getEndingFrame(const QModelIndex &annotation);
  QModelIndexList
  match(const QModelIndex &start, int role, const QVariant &value,
        int hits = -1,
        Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith |
                                              Qt::MatchWrap)) const override;

signals:
  void currentFileChanged();
  void countChanged();

private:
  QVector<AnnotationFrame> m_frames;
};
