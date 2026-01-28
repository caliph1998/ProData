#pragma once
#include <QMainWindow>

class QTableView;
class CsvTableModel;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow() override;

private slots:
// void openCsv();
// void cancelLoad();

// void onSelectionChanged();
// void saveAnnotations();
// void loadAnnotations();
// void applyAnnotation();
// void clearAnnotation();

private:
    CsvTableModel* m_model = nullptr;
    QTableView* m_table = nullptr;
};