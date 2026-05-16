#include "annotationmodel.h"
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
  }
  return {};
}

QHash<int, QByteArray> AnnotationModel::roleNames() const {
  return {{StartFrameRole, "startFrame"},
          {EndFrameRole, "endFrame"},
          {LabelRole, "label"},
          {SyllableIdRole, "syllableId"},
          {isPartialRole, "isPartial"}};
}

int AnnotationModel::count() const { return m_frames.size(); }

int AnnotationModel::addFrame(int start, int end, const QString &label,
                              bool isPartial) {
  beginInsertRows(QModelIndex(), m_frames.size(), m_frames.size());
  m_frames.append({start, end, label, -1, isPartial});
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
  std::cerr << "in edit label" << "\n";
  if (index < 0 || index >= m_frames.size())
    return;
  m_frames[index].label = newLabel;
  emit dataChanged(this->index(index), this->index(index), {LabelRole});
}

int AnnotationModel::beginFrame(const int start) {
  std::cerr << "In beginFrame with start: " << start << "\n";
  return addFrame(start, -1, "No Label", true);
}

void AnnotationModel::completeFrame(const int index, const int end) {
  std::cerr << "In completeFrame ith index: " << index << " and end: " << end
            << "\n";
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

void AnnotationModel::load(const QString &path) {
  Q_UNUSED(path);
  beginResetModel();
  m_frames.clear();
  endResetModel();
  emit countChanged();
}
