#pragma once

#include "CsvLoader.h" // for CsvDataset struct

#include <QAbstractTableModel>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QVector>

struct Annotation {
    QString note;
    QString severity;
};


class CsvTableModel final : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CsvTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setDataset(QStringList headers, QVector<QStringList> rows);
    void setDataset(CsvDataset&& ds);

    bool hasAnnotation(int row) const;
    Annotation annotation(int row) const;
    void setAnnotation(int row, Annotation a);
    void clearAnnotation(int row);

    QJsonObject annotationsToJson() const;
    void annotationsFromJson(const QJsonObject& obj);

private:
    QStringList m_headers;
    QVector<QStringList> m_rows;
    QHash<int, Annotation> m_rowAnnotations;
};