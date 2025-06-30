// mainwindow.cpp
#include "mainwindow.h"
#include "portdialog.h"
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QApplication>
#include <QStyle>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mdiArea = new QMdiArea(this);
    // mdiArea->setStyleSheet("background-color: white;");

    taskBarWidget = new QWidget(this);
    taskBarLayout = new QHBoxLayout(taskBarWidget);
    taskBarLayout->setContentsMargins(5, 5, 5, 5);
    taskBarLayout->setSpacing(5);

    QWidget *central = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(central);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(mdiArea);
    centralLayout->addWidget(taskBarWidget);
    setCentralWidget(central);

    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, [this]() {
        while (serial->canReadLine()) {
            QByteArray line = serial->readLine().trimmed();
            bool ok;
            double value = line.toDouble(&ok);
            if (ok && series) {
                series->append(dataPointIndex++, value);

                // Add to rolling history
                yValueHistory.enqueue(value);
                if (yValueHistory.size() > maxHistorySize)
                    yValueHistory.dequeue();

                // Auto-expand Y
                if (value > maxYValue) {
                    maxYValue = value + 1;
                    chart->axisY()->setRange(0, maxYValue);
                }

                // Auto-scroll X
                if (dataPointIndex > 100) {
                    chart->scroll(chart->plotArea().width()/100, 0);
                    chart->axisX()->setRange(dataPointIndex - 100, dataPointIndex);
                }

                lastReceivedValue = value;
                lastUpdateTime = QDateTime::currentDateTime();

                if (statusLabel) {
                    statusLabel->setText(
                        QString("Status: Connected\n"
                                "Port: %1\n"
                                "Last Value: %2\n"
                                "Last Updated: %3")
                            .arg(currentPortName.isEmpty() ? "N/A" : currentPortName)
                            .arg(lastReceivedValue)
                            .arg(lastUpdateTime.toString("hh:mm:ss")));
                }

            }
        }
    });


    shrinkTimer = new QTimer(this);
    shrinkTimer->setInterval(1000);  // check every 1 second
    connect(shrinkTimer, &QTimer::timeout, this, &MainWindow::checkAutoShrinkYAxis);
    shrinkTimer->start();

    setupMenuBar();
    setupToolBar();
    setupTaskBar();
    createPositionedSubWindows();
}

MainWindow::~MainWindow() {}


void MainWindow::checkAutoShrinkYAxis() {
    if (yValueHistory.isEmpty()) return;

    double currentMax = *std::max_element(yValueHistory.begin(), yValueHistory.end());

    double suggestedMax = currentMax + 1;
    if (suggestedMax < maxYValue - 20) {
        maxYValue = suggestedMax;
        chart->axisY()->setRange(0, maxYValue);
        qDebug() << "Shrinking Y-axis to:" << maxYValue;
    }
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Open File", this, &MainWindow::openFile);
    fileMenu->addAction("Close", this, &MainWindow::closeFile);
    fileMenu->addAction("Export Chart as Image", this, &MainWindow::exportChartAsImage);
    fileMenu->addAction("Export Chart as PDF", this, &MainWindow::exportChartAsPdf);

    fileMenu->addAction("Print", this, &MainWindow::print);
    fileMenu->addAction("Setting", this, &MainWindow::openSettings);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &MainWindow::exitApp);

    QMenu *deviceMenu = menuBar()->addMenu("Device");
    deviceMenu->addAction("Open Port", this, &MainWindow::openPort);
    deviceMenu->addAction("Disconnect Port", this, &MainWindow::disconnectPort);
    deviceMenu->addAction("Discover", this, &MainWindow::discoverDevices);
    deviceMenu->addAction("Start Reading Data / Stop", this, &MainWindow::toggleReading);

    QMenu *readingMenu = menuBar()->addMenu("Reading Data");
    readingMenu->addAction("Start Recording Data / Stop", this, &MainWindow::toggleRecording);

    QMenu *recordingMenu = menuBar()->addMenu("Recording Data");
    recordingMenu->addAction("Configure", this, &MainWindow::configureDevice);
    recordingMenu->addAction("Calibrate", this, &MainWindow::calibrateDevice);
    recordingMenu->addAction("Update Firmware", this, &MainWindow::updateFirmware);

    QMenu *viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction("Data Graphs Window", this, &MainWindow::openGraphsWindow);
    viewMenu->addAction("3D Visualizer Window", this, &MainWindow::open3DVisualizerWindow);
    viewMenu->addAction("Data View Window", this, &MainWindow::openDataViewWindow);
    viewMenu->addAction("Device Status Window", this, &MainWindow::openDeviceStatusWindow);
    viewMenu->addSeparator();
    viewMenu->addAction("Tile Windows", this, &MainWindow::tileWindows);
    viewMenu->addAction("Cascade Windows", this, &MainWindow::cascadeWindows);
    viewMenu->addAction("Minimize All Windows", this, &MainWindow::minimizeAllSubWindows);
    viewMenu->addAction("Reset Windows", this, &MainWindow::resetLayout);

    QMenu *helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction("Documentation", this, &MainWindow::openDocumentation);
    helpMenu->addAction("Support", this, &MainWindow::openSupport);
    helpMenu->addAction("About", this, &MainWindow::openAbout);
}

void MainWindow::setupToolBar() {
    QToolBar *toolBar = addToolBar("Main Toolbar");

    QAction *openFileAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton), "Open File", this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    toolBar->addAction(openFileAction);

    QAction *closeFileAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton), "Close File", this);
    connect(closeFileAction, &QAction::triggered, this, &MainWindow::closeFile);
    toolBar->addAction(closeFileAction);

    QAction *exportAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), "Export", this);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportData);
    toolBar->addAction(exportAction);

    toolBar->addSeparator();

    QAction *openPortAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), "Open Port", this);
    connect(openPortAction, &QAction::triggered, this, &MainWindow::openPort);
    toolBar->addAction(openPortAction);

    QAction *disconnectPortAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton), "Disconnect Port", this);
    connect(disconnectPortAction, &QAction::triggered, this, &MainWindow::disconnectPort);
    toolBar->addAction(disconnectPortAction);

    QAction *discoverAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon), "Discover", this);
    connect(discoverAction, &QAction::triggered, this, &MainWindow::discoverDevices);
    toolBar->addAction(discoverAction);

    QAction *playAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), "Play", this);
    connect(playAction, &QAction::triggered, this, &MainWindow::toggleReading);
    toolBar->addAction(playAction);

    // QAction *recordAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaRecord), "Record", this);
    // connect(recordAction, &QAction::triggered, this, &MainWindow::toggleRecording);
    // toolBar->addAction(recordAction);

    QAction *stopAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MediaStop), "Stop", this);
    connect(stopAction, &QAction::triggered, this, &MainWindow::resetLayout);
    toolBar->addAction(stopAction);

    QAction *configureAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Configure", this);
    connect(configureAction, &QAction::triggered, this, &MainWindow::configureDevice);
    toolBar->addAction(configureAction);

    toolBar->addSeparator();

    QAction *graphsAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DesktopIcon), "Graphs", this);
    connect(graphsAction, &QAction::triggered, this, &MainWindow::openGraphsWindow);
    toolBar->addAction(graphsAction);

    QAction *object3DAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_DriveCDIcon), "3D Object", this);
    connect(object3DAction, &QAction::triggered, this, &MainWindow::open3DVisualizerWindow);
    toolBar->addAction(object3DAction);

    QAction *dataViewAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_FileIcon), "Data View", this);
    connect(dataViewAction, &QAction::triggered, this, &MainWindow::openDataViewWindow);
    toolBar->addAction(dataViewAction);

    QAction *deviceStatusAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), "Device Status", this);
    connect(deviceStatusAction, &QAction::triggered, this, &MainWindow::openDeviceStatusWindow);
    toolBar->addAction(deviceStatusAction);
}

void MainWindow::setupTaskBar() {
    taskBarWidget->setStyleSheet("background-color: #ddd;");
}
void MainWindow::createPositionedSubWindows() {
    QStringList names = { "Data Graphs", "3D Visualizer", "Data View", "Device Status" };

    for (const QString &name : names) {
        QWidget *content = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(content);

        if (name == "Data View") {
            series = new QLineSeries();
            chart = new QChart();
            chart->legend()->hide();
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axisX()->setRange(0, 100);
            chart->axisY()->setRange(0, maxYValue);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            layout->addWidget(chartView);
        }

        if (name == "Device Status") {
            statusLabel = new QLabel("Disconnected");
            statusLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            layout->addWidget(statusLabel);
        }

        content->setLayout(layout);
        QMdiSubWindow *sub = mdiArea->addSubWindow(content);
        sub->setWindowTitle(name);

        // Set predefined icon based on the window name
        QIcon icon;
        if (name == "Data Graphs") {
            icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
        } else if (name == "3D Visualizer") {
            icon = QApplication::style()->standardIcon(QStyle::SP_DesktopIcon);
        } else if (name == "Data View") {
            icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
        } else if (name == "Device Status") {
            icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation);
        }
        sub->setWindowIcon(icon);

        sub->resize(400, 300);
        sub->show();
        addMinimizeContext(sub);
    }

    mdiArea->tileSubWindows();
}


void MainWindow::addMinimizeContext(QMdiSubWindow *subWindow) {
    QAction *minimizeAct = new QAction("Minimize", subWindow);
    subWindow->addAction(minimizeAct);
    subWindow->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(minimizeAct, &QAction::triggered, this, [this, subWindow]() {
        minimizeSubWindow(subWindow);
    });
}

void MainWindow::minimizeSubWindow(QMdiSubWindow *subWin) {
    subWin->hide();
    QPushButton *btn = new QPushButton(subWin->windowTitle(), taskBarWidget);
    taskBarLayout->addWidget(btn);
    taskBarButtons[btn] = subWin;
    connect(btn, &QPushButton::clicked, this, &MainWindow::restoreSubWindow);
}

void MainWindow::restoreSubWindow() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !taskBarButtons.contains(btn)) return;

    QMdiSubWindow *subWin = taskBarButtons[btn];
    subWin->showNormal();
    subWin->show();

    taskBarLayout->removeWidget(btn);
    btn->deleteLater();
    taskBarButtons.remove(btn);
}

#define DEFINE_SLOT(name) void MainWindow::name() { qDebug() << #name " triggered"; }
DEFINE_SLOT(closeFile)
DEFINE_SLOT(exportData)
DEFINE_SLOT(print)
DEFINE_SLOT(openSettings)
DEFINE_SLOT(exitApp)
DEFINE_SLOT(discoverDevices)
DEFINE_SLOT(toggleReading)
DEFINE_SLOT(toggleRecording)
DEFINE_SLOT(configureDevice)
DEFINE_SLOT(calibrateDevice)
DEFINE_SLOT(updateFirmware)
DEFINE_SLOT(openGraphsWindow)
DEFINE_SLOT(open3DVisualizerWindow)
DEFINE_SLOT(openDataViewWindow)
DEFINE_SLOT(openDeviceStatusWindow)
DEFINE_SLOT(openDocumentation)
DEFINE_SLOT(openSupport)
DEFINE_SLOT(openAbout)

void MainWindow::openFile() {
    qDebug() << "Open File triggered";
}



void MainWindow::openPort() {
    if (serial->isOpen()) {
        serial->close();
    }

    PortDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString portName = dialog.selectedPort();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "No Port", "No port selected.");
        return;
    }

    currentPortName = portName;

    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open port.");
    } else {
        lastUpdateTime = QDateTime::currentDateTime();
        QString statusText = QString("Status: Connected\n"
                                     "Port: %1\n"
                                     "Baud Rate: %2\n"
                                     "Last Update: %3")
                                 .arg(currentPortName)
                                 .arg(serial->baudRate())
                                 .arg(lastUpdateTime.toString("yyyy-MM-dd hh:mm:ss"));

        if (statusLabel) {
            statusLabel->setText(statusText);
        }

        QMessageBox::information(this, "Port Opened", "Connected to " + portName);
    }
}

void MainWindow::disconnectPort() {
    if (serial->isOpen()) {
        serial->close();
        if (statusLabel) {
            statusLabel->setText("Status: Disconnected");
        }
        QMessageBox::information(this, "Disconnected", "Serial port disconnected.");
    } else {
        QMessageBox::information(this, "Info", "No serial port is currently open.");
    }
}



void MainWindow::cascadeWindows() {
    for (auto win : mdiArea->subWindowList()) {
        if (win->isHidden()) win->show();
    }
    mdiArea->cascadeSubWindows();
}

void MainWindow::tileWindows() {
    for (auto win : mdiArea->subWindowList()) {
        if (win->isHidden()) win->show();
    }
    mdiArea->tileSubWindows();
}

void MainWindow::minimizeAllSubWindows() {
    for (auto win : mdiArea->subWindowList()) {
        if (!win->isHidden()) minimizeSubWindow(win);
    }
}

void MainWindow::resetLayout() {
    QSize mdiSize = mdiArea->size();
    int w = mdiSize.width() / 2;
    int h = mdiSize.height() / 2;

    QPoint positions[] = {
        QPoint(0, 0), QPoint(w, 0),
        QPoint(0, h), QPoint(w, h)
    };

    int i = 0;
    for (QMdiSubWindow *win : mdiArea->subWindowList()) {
        if (win->isHidden()) win->show();
        if (i < 4) {
            win->resize(w - 20, h - 20);
            win->move(positions[i]);
        }
        ++i;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    resetLayout();
}
void MainWindow::exportChartAsImage() {
    if (!chartView) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Chart Image", "", "PNG Image (*.png)");
    if (fileName.isEmpty()) return;

    QPixmap pixmap(chartView->size());
    QPainter painter(&pixmap);
    chartView->render(&painter);
    painter.end();


    if (!pixmap.save(fileName)) {
        QMessageBox::warning(this, "Export Failed", "Could not save the image.");
    } else {
        QMessageBox::information(this, "Export Successful", "Chart saved as image.");
    }
}

void MainWindow::exportChartAsPdf() {
    if (!chartView) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Chart PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&writer);

    // Scale chartView to fit PDF page
    QSize viewSize = chartView->size();
    QSize pdfSize(writer.width(), writer.height());
    qreal xScale = pdfSize.width() / qreal(viewSize.width());
    qreal yScale = pdfSize.height() / qreal(viewSize.height());
    qreal scale = qMin(xScale, yScale);
    painter.scale(scale, scale);

    chartView->render(&painter);
    painter.end();

    QMessageBox::information(this, "Export Successful", "Chart saved as PDF.");
}
