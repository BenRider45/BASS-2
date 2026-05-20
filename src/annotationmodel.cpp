#include "annotationmodel.h"
#include "metamanager.h"
#include <iostream>
AnnotationModel::AnnotationModel(QObject *parent)
    : QAbstractListModel(parent) {}

int AnnotationModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_frames.size();
}

QVariant AnnotationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_frames.size())
    return {};
  // std::cerr << "Getting frame " << index.row() << "\n";
  const auto &frame = m_frames.at(index.row());
  switch (role) {
  case StartFrameRole:
    return frame.startFrame;
  case EndFrameRole:
    return frame.endFrame;
  case LabelRole:
    return frame.label;
  case SyllableIdRole:
    return frame.syllableId;
  case isPartialRole:
    return frame.isPartial;
  case FileNameRole:
    return frame.fileName;
  }

  return {};
}

QHash<int, QByteArray> AnnotationModel::roleNames() const {
  return {{StartFrameRole, "startFrame"}, {EndFrameRole, "endFrame"},
          {LabelRole, "label"},           {FileNameRole, "fileName"},
          {SyllableIdRole, "syllableId"}, {isPartialRole, "isPartial"}};
}

int AnnotationModel::count() const { return m_frames.size(); }

void AnnotationModel::clearModel() {
  beginResetModel();
  m_frames.clear();
  endResetModel();
}

int AnnotationModel::addFrame(int start, int end, const QString &label,
                              bool isPartial, QString fileName) {
  beginInsertRows(QModelIndex(), m_frames.size(), m_frames.size());
  m_frames.append(
      {start, end, label, QUuid::createUuid(), isPartial, fileName});
  endInsertRows();
  emit countChanged();
  return m_frames.size() - 1;
}

void AnnotationModel::removeFrame(int index) {
  if (index < 0 || index >= m_frames.size())
    return;
  beginRemoveRows(QModelIndex(), index, index);
  m_frames.removeAt(index);
  endRemoveRows();
  emit countChanged();
}

void AnnotationModel::editLabel(int index, const QString &newLabel) {
  // std::cerr << "in edit label" << "\n";
  if (index < 0 || index >= m_frames.size())
    return;
  m_frames[index].label = newLabel;
  emit dataChanged(this->index(index), this->index(index), {LabelRole});
}

int AnnotationModel::beginFrame(const int start, QString fileName) {
  //  std::cerr << "In beginFrame with start: " << start
  //            << " at file: " << fileName.toStdString() << "\n";
  return addFrame(start, -1, "No Label", true, fileName);
}

void AnnotationModel::completeFrame(const int index, const int end) {
  // std::cerr << "In completeFrame ith index: " << index << " and end: " << end
  //           << "\n";
  if (index < 0 || index >= m_frames.size()) {
    return;
  }
  m_frames[index].endFrame = end;
  m_frames[index].isPartial = false;
  emit dataChanged(this->index(index), this->index(index),
                   {EndFrameRole, isPartialRole, LabelRole});
}
void AnnotationModel::save() {
  // Skeleton: would write annotations to file
}

QVector<AnnotationFrame> AnnotationModel::getData() { return m_frames; }
void AnnotationModel::load(const QString &path) {

  clearModel();
  QJsonObject data = MetaManager::extractMetaDataContent(path);
  emit beginInsertRows(QModelIndex(), 0, data.keys().size() - 1);
  for (const auto &key : data.keys()) {
    Result<AnnotationFrame> frame =
        MetaManager::retrieveData<AnnotationFrame>(path, key);
    if (frame.isSuccess()) {
      m_frames.append(frame.value());
    }
  }
  emit endInsertRows();
  return;
}
