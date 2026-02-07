# ProData

A Qt6-based desktop application for inspecting, filtering, and annotating CSV datasets.

## Features

- **CSV Loading**: Load and display large CSV files with support for progress tracking and cancellation
- **Table Viewing**: Interactive table view with sortable columns and row selection
- **Filtering**: Real-time search and filtering across all columns
- **Annotation System**: Add notes and severity levels (Info, Warn, Critical) to individual rows
- **Annotation Persistence**: Save and load annotations to/from JSON files
- **Concurrent Processing**: Uses Qt's concurrent framework for responsive UI during large file operations

## Project Structure

```
.
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── MainWindow.h/cpp         # Main application window and UI
│   ├── CsvTableModel.h/cpp      # Data model for CSV display with annotations
│   ├── CsvLoader.h/cpp          # CSV file parsing and loading logic
│   └── CMakeLists.txt           # Build configuration
├── tests/
│   └── test_csv.cpp             # Unit tests for CSV functionality
└── CMakeLists.txt               # Root build configuration
```

## Building

### Prerequisites

- C++17 or later
- CMake 3.16+
- Qt6 (Widgets, Concurrent, Test components)

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd build
ctest
# or directly:
./DatasetInspectorTests
```

## Running the Application

```bash
cd build
./DatasetInspector
```

This will open a window sized to 1100x700 pixels.

## Usage

1. **Load a CSV**: Use the menu to open a CSV file
2. **Browse Data**: Navigate through the table using sorting and scrolling
3. **Filter Rows**: Type in the search box to filter by any column value
4. **Add Annotations**: 
   - Select a row in the table
   - Enter a note and select a severity level
   - Click "Apply" to save the annotation
5. **Manage Annotations**:
   - Click "Clear" to remove annotations from the selected row
   - Use the menu to save/load annotations to/from JSON files

## Components

### MainWindow
The main application window managing the UI layout with:
- Table view for CSV data display
- Filter search box
- Annotation editor panel
- Progress bar and status indicators
- Menu bar with file operations

### CsvTableModel
Custom `QAbstractTableModel` implementation providing:
- Dataset display with headers and rows
- Per-row annotation storage
- JSON serialization of annotations
- Support for dynamic data updates

### CsvLoader
Asynchronous CSV file loader with:
- Configurable CSV parsing (handles quoted fields and commas)
- Progress signal emission
- Cancellable loading operations
- Support for large files

## Architecture

The application uses a pimpl (pointer to implementation) pattern in `MainWindow` with an internal `Engine` struct containing all UI components and business logic. This provides better encapsulation and faster compilation.

CSV loading is performed using Qt's concurrent framework, keeping the UI responsive during large file operations.

## Testing

The project includes unit tests for CSV parsing functionality. Tests are built and run using Qt Test framework.

```bash
cd build
./DatasetInspectorTests
```

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## Author

Ali Khoshrou
