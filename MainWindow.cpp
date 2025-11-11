#include "MainWindow.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPalette>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "CpuMonitorUsage.h"
#include "DiskInfo.h"
#include "ProcessInfo.h"
#include <QTableWidget>
#include<QHeaderView>
#include "Network.h"
#include "RamUsage.h"
#include "UsageGraph.h"


// CPU widget with actual monitoring
class CpuWidget : public QWidget
{
public:
    CpuWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);

        // Title
        QLabel *title = new QLabel("CPU Performance");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "QLabel { color: white; font-size: 18px; font-weight: 500; margin-bottom: 20px; }");
        layout->addWidget(title);

        // Create Usage Graph
        cpuGraph = new UsageGraph("CPU Usage", 0.0, 100.0, "%", this);
        cpuGraph->setMinimumHeight(350);
        layout->addWidget(cpuGraph);
        cpuGraph->setMaximumWidth(800); // Prevent horizontal stretching

        // CPU usage display
        cpuUsageLabel = new QLabel("Overall CPU Usage: 0.0%");
        cpuUsageLabel->setAlignment(Qt::AlignCenter);
        cpuUsageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(cpuUsageLabel);

        // Create and connect CPU monitor
        cpuMonitor = new CpuMonitorUsage(this);
        connect(cpuMonitor, &CpuMonitorUsage::usageUpdated, this, &CpuWidget::updateUsage);

        layout->addStretch(); // Push content to top
        setStyleSheet("QWidget { background-color: #1e1e1e; }");
    }

private slots:
    void updateUsage(double usage)
    {
        cpuUsageLabel->setText(
            QString("Overall CPU Usage: %1%").arg(QString::number(usage, 'f', 1)));
        cpuGraph->addUtilizationValue(usage); // giving the graph the cpu data
    }

private:
    QLabel *cpuUsageLabel;
    CpuMonitorUsage *cpuMonitor;
    UsageGraph *cpuGraph;
};

// Network widget
class NetWidget : public QWidget
{
public:
    NetWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);

        // Title
        QLabel *title = new QLabel("Network Performance");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "QLabel { color: white; font-size: 18px; font-weight: 500; margin-bottom: 20px; }");
        layout->addWidget(title);

        // container for graphs
        QHBoxLayout *graphLayout = new QHBoxLayout();
        graphLayout->setSpacing(20); // Add some spacing between graphs

        // bytes received graph
        recvGraph = new UsageGraph("Received", 0, 1000, " Kbps", this);
        recvGraph->setMinimumHeight(350);
        recvGraph->setMaximumWidth(400); // prevent horizontal stretching
        graphLayout->addWidget(recvGraph);

        // bytes sent graph
        sentGraph = new UsageGraph("Sent", 0, 1000, " Kbps", this);
        sentGraph->setMinimumHeight(350);
        sentGraph->setMaximumWidth(400);
        graphLayout->addWidget(sentGraph);
        // adds graphs to main layout
        layout->addLayout(graphLayout);

        // interface's name display
        interfaceLabel = new QLabel("Adapter: N/A");
        interfaceLabel->setAlignment(Qt::AlignCenter);
        interfaceLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(interfaceLabel);

        // connection type display
        connectionLabel = new QLabel("Connection Type: N/A");
        connectionLabel->setAlignment(Qt::AlignCenter);
        connectionLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(connectionLabel);

        // ipv6 address display
        ipv6Label = new QLabel("IPv6 Address: ::1");
        ipv6Label->setAlignment(Qt::AlignCenter);
        ipv6Label->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(ipv6Label);

        // ipv4 address display
        ipv4Label = new QLabel("IPv4 Address: 127.0.0.1");
        ipv4Label->setAlignment(Qt::AlignCenter);
        ipv4Label->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(ipv4Label);

        // Received bytes display
        bytesReceivedLabel = new QLabel("Received: 0.0 Kbps");
        bytesReceivedLabel->setAlignment(Qt::AlignCenter);
        bytesReceivedLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(bytesReceivedLabel);

        // Sent bytes display
        bytesSentLabel = new QLabel("Sent: 0.0 Kbps");
        bytesSentLabel->setAlignment(Qt::AlignCenter);
        bytesSentLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(bytesSentLabel);

        // Create and connect network monitor
        interfaceMonitor = new networkStats(this);
        connect(interfaceMonitor, &networkStats::updateIfaceData, this, &NetWidget::updateNetSpecs);
        connect(interfaceMonitor, &networkStats::updatedThroughput, this, &NetWidget::updateNetData);
        layout->addStretch(); // Push content to top
        setStyleSheet("QWidget { background-color: #1e1e1e; }");
    }

private slots:
    void updateNetSpecs(QString iface, QString type, QString ipv6, QString ipv4)
    {
        interfaceLabel->setText(QString("Adapter:  %1").arg(iface));
        connectionLabel->setText(QString("Connection Type:  %1").arg(type));
        ipv6Label->setText(QString("IPv6 Address:  %1").arg(ipv6));
        ipv4Label->setText(QString("IPv4 Address:  %1").arg(ipv4));
    }

    void updateNetData(quint64 recBits, QString recSpeed, quint64 senBits, QString senSpeed)
    {
        bytesReceivedLabel->setText(QString("Received:  %1 %2ps").arg(QString::number(recBits, 'f', 2), recSpeed));
        bytesSentLabel->setText(QString("Sent:  %1 %2ps").arg(QString::number(senBits, 'f', 2), senSpeed));


        if(recSpeed == "Kb"){
            recvGraph->setRange(0,1000);
        } else if(recSpeed == "Mb"){
            recvGraph->setRange(0,1000000);
            recBits = recBits * 1000;
        } else if(recSpeed == "Gb"){
            recvGraph->setRange(0,1000000 * 1000);
            recBits = recBits * 1000000;
        };


        if(senSpeed == "Kb"){
            sentGraph->setRange(0,1000);
        } else if(senSpeed == "Mb"){
            sentGraph->setRange(0,1000000);
        } else if(senSpeed == "Gb"){
            senBits = senBits * 1000;
            sentGraph->setRange(0,1000000 * 1000);
            senBits = senBits * 1000000;
        };


        recvGraph->setUnit(QString(" %1ps").arg(recSpeed));
        recvGraph->addUtilizationValue(recBits); // gives received graph its data

        sentGraph->setUnit(QString(" %1ps").arg(senSpeed));
        sentGraph->addUtilizationValue(senBits); // gives sent graph its data
    }

private:
    QLabel *interfaceLabel;
    QLabel *connectionLabel;
    QLabel *ipv6Label;
    QLabel *ipv4Label;
    QLabel *bytesReceivedLabel;
    QLabel *bytesSentLabel;
    networkStats *interfaceSpecs;
    networkStats *interfaceMonitor;
    UsageGraph *recvGraph;
    UsageGraph *sentGraph;
};

class RamWidget : public QWidget
{
public:
    RamWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);

        //title
        QLabel *title = new QLabel("Ram Performance");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "QLabel{ color: white; font-size: 18px; font-weight: 500; margin-bottom: 20px;}");
        layout->addWidget(title);

        ramUsageLabel = new QLabel("Memory Used: 0.0 GB / 0.0 GB");
        ramUsageLabel->setAlignment(Qt::AlignCenter);
        ramUsageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(ramUsageLabel);

        ramMonitor = new RamUsage(this);
        connect(ramMonitor, &RamUsage::ramUsageUpdated, this, &RamWidget::updateUsage);

        layout->addStretch();
        setStyleSheet("QWidget { background-color: #1e1e1e;}");
    }
private slots:
    void updateUsage(long usedRamKB) { ramUsageLabel->setText(ramMonitor->getRamUsageString()); }

private:
    QLabel *ramUsageLabel;
    RamUsage *ramMonitor;
};

class DiskWidget : public QWidget
{
public:
    DiskWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);

        QLabel *title = new QLabel("Disk Information");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "QLabel{ color: white; font-size: 18px; font-weight: 500; margin-bottom: 20px;}");
        layout->addWidget(title);

        diskUsageLabel = new QLabel("Reads Completed: 0 Writes Completed: 0\n"
                                    "Read Throughput: 1 MB/s Write Throughput: 2 MB/s");

        diskUsageLabel->setAlignment(Qt::AlignCenter);
        diskUsageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(diskUsageLabel);
        layout->addStretch();
        setStyleSheet("QWidget { background-color: #1e1e1e;}");

        diskMonitor = new DiskInfo(this);
        connect(diskMonitor, &DiskInfo::updateReads, this, &DiskWidget::updateDiskInfo);
        connect(diskMonitor, &DiskInfo::updateWrites, this, &DiskWidget::updateDiskInfo);
        connect(diskMonitor, &DiskInfo::updateReadThroughput, this, &DiskWidget::updateDiskInfo);
        connect(diskMonitor, &DiskInfo::updateWriteThroughput, this, &DiskWidget::updateDiskInfo);
    }
private slots:
    void updateDiskInfo() { diskUsageLabel->setText(diskMonitor->getDiskInfoString()); }

private:
    QLabel *diskUsageLabel;
    DiskInfo *diskMonitor;
};


class ProcessWidget: public QWidget
{
public:
    ProcessWidget(QWidget *parent = nullptr)
        :QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);

        QLabel *title = new QLabel("Process Information");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "QLabel{ color: white; font-size: 18px; font-weight: 500; margin-bottom: 20px;}");
        layout->addWidget(title);

        //table for all processes
        processTable = new QTableWidget(this);
        processTable->setColumnCount(5);
        processTable->setHorizontalHeaderLabels({"PID", "Name", "CPU %", "RAM", "I/O"});
        processTable->horizontalHeader()->setStretchLastSection(true);
        processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        processTable->setStyleSheet(
            "QTableWidget { background-color: #2d2d2d; color: white; }"
            "QHeaderView::section { background-color: #3d3d3d; color: white; padding: 5px; }"
        );

        layout->addWidget(processTable);
        processMonitor = new ProcessInfo(this);
        connect(processMonitor, &ProcessInfo::processesUpdated, this, &ProcessWidget::updateProcesses);
        setStyleSheet("QWidget { background-color: #1e1e1e;}");


    }
private slots:
    void updateProcesses(std::vector<ProcessUsage> processes)
    {
        qDebug() << "updateProcesses called with" << processes.size();
        processTable->setRowCount(processes.size());
        for(size_t i = 0; i < processes.size(); ++i)
        {
            const ProcessUsage &proc = processes[i];
            qDebug() << "Process" << i << ":" << proc.PID << proc.name;

            processTable->setItem(i, 0, new QTableWidgetItem(QString::number(proc.PID)));
            processTable->setItem(i, 1, new QTableWidgetItem(proc.name));
            processTable->setItem(i, 2, new QTableWidgetItem(QString::number(proc.cpuUsage, 'f', 2 )));
            processTable->setItem(i, 3, new QTableWidgetItem(QString::number(proc.ramUsage, 'f', 2) + " MB"));
            processTable->setItem(i, 4, new QTableWidgetItem("Bytes Read: " + QString("%1").arg(proc.bytesRead) +
                                                             " Bytes Written: " + QString("%1").arg(proc.bytesWritten)));

        }
    }

private:
    QTableWidget *processTable;
    ProcessInfo *processMonitor;
};




// placeholder widget for other tab pages
class PlaceholderWidget : public QWidget
{
public:
    PlaceholderWidget(const QString &title, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);
        //temp display for the tab
        QLabel *label = new QLabel(title + " Performance");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { color: white; font-size: 18px; font-weight: 500; }");
        layout->addWidget(label);
        setStyleSheet("QWidget { background-color: #1e1e1e; }");
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , performanceSidebar(nullptr)
    , contentStack(nullptr)
    , mainLayout(nullptr)
{
    setWindowTitle("Task Manager");
    resize(1000, 600);
    setupUI();
}

MainWindow::~MainWindow()
{
    // Qt handles cleanup automatically
}

void MainWindow::setupUI()
{
    //calling the functions
    setupLayout();
    setupPerformanceSidebar();
    setupContentStack();
    connectSignals();
    applyStyles();
}

void MainWindow::setupLayout()
{
    centralWidget = new QWidget(this);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(centralWidget);
}

void MainWindow::setupPerformanceSidebar()
{
    //using a QList to store our tabs and keep index
    performanceSidebar = new QListWidget();
    performanceSidebar->addItems({"CPU", "Memory", "Disk", "Network", "Processes"});
    performanceSidebar->setMinimumWidth(140);
    performanceSidebar->setMaximumWidth(180);
    performanceSidebar->setCurrentRow(0); // Start with CPU selected
    mainLayout->addWidget(performanceSidebar, 1);
}

void MainWindow::setupContentStack()
{
    //stack of pages that we will display based on our QList index
    contentStack = new QStackedWidget();

    // Add CPU widget with actual monitoring
    contentStack->addWidget(new CpuWidget());
    contentStack->addWidget(new RamWidget());
    contentStack->addWidget(new DiskWidget());
    contentStack->addWidget(new NetWidget());
    contentStack->addWidget(new ProcessWidget());

    // Add placeholder widgets for other performance tabs
    QStringList tabs = {"Disk", "Processes"};
    for (const QString &tab : tabs) {
        contentStack->addWidget(new PlaceholderWidget(tab));
    }

    mainLayout->addWidget(contentStack, 5);
}

void MainWindow::connectSignals()
{
    //when you click on a new tab the click signals the onPerformanceTab
    connect(performanceSidebar,
            &QListWidget::currentRowChanged,
            this,
            &MainWindow::onPerformanceTabChanged);
}

//takes the index of the tab you selected and changes the contentStack to your tab page
void MainWindow::onPerformanceTabChanged(int index)
{
    contentStack->setCurrentIndex(index);
}

//styling
void MainWindow::applyStyles()
{
    // Main window styling
    setStyleSheet("QMainWindow { background-color: rgb(45, 45, 45); }");

    // Direct sidebar styling (removed AppTheme complexity)
    performanceSidebar->setStyleSheet(R"(
        QListWidget {
            background-color: #2d2d2d;
            border: none;
            font-size: 15px;
            font-weight: 400;
            border-right: 1px solid rgba(255,255,255,0.1);
            outline: none;
        }
        QListWidget::item {
            padding: 12px 16px;
            height: 40px;
            margin: 2px 4px;
            color: #d1d5db;
            border-bottom: 1px solid rgba(255,255,255,0.08);
        }
        QListWidget::item:hover {
            background-color: #374151;
        }
        QListWidget::item:selected {
            background-color: #7c3aed;
            color: white;
            font-weight: 500;
        }
        QListWidget::item:selected:hover {
            background-color: #8b5cf6;
        }
    )");
}
