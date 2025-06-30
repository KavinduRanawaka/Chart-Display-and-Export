#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QMap>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QSerialPort>
#include <QLabel>
#include <QDateTime>

#include <QQueue>
#include <QTimer>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_USE_NAMESPACE

    class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // File
    void openFile();
    void closeFile();
    void exportData();
    void exportChartAsImage();
    void exportChartAsPdf();
    void print();
    void openSettings();
    void exitApp();

    // Device
    void openPort();
    void disconnectPort();
    void discoverDevices();
    void toggleReading();

    // Reading Data
    void toggleRecording();

    // Recording Data
    void configureDevice();
    void calibrateDevice();
    void updateFirmware();

    // View Windows
    void openGraphsWindow();
    void open3DVisualizerWindow();
    void openDataViewWindow();
    void openDeviceStatusWindow();

    // Window Management
    void cascadeWindows();
    void tileWindows();
    void minimizeAllSubWindows();
    void resetLayout();

    // Help
    void openDocumentation();
    void openSupport();
    void openAbout();

    void restoreSubWindow();
    void minimizeSubWindow(QMdiSubWindow *subWin);

private:
    // UI Layout
    QMdiArea *mdiArea;
    QWidget *taskBarWidget;
    QHBoxLayout *taskBarLayout;
    QMap<QPushButton*, QMdiSubWindow*> taskBarButtons;

    QLabel *statusLabel = nullptr;
    QString currentPortName;
    int lastReceivedValue = 0;
    QDateTime lastUpdateTime;



    // Serial Communication
    QSerialPort *serial;
    QPlainTextEdit *dataViewEdit;

    // Data View Chart
    QChart *chart = nullptr;
    QChartView *chartView = nullptr;
    QLineSeries *series = nullptr;



    // Chart tracking
    int dataPointIndex = 0;
    int maxYValue = 50;

    // Auto-shrink Y-axis
    QQueue<int> yValueHistory;
    QTimer *shrinkTimer = nullptr;
    const int maxHistorySize = 100;

    // Setup methods
    void setupMenuBar();
    void setupToolBar();
    void setupTaskBar();
    void createPositionedSubWindows();
    void addMinimizeContext(QMdiSubWindow *subWindow);
    void checkAutoShrinkYAxis();
    void update3DVisualizer(int index, int value);
};

#endif // MAINWINDOW_H

