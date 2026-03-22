#include "filelistmodel.h"

FileListModel::FileListModel(QObject *parent) : QAbstractListModel(parent) {}

int FileListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_files.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_files.size())
    return {};

  const auto &entry = m_files.at(index.row());
  switch (role) {
  case FileNameRole:
    return entry.fileName;
  case AnnotationCountRole:
    return entry.annotationCount;
  case FilePathRole:
    return entry.filePath;
  }
  return {};
}

QHash<int, QByteArray> FileListModel::roleNames() const {
  return {{FileNameRole, "fileName"},
          {AnnotationCountRole, "annotationCount"},
          {FilePathRole, "filePath"}};
}

int FileListModel::currentIndex() const { return m_currentIndex; }

void FileListModel::setCurrentIndex(int index) {
  if (m_currentIndex != index) {
    m_currentIndex = index;
    emit currentIndexChanged();
  }
}

int FileListModel::count() const { return m_files.size(); }

int FileListModel::currentPage() const { return m_currentPage; }

void FileListModel::setCurrentPage(int page) {
  if (m_currentPage != page) {
    m_currentPage = page;
    emit currentPageChanged();
  }
}

int FileListModel::pageCount() const { return m_pageCount; }

void FileListModel::loadFromProject() {
  // Skeleton: would scan project wav directory
  beginResetModel();
  m_files.clear();
  endResetModel();
  emit countChanged();
}

void FileListModel::selectFile(int index) {
  if (index >= 0 && index < m_files.size()) {
    setCurrentIndex(index);
  }
}
