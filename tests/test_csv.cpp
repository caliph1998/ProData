#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QJsonDocument>

#include "../src/CsvLoader.h"
#include "../src/CsvTableModel.h"

class CsvTests : public QObject {
  Q_OBJECT

private slots:
  void load_small_csv_parses_headers_and_rows();
  void annotations_roundtrip_json();
  void model_dimensions_after_setDataset();
};

static QString writeTempCsv(const QByteArray& content) {
  QTemporaryFile tmp;
  tmp.setAutoRemove(false);
  if (!tmp.open()) {
    return {};
  }

  if (tmp.write(content) != content.size()) {
    return {};
  }
  tmp.flush();
  return tmp.fileName();
}

void CsvTests::load_small_csv_parses_headers_and_rows() {
  const QByteArray csv =
    "A,B,C,Notes\n"
    "1,2,3,\"hello, world\"\n"
    "4,5,6,plain\n"
    ",8,9,\"\"\n";

  const QString path = writeTempCsv(csv);

  CsvLoader loader;
  CsvDataset ds;
  try {
    ds = loader.load(path);
  } catch (const std::exception& e) {
    QFAIL(e.what());
  }

  QCOMPARE(ds.headers.size(), 4);
  QCOMPARE(ds.headers[0], QString("A"));
  QCOMPARE(ds.headers[3], QString("Notes"));

  QCOMPARE(ds.rows.size(), 3);
  QCOMPARE(ds.rows[0].size(), 4);
  QCOMPARE(ds.rows[0][3], QString("hello, world")); // quoted comma stays inside field
  QCOMPARE(ds.rows[2][0], QString(""));             // empty field
}

void CsvTests::annotations_roundtrip_json() {
  CsvTableModel model;

  QStringList headers{"id", "name"};
  QVector<QStringList> rows{
    {"1", "Alice"},
    {"2", "Bob"},
    {"3", "Cara"}
  };

  model.setDataset(headers, rows);

  // Set annotation on row 1 (Bob)
  Annotation a;
  a.note = "needs review";
  a.severity = "Warn";
  model.setAnnotation(1, a);

  QVERIFY(model.hasAnnotation(1));
  QCOMPARE(model.annotation(1).note, QString("needs review"));
  QCOMPARE(model.annotation(1).severity, QString("Warn"));

  // Export to JSON
  const auto obj = model.annotationsToJson();
  const QJsonDocument doc(obj);
  QVERIFY(!doc.toJson().isEmpty());

  // New model with same dataset: import JSON
  CsvTableModel model2;
  model2.setDataset(headers, rows);
  model2.annotationsFromJson(obj);

  QVERIFY(model2.hasAnnotation(1));
  QCOMPARE(model2.annotation(1).note, QString("needs review"));
  QCOMPARE(model2.annotation(1).severity, QString("Warn"));

  // Clear and ensure gone
  model2.clearAnnotation(1);
  QVERIFY(!model2.hasAnnotation(1));
}

void CsvTests::model_dimensions_after_setDataset() {
  CsvTableModel model;

  QStringList headers{"c1","c2","c3"};
  QVector<QStringList> rows{
    {"a","b","c"},
    {"d","e","f"}
  };

  model.setDataset(headers, rows);

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 3);

  // Check a cell
  const QModelIndex idx = model.index(1, 2);
  QVERIFY(idx.isValid());
  QCOMPARE(model.data(idx, Qt::DisplayRole).toString(), QString("f"));
}

QTEST_MAIN(CsvTests)
#include "test_csv.moc"
