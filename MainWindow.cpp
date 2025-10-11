#include "MainWindow.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPalette>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "CpuMonitorUsage.h"
#include "RamUsage.h"
#include "Network.h"

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
    }

private:
    QLabel *cpuUsageLabel;
    CpuMonitorUsage *cpuMonitor;
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
        bytesReceivedLabel = new QLabel("Received: 0.0 Kb/s");
        bytesReceivedLabel->setAlignment(Qt::AlignCenter);
        bytesReceivedLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(bytesReceivedLabel);

        // Sent bytes display
        bytesSentLabel = new QLabel("Sent: 0.0 Kb/s");
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
        interfaceLabel->setText(
            QString("Adapter: %1").arg(iface));
            connectionLabel->setText(
                QString("Connection Type: %1").arg(type));
            ipv6Label->setText(
                QString("IPv6 Address: %1").arg(ipv6));
            ipv4Label->setText(
                QString("IPv4 Address: %1").arg(ipv4));
    }

    void updateNetData(QString received, QString sent)
    {
        bytesReceivedLabel->setText(
            QString("Received: %1").arg(received));
        bytesSentLabel->setText(
            QString("Sent: %1").arg(sent));
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
};


class RamWidget: public QWidget
{
public:
    RamWidget(QWidget *parent = nullptr)
        :QWidget(parent)
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
    void updateUsage(long usedRamKB)
    {
        ramUsageLabel->setText(ramMonitor->getRamUsageString());
    }
private:
    QLabel *ramUsageLabel;
    RamUsage *ramMonitor;

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
    contentStack->addWidget(new PlaceholderWidget("Disk"));
    contentStack->addWidget(new NetWidget());
    contentStack->addWidget(new PlaceholderWidget("Processes"));


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
