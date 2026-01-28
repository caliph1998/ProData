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

MainWindow::MainWindow() {
    setWindowTitle("Dataset Inspector");

    m_model = new CsvTableModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterKeyColumn(-1);

    m_table = new QTableView(this);
    m_table->setModel(m_proxy);
    m_table->setSortingEnabled(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged);
    
    auto* rightPanel = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(new QLabel("Filter (search):", rightPanel));
    
    m_filter = new QLineEdit(rightPanel);
    m_filter->setPlaceholderText("Type to filter rows...");
    rightLayout->addWidget(m_filter);
    connect(m_filter, &QLineEdit::textChanged, this, [this](const QString& t) {
        m_proxy->setFilterFixedString(t);
    });

    rightLayout->addSpacing(8);
    rightLayout->addWidget(new QLabel("Annotation note:", rightPanel));
    m_note = new QPlainTextEdit(rightPanel);
    m_note->setPlaceholderText("Write a note about the selected row...");
    m_note->setMaximumBlockCount(2000);
    rightLayout->addWidget(m_note, 1);

    auto* splitter = new QSplitter(this);
    splitter->addWidget(m_table);
    splitter->addWidget(rightPanel);

    setCentralWidget(splitter);
}

MainWindow::~MainWindow() = default;

void MainWindow::onSelectionChanged()
{

}
