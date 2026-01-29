#pragma once
#include <QMainWindow>
#include <memory>

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow() override;

private slots:
void openCsv();
void cancelLoad();

void onSelectionChanged();
void saveAnnotations();
void loadAnnotations();
void applyAnnotation();
void clearAnnotation();

private:
    struct Engine;
    std::unique_ptr<Engine> e;
};