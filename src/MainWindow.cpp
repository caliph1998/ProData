#include "MainWindow.h"
#include "CsvTableModel.h"

#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>

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

//   CsvLoader* loader = nullptr;
//   QFutureWatcher<CsvDataset> watcher;

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
}

MainWindow::~MainWindow() = default;

void MainWindow::onSelectionChanged()
{

}

void MainWindow::applyAnnotation()
{

}

void MainWindow::clearAnnotation()
{

}

void  MainWindow::openCsv()
{

}

void  MainWindow::cancelLoad()
{

}

void  MainWindow::saveAnnotations()
{

}

void  MainWindow::loadAnnotations()
{

}