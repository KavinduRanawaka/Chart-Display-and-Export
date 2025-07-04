# Serial Data Visualizer (Qt C++ GUI Application)

This project is a Qt-based C++ desktop application designed for reading and visualizing real-time data from a serial port. It features a multi-window interface using `QMdiArea`, live data plotting with `QtCharts`, and tools for device interaction, recording, and exporting data.

## Features

- 📈 Real-time data plotting with dynamic Y-axis scaling
- 🔌 Serial port communication using `QSerialPort`
- 🧭 Device discovery, configuration, calibration, and firmware update stubs
- 🗂 Multiple sub-windows: Graphs, 3D Visualizer, Data View, Device Status
- 🧰 Built-in taskbar for minimizing/restoring sub-windows
- 📤 Export chart as PNG or PDF
- 🖨 Print and export functionality
- 🎛 User-friendly menu bar and toolbar with icons


## How It Works

- The application uses a `QMdiArea` to manage sub-windows such as data graphs and status views.
- Serial data is read line-by-line and converted to double values.
- Values are plotted on a `QLineSeries` chart in real-time.
- Y-axis scales dynamically based on incoming data values.
- A taskbar allows minimizing and restoring individual sub-windows.

## Build & Run

### Requirements

- Qt 5 or 6 (Tested with Qt 5.15+)
- CMake or qmake
- C++17 or later

### Build Instructions

#### Using Qt Creator

1. Open the `.pro` or `CMakeLists.txt` file in Qt Creator.
2. Configure and build the project.
3. Run the executable.

#### Using Terminal

```bash
mkdir build && cd build
cmake ..
make
./SerialDataVisualizer
