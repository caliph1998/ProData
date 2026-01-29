#include "CsvLoader.h"

#include <QFile>
#include <QTextStream>
#include <QString>

CsvLoader::CsvLoader(QObject* parent) : QObject(parent) {}

void CsvLoader::requestCancel()
{
    m_cancel.store(true, std::memory_order_relaxed);
}

QStringList CsvLoader::splitCsvLine(const QString& line)
{
    QStringList out;
    QString cur;
    bool inQuotes{false};

    for (int i = 0; i < std::size(line); i++)
    {
        const QChar ch = line[i];
        if (ch == '"')
        {
            inQuotes = !inQuotes;
            continue;
        }
        if (ch == ',' && !inQuotes)
        {
            out.push_back(cur.trimmed());
            cur.clear();
        }
        else
        {
            cur.push_back(ch);
        }
    }
    out.push_back(cur.trimmed());
    return out;
}

CsvDataset CsvLoader::load(const QString& path)
{
    CsvDataset ds;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::runtime_error(("Failed to open file: " + path).toStdString());
    }

    const qint64 total = std::size(f);
    QTextStream in(&f);

    bool first{true};
    while (!in.atEnd())
    {
        if (m_cancel.load(std::memory_order_relaxed))
            break;
        
        const QString line = in.readLine();
        if (line.isEmpty())
            continue;

        if (first)
        {
            ds.headers = splitCsvLine(line);
            first = false;
        }
        else
        {
            ds.rows.emplace_back(splitCsvLine(line));
        }
    }

    if (total > 0)
    {
        const qint64 pos = f.pos();
        const int pct = static_cast<int>((100 * pos) / total);
        emit progress(std::min(100, std::max(0, pct)));
    }

    emit progress(100);
    return ds;
}