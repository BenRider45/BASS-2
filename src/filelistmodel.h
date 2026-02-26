#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct FileEntry {
    QString fileName;
    int annotationCount = 0;
    QString filePath;
};

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

public:
    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        AnnotationCountRole,
        FilePathRole
    };

    explicit FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentIndex() const;
    void setCurrentIndex(int index);
    int count() const;
    int currentPage() const;
    void setCurrentPage(int page);
    int pageCount() const;

    Q_INVOKABLE void loadFromProject();
    Q_INVOKABLE void selectFile(int index);

signals:
    void currentIndexChanged();
    void countChanged();
    void currentPageChanged();
    void pageCountChanged();

private:
    QVector<FileEntry> m_files;
    int m_currentIndex = -1;
    int m_currentPage = 0;
    int m_pageCount = 1;
};

#endif // FILELISTMODEL_H
