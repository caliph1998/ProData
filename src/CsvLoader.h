#pragma once

#include <atomic>

#include <QStringList>
#include <QVector>
#include <QObject>
#include <QString>

struct CsvDataset
{
    QStringList headers;
    QVector<QStringList> rows;
};

class CsvLoader final : public QObject
{
    Q_OBJECT
public:
    explicit CsvLoader(QObject* parent = nullptr);
    CsvDataset load(const QString& path);
    void requestCancel();

signals:
    void progress(int percent);

private:
    std::atomic_bool m_cancel{false};
    static QStringList splitCsvLine(const QString& line);
};