#include "MainWindow.h"
#include "CsvTableModel.h"
#include "CsvLoader.h"

#include <QAction>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrent>

struct MainWindow::Engine 
{
  CsvTableModel* model = nullptr;
  QSortFilterProxyModel* proxy = nullptr;

  QTableView* table = nullptr;
  QLineEdit* filter = nullptr;
  QPlainTextEdit* note = nullptr;
  QComboBox* severity = nullptr;
  QLabel* status = nullptr;
  QProgressBar* progress = nullptr;

  QAction* cancelAction = nullptr;

  CsvLoader* loader = nullptr;
  QFutureWatcher<CsvDataset> watcher;

//   QString lastCsvPath;
//   QString lastNotesPath;
};

MainWindow::MainWindow() : QMainWindow(), e(std::make_unique<Engine>()) {
    setWindowTitle("Dataset Inspector");

    // model setup
    e->model = new CsvTableModel(this);
    e->proxy = new QSortFilterProxyModel(this);
    e->proxy->setSourceModel(e->model);
    e->proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    e->proxy->setFilterKeyColumn(-1);

    // table
    e->table = new QTableView(this);
    e->table->setModel(e->proxy);
    e->table->setSortingEnabled(true);
    e->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    e->table->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(e->table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged);
    
    auto* rightPanel = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(new QLabel("Filter (search):", rightPanel));
    
    e->filter = new QLineEdit(rightPanel);
    e->filter->setPlaceholderText("Type to filter rows...");
    rightLayout->addWidget(e->filter);
    connect(e->filter, &QLineEdit::textChanged, this, [this](const QString& t) {
        e->proxy->setFilterFixedString(t);
    });

    // right panel (annotation editor)
    rightLayout->addSpacing(8);
    rightLayout->addWidget(new QLabel("Annotation note:", rightPanel));
    e->note = new QPlainTextEdit(rightPanel);
    e->note->setPlaceholderText("Write a note about the selected row...");
    e->note->setMaximumBlockCount(2000);
    rightLayout->addWidget(e->note, 1);

    rightLayout->addWidget(new QLabel("Severity:", rightPanel));
    e->severity = new QComboBox(rightPanel);
    e->severity->addItems({"Info", "Warn", "Critical"});
    rightLayout->addWidget(e->severity);

    auto* btnRow = new QWidget(rightPanel);
    auto* btnLayout = new QHBoxLayout(btnRow);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    auto* applyBtn = new QPushButton("Apply", btnRow);
    auto* clearBtn = new QPushButton("Clear", btnRow);
    btnLayout->addWidget(applyBtn);
    btnLayout->addWidget(clearBtn);
    rightLayout->addWidget(btnRow);

    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyAnnotation);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearAnnotation);

    // splitter main layout
    auto* splitter = new QSplitter(this);
    splitter->addWidget(e->table);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    // status
    e->status = new QLabel(this);
    e->progress = new QProgressBar(this);
    e->progress->setRange(0, 100);
    e->progress->setValue(0);
    e->progress->setFixedWidth(180);
    statusBar()->addWidget(e->status, 1);
    statusBar()->addPermanentWidget(e->progress);

    // Toolbar
    auto* tb = addToolBar("Main");
    tb->setMovable(false);
    tb->addAction("Open CSV", this, &MainWindow::openCsv);
    tb->addAction("Save Notes", this, &MainWindow::saveAnnotations);
    tb->addAction("Load Notes", this, &MainWindow::loadAnnotations);
    e->cancelAction = tb->addAction("Cancel Load", this, &MainWindow::cancelLoad);
    e->cancelAction->setEnabled(false);

    e->loader = new CsvLoader(this);
    connect(e->loader, &CsvLoader::progress, this, [this](int p) {
        e->progress->setValue(p);
    });

    connect(&e->watcher, &QFutureWatcher<CsvDataset>::finished, this, [this]() {
        try
        {
            e->model->setDataset(e->watcher.result());
            setBusy(false, QString("Loaded %1 rows.").arg(e->model->rowCount()));
        } catch (const std::exception& e) {
            setBusy(false, "Load failed.");
            QMessageBox::critical(this, "Error", e.what());
        }
    });

    setBusy(false, "Ready.");
}

MainWindow::~MainWindow() = default;

int MainWindow::currentSourceRow() const
{
    const auto idx = e->table->currentIndex();
    if (!idx.isValid())
        return -1;
    const auto src = e->proxy->mapToSource(idx);
    return src.row();
}

void MainWindow::onSelectionChanged()
{
    const int r = currentSourceRow();
    if (r < 0)
        return;
    if (e->model->hasAnnotation(r))
    {
        const auto a = e->model->annotation(r);
        e->note->setPlainText(a.note);
        const int s = e->severity->findText(a.severity);
        e->severity->setCurrentIndex(s >= 0 ? s : 0);
    }
    else
    {
        e->note->clear();
        e->severity->setCurrentIndex(0);
    }
}

void MainWindow::applyAnnotation()
{
    const int r = currentSourceRow();
    if (r < 0)
    {
        QMessageBox::information(this, "No selection", "Select a row first.");
        return;
    }

    Annotation a;
    a.note = e->note->toPlainText().trimmed();
    a.severity = e->severity->currentText();
    if (a.note.isEmpty())
    {
        QMessageBox::information(this, "Empty note", "Write a note (or press Clear).");
        return;
    }
    e->model->setAnnotation(r, a);
    e->status->setText("Annotation saved for selected row.");
}

void MainWindow::clearAnnotation()
{
    const int r = currentSourceRow();
    if (r < 0)
        return;
    e->model->clearAnnotation(r);
    e->note->clear();
    e->severity->setCurrentIndex(0);
    e->status->setText("Annotation cleared.");
}

void MainWindow::saveAnnotations()
{
    const QString path = QFileDialog::getSaveFileName(
        this, "Save Annotations", "annotations.json", "JSON (*.json)");
    if (path.isEmpty())
        return;
    
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Error", "Could not write file.");
        return;
    }

    QJsonDocument doc(e->model->annotationsToJson());
    f.write(doc.toJson(QJsonDocument::Indented));
    e->status->setText("Annotations saved.");
}

void MainWindow::loadAnnotations()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Load Annotations", QString(), "JSON (*.json)");
    if (path.isEmpty())
        return;
    
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Error", "Could not read file.");
        return;
    }

    const auto bytes = f.readAll();
    const auto doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
    {
        QMessageBox::critical(this, "Error", "Invalid JSON.");
        return;
    }

    e->model->annotationsFromJson(doc.object());
    e->status->setText("Annotations loaded.");
}

void MainWindow::openCsv()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Open CSV", QString(), "CSV Files (*.csv);;All Files (*)");
    if (path.isEmpty())
        return;
    e->loader->requestCancel();
    e->loader = new CsvLoader(this);

    // re-connect required since loader is re-initialized
    connect(e->loader, &CsvLoader::progress, this, [this](int p) {
        e->progress->setValue(p);
    });

    setBusy(true, "Loading...");

    auto future = QtConcurrent::run([loader = e->loader, path]() {
        return loader->load(path);
    });
    e->watcher.setFuture(future);
}

void MainWindow::cancelLoad()
{
    if (e->loader)
        e->loader->requestCancel();
    setBusy(true, "Cancel requested...");
}

void MainWindow::setBusy(bool busy, const QString& msg)
{
    e->cancelAction->setEnabled(busy);
    e->filter->setEnabled(!busy);
    e->table->setEnabled(!busy);
    e->note->setEnabled(!busy);
    e->severity->setEnabled(!busy);

    e->status->setText(msg);
    if (!busy)
        e->progress->setValue(0);
}