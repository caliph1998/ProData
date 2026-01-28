#include "CsvTableModel.h"
#include <QBrush>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>


CsvTableModel::CsvTableModel(QObject* parent) : QAbstractTableModel(parent)
{

}

int CsvTableModel::rowCount(const QModelIndex&) const 
{
    return std::size(m_rows);
}

int CsvTableModel::columnCount(const QModelIndex&) const
{
    return std::size(m_headers);
}

QVariant CsvTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return {};

    const int r = index.row();
    const int c = index.column();

    if (r < 0 || r >= std::size(m_rows)) return {};
    if (c < 0 || c >= std::size(m_headers)) return {};

    const auto& row = m_rows[r];
    const QString value = c < std::size(row) ? row[c] : QString{};

    switch(role)
    {
        case Qt::DisplayRole:
            return value;

        case Qt::ToolTipRole:
        {
            if (m_rowAnnotations.contains(r))
            {
                const auto a = m_rowAnnotations.value(r);
                return QString("[%1] %2").arg(a.severity, a.note);
            }
        }

        case Qt::BackgroundRole:
        {
            if (m_rowAnnotations.contains(r))
            {
                const auto sev = m_rowAnnotations.value(r).severity.toLower();
                if (sev == "critical") return QBrush(QColor(255, 230, 230));
                if (sev == "warn") return QBrush(QColor(255, 245, 220));
                return QBrush(QColor(235, 245, 255));
            }
            return {};
        }

        default:
            return {};
    }
}

QVariant CsvTableModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (role != Qt::DisplayRole) return {};
    if (orient == Qt::Horizontal)
    {
        if (section >= 0 && section < std::size(m_headers))
            return m_headers[section];
        else
            return section + 1;
    }
    return {};
}

void CsvTableModel::setDataset(QStringList headers, QVector<QStringList> rows)
{
    beginResetModel();
    m_headers = std::move(headers);
    m_rows = std::move(rows);
    m_rowAnnotations.clear();
    endResetModel();
}

bool CsvTableModel::hasAnnotation(int row) const
{
    return m_rowAnnotations.contains(row);
}

Annotation CsvTableModel::annotation(int row) const
{
    return m_rowAnnotations.value(row);
}

void CsvTableModel::setAnnotation(int row, Annotation a)
{
    if (row < 0 || row >= std::size(m_rows))
        return;
    m_rowAnnotations.insert(row, std::move(a));
    emit dataChanged(index(row, 0), index(row, columnCount() - 1),
                        {Qt::BackgroundRole, Qt::ToolTipRole});
}

void CsvTableModel::clearAnnotation(int row)
{
    if (!m_rowAnnotations.contains(row))
        return;
    m_rowAnnotations.remove(row);
    emit dataChanged(index(row, 0), index(row, columnCount() - 1),
                        {Qt::BackgroundRole, Qt::ToolTipRole});
}

QJsonObject CsvTableModel::annotationsToJson() const
{
    QJsonArray arr;
    for (auto it = m_rowAnnotations.cbegin(); it != m_rowAnnotations.cend(); it++)
    {
        QJsonObject o;
        o["row"] = it.key();
        o["note"] = it.value().note;
        o["severity"] = it.value().severity;
        arr.push_back(o);
    }
    QJsonObject root;
    root["annotations"] = arr;
    return root;
}

void CsvTableModel::annotationsFromJson(const QJsonObject& obj)
{
    if (!obj.contains("annotations") || !obj["annotations"].isArray())
        return;
    
    beginResetModel();
    m_rowAnnotations.clear();
    const auto arr = obj["annotations"].toArray();
    for (const auto& v : arr)
    {
        if (!v.isObject())
            continue;
        const auto o = v.toObject();
        const auto row = o["row"].toInt(-1);
        if (row < 0 || row >= std::size(m_rows))
            continue;
        Annotation a;
        a.note = o["note"].toString();
        a.severity = o["severity"].toString("Info");
        m_rowAnnotations.insert(row, a);
    }
    endResetModel();
}