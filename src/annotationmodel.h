#ifndef ANNOTATIONMODEL_H
#define ANNOTATIONMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct AnnotationFrame {
    int startFrame = 0;
    int endFrame = 0;
    QString label;
    int syllableId = -1;
};

class AnnotationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        StartFrameRole = Qt::UserRole + 1,
        EndFrameRole,
        LabelRole,
        SyllableIdRole
    };

    explicit AnnotationModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString currentFile() const;
    void setCurrentFile(const QString &file);
    int count() const;

    Q_INVOKABLE void addFrame(int start, int end, const QString &label);
    Q_INVOKABLE void removeFrame(int index);
    Q_INVOKABLE void editLabel(int index, const QString &newLabel);
    Q_INVOKABLE void save();
    Q_INVOKABLE void load(const QString &path);

signals:
    void currentFileChanged();
    void countChanged();

private:
    QVector<AnnotationFrame> m_frames;
    QString m_currentFile;
};

#endif // ANNOTATIONMODEL_H
