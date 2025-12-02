#include "MainWindow.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QPalette>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include "CpuMonitorUsage.h"
#include "DiskInfo.h"
#include "Network.h"
#include "ProcessInfo.h"
#include "RamUsage.h"
#include "UsageGraph.h"
#include "PageCustomization.h"

// CPU widget with actual monitoring
class CpuWidget : public QWidget
{
public:
    CpuWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 24, 24, 24);
        layout->setSpacing(20);

        // Header
        QHBoxLayout *headerLayout = new QHBoxLayout();
        QLabel *title = new QLabel("CPU");
        title->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
        cpuModelLabel = new QLabel("...");
        cpuModelLabel->setStyleSheet("font-size: 14px; color: rgba(255, 255, 255, 0.7);");
        cpuModelLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
        headerLayout->addWidget(title);
        headerLayout->addWidget(cpuModelLabel, 1);
        layout->addLayout(headerLayout);

        // Graph at the top
        cpuGraph = new UsageGraph("CPU Usage", 0.0, 100.0, "%", this);
        cpuGraph->setMinimumHeight(300);
        layout->addWidget(cpuGraph);

        // Data grid below
        QGridLayout *dataGrid = new QGridLayout();
        dataGrid->setHorizontalSpacing(40);
        dataGrid->setVerticalSpacing(15);

        // Left column
        addDataRow(dataGrid, 0, "Utilization:", &utilLabel);
        addDataRow(dataGrid, 1, "Processes:", &processesLabel);
        addDataRow(dataGrid, 2, "Threads:", &threadsLabel);
        addDataRow(dataGrid, 3, "Up time:", &uptimeLabel);

        // Right column
        addDataRow(dataGrid, 0, "Sockets:", &socketsLabel, 2);
        addDataRow(dataGrid, 1, "Cores:", &coresLabel, 2);

        layout->addLayout(dataGrid);
        layout->addStretch();

        // Change background color button
        backgroundColor_btn = new QPushButton("Change Background Color");
        backgroundColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(backgroundColor_btn);

        // Change text color button
        textColor_btn = new QPushButton("Change Content Color");
        textColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(textColor_btn);

        // Apply page styling to all pages button
        applyAllPages_btn = new QPushButton("Apply Styling To All Pages");
        applyAllPages_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(applyAllPages_btn);

        // Connects background color button to color dialog box (color wheel)
        connect(backgroundColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setBackgroundColorDialog(this);
        });

        // Connects background color button to color dialog box (color wheel)
        connect(textColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setTextColorDialog(this);
        });

        connect(applyAllPages_btn, &QPushButton::clicked, this, [this]() {
            QStackedWidget* stack = qobject_cast<QStackedWidget*>(parentWidget());
            if (stack) {
                QList<QWidget*> pages;
                for (int i = 0; i < stack->count(); ++i) {
                    pages.append(stack->widget(i));
                }
                ColorUtils::setAllStyles(this, pages);
            }
        });

        // Connect to monitor
        cpuMonitor = new CpuMonitorUsage(this);
        connect(cpuMonitor, &CpuMonitorUsage::usageUpdated, this, &CpuWidget::updateUsage);
        connect(cpuMonitor, &CpuMonitorUsage::cpuInfoUpdated, this, &CpuWidget::updateCpuInfo);

        const QVector<double> history = cpuMonitor->getUtilizationHistory();
        for (double value : history) {
            cpuGraph->addUtilizationValue(value);
        }

        setStyleSheet("QWidget { background-color: #1e1e1e; }");
    }

private:
    void addDataRow(
        QGridLayout *grid, int row, const QString &label, QLabel **valueLabel, int colOffset = 0)
    {
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 14px;");

        QLabel *val = new QLabel("...");
        val->setStyleSheet("color: white; font-size: 14px; font-weight: 500;");
        *valueLabel = val;

        grid->addWidget(lbl, row, 0 + colOffset);
        grid->addWidget(val, row, 1 + colOffset);
    }

private slots:
    void updateUsage(double usage)
    {
        utilLabel->setText(QString::number(usage, 'f', 1) + "%");
        cpuGraph->addUtilizationValue(usage);
    }

    void updateCpuInfo(const CpuInfo &info)
    {
        cpuModelLabel->setText(info.modelName);
        processesLabel->setText(QString::number(info.processes));
        threadsLabel->setText(QString::number(info.threads));
        uptimeLabel->setText(info.uptime);
        socketsLabel->setText(QString::number(info.sockets));
        coresLabel->setText(QString::number(info.coresPerSocket));
    }

private:
    QLabel *cpuModelLabel;
    QLabel *utilLabel, *processesLabel, *threadsLabel, *uptimeLabel;
    QLabel *socketsLabel, *coresLabel;
    CpuMonitorUsage *cpuMonitor;
    UsageGraph *cpuGraph;
    QPushButton *backgroundColor_btn;
    QPushButton *textColor_btn;
    QPushButton *applyAllPages_btn;
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

        // Change background color button
        backgroundColor_btn = new QPushButton("Change Background Color");
        backgroundColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(backgroundColor_btn);

        // Change text color button
        textColor_btn = new QPushButton("Change Content Color");
        textColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(textColor_btn);

        // Apply page styling to all pages button
        applyAllPages_btn = new QPushButton("Apply Styling To All Pages");
        applyAllPages_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(applyAllPages_btn);

        // Connects background color button to color dialog box (color wheel)
        connect(backgroundColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setBackgroundColorDialog(this);
        });

        // Connects background color button to color dialog box (color wheel)
        connect(textColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setTextColorDialog(this);
        });

        connect(applyAllPages_btn, &QPushButton::clicked, this, [this]() {
            QStackedWidget* stack = qobject_cast<QStackedWidget*>(parentWidget());
            if (stack) {
                QList<QWidget*> pages;
                for (int i = 0; i < stack->count(); ++i) {
                    pages.append(stack->widget(i));
                }
                ColorUtils::setAllStyles(this, pages);
            }
        });

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
        bytesReceivedLabel->setText(
            QString("Received:  %1 %2ps").arg(QString::number(recBits, 'f', 2), recSpeed));
        bytesSentLabel->setText(
            QString("Sent:  %1 %2ps").arg(QString::number(senBits, 'f', 2), senSpeed));

        if (recSpeed == "Kb") {
            // These repetitive if statements stop the graphs from being continuously redrawn every time the update function is called (1 second interval)
            if (currentSpeed_Rec != recSpeed) {
                recvGraph->setRange(0, 1000);
            };
        } else if (recSpeed == "Mb") {
            recBits = recBits * 1000;
            if (currentSpeed_Rec != recSpeed) {
                recvGraph->setRange(0, 1000000);
            };
        } else if (recSpeed == "Gb") {
            recBits = recBits * 1000000;
            if (currentSpeed_Rec != recSpeed) {
                recvGraph->setRange(0, 1000000 * 1000);
            };
        };


        if (senSpeed == "Kb") {
            if (currentSpeed_Sen != senSpeed) {
                sentGraph->setRange(0, 1000);
            };
        } else if (senSpeed == "Mb") {
            senBits = senBits * 1000;
            if (currentSpeed_Sen != senSpeed) {
                sentGraph->setRange(0, 1000000);
            };
        } else if (senSpeed == "Gb") {
            senBits = senBits * 1000000;
            if (currentSpeed_Sen != senSpeed) {
                sentGraph->setRange(0, 1000000 * 1000);
            };
        };

        currentSpeed_Rec = recSpeed;
        currentSpeed_Sen = senSpeed;

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
    QString currentSpeed_Sen;
    QString currentSpeed_Rec;
    UsageGraph *recvGraph;
    UsageGraph *sentGraph;
    QPushButton *backgroundColor_btn;
    QPushButton *textColor_btn;
    QPushButton *applyAllPages_btn;
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

        ramMonitor = new RamUsage(this);
        // Create Usage Graph
        double const range = (ramMonitor->getTotalSysRam()) / (1024.0 * 1024.0);
        ramGraph = new UsageGraph("Ram Usage", 0.0, range, "GB", this);
        ramGraph->setMinimumHeight(350);
        layout->addWidget(ramGraph);
        ramGraph->setMaximumWidth(800); // Prevent horizontal stretching

        ramUsageLabel = new QLabel("Memory Used: 0.0 GB / 0.0 GB");
        ramUsageLabel->setAlignment(Qt::AlignCenter);
        ramUsageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(ramUsageLabel);

        // Change background color button
        backgroundColor_btn = new QPushButton("Change Background Color");
        backgroundColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(backgroundColor_btn);

        // Change text color button
        textColor_btn = new QPushButton("Change Content Color");
        textColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(textColor_btn);

        // Apply page styling to all pages button
        applyAllPages_btn = new QPushButton("Apply Styling To All Pages");
        applyAllPages_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(applyAllPages_btn);

        // Connects background color button to color dialog box (color wheel)
        connect(backgroundColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setBackgroundColorDialog(this);
        });

        // Connects background color button to color dialog box (color wheel)
        connect(textColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setTextColorDialog(this);
        });

        connect(applyAllPages_btn, &QPushButton::clicked, this, [this]() {
            QStackedWidget* stack = qobject_cast<QStackedWidget*>(parentWidget());
            if (stack) {
                QList<QWidget*> pages;
                for (int i = 0; i < stack->count(); ++i) {
                    pages.append(stack->widget(i));
                }
                ColorUtils::setAllStyles(this, pages);
            }
        });

        // Connects monitor to update ram usage functions
        connect(ramMonitor, &RamUsage::ramUsageUpdated, this, &RamWidget::updateUsage);

        layout->addStretch();
        setStyleSheet("QWidget { background-color: #1e1e1e;}");
    }
private slots:
    void updateUsage(long usedRamKB)
    {
        double usedRamGB = usedRamKB / (1024.0 * 1024.0);
        ramUsageLabel->setText(ramMonitor->getRamUsageString());
        ramGraph->addUtilizationValue(usedRamGB);
    }

private:
    QLabel *ramUsageLabel;
    RamUsage *ramMonitor;
    UsageGraph *ramGraph;
    QPushButton *backgroundColor_btn;
    QPushButton *textColor_btn;
    QPushButton *applyAllPages_btn;
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

        diskMonitor = new DiskInfo(this);

        // container for graphs
        QHBoxLayout *graphLayout = new QHBoxLayout();
        graphLayout->setSpacing(20); // Add some spacing between graphs

        readGraph = new UsageGraph("Read Throughput", 0.0, 3000.00, "MB/s", this);
        readGraph->setMinimumHeight(350);
        layout->addWidget(readGraph);
        readGraph->setMaximumWidth(400); // Prevent horizontal stretching
        graphLayout->addWidget(readGraph);
        layout->addLayout(graphLayout);

        writeGraph = new UsageGraph("Write Throughput", 0.0, 3000.00, "MB/s", this);
        writeGraph->setMinimumHeight(350);
        layout->addWidget(writeGraph);
        writeGraph->setMaximumWidth(400); // Prevent horizontal stretching
        graphLayout->addWidget(writeGraph);

        diskUsageLabel = new QLabel("Reads Completed: 0 Writes Completed: 0\n"
                                    "Read Throughput: 1 MB/s Write Throughput: 2 MB/s");

        diskUsageLabel->setAlignment(Qt::AlignCenter);
        diskUsageLabel->setStyleSheet("QLabel { color: white; font-size: 18px; }");
        layout->addWidget(diskUsageLabel);

        // Change background color button
        backgroundColor_btn = new QPushButton("Change Background Color");
        backgroundColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(backgroundColor_btn);

        // Change text color button
        textColor_btn = new QPushButton("Change Content Color");
        textColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(textColor_btn);

        // Apply page styling to all pages button
        applyAllPages_btn = new QPushButton("Apply Styling To All Pages");
        applyAllPages_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(applyAllPages_btn);

        // Connects background color button to color dialog box (color wheel)
        connect(backgroundColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setBackgroundColorDialog(this);
        });

        // Connects background color button to color dialog box (color wheel)
        connect(textColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setTextColorDialog(this);
        });

        connect(applyAllPages_btn, &QPushButton::clicked, this, [this]() {
            QStackedWidget* stack = qobject_cast<QStackedWidget*>(parentWidget());
            if (stack) {
                QList<QWidget*> pages;
                for (int i = 0; i < stack->count(); ++i) {
                    pages.append(stack->widget(i));
                }
                ColorUtils::setAllStyles(this, pages);
            }
        });

        connect(diskMonitor, &DiskInfo::updateReads, this, &DiskWidget::updateDiskInfo);
        connect(diskMonitor, &DiskInfo::updateWrites, this, &DiskWidget::updateDiskInfo);
        connect(diskMonitor,
                &DiskInfo::updateReadThroughput,
                this,
                &DiskWidget::updateReadThroughputGraph);
        connect(diskMonitor,
                &DiskInfo::updateWriteThroughput,
                this,
                &DiskWidget::updateWriteThroughputGraph);

        layout->addStretch();
        setStyleSheet("QWidget { background-color: #1e1e1e;}");
    }
private slots:
    void updateDiskInfo() { diskUsageLabel->setText(diskMonitor->getDiskInfoString()); }

    void updateReadThroughputGraph(double readBytesPerSec)
    {
        double readMBps = readBytesPerSec / (1024.0 * 1024.0);
        qDebug() << "Read throughput:" << readMBps << "MB/s";
        readGraph->addUtilizationValue(readMBps);
    }

    void updateWriteThroughputGraph(double writtenBytesPerSec)
    {
        double writeMBps = writtenBytesPerSec / (1024.0 * 1024.0);
        writeGraph->addUtilizationValue(writeMBps);
    }

private:
    QLabel *diskUsageLabel;
    DiskInfo *diskMonitor;
    UsageGraph *readGraph;
    UsageGraph *writeGraph;
    QPushButton *backgroundColor_btn;
    QPushButton *textColor_btn;
    QPushButton *applyAllPages_btn;
};

class ProcessWidget : public QWidget
{
public:
    ProcessWidget(QWidget *parent = nullptr)
        : QWidget(parent)
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
            "QHeaderView::section { background-color: #3d3d3d; color: white; padding: 5px; }");

        processTable->setMaximumWidth(820);
        layout->addWidget(processTable);

        // Change background color button
        backgroundColor_btn = new QPushButton("Change Background Color");
        backgroundColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(backgroundColor_btn);

        // Change text color button
        textColor_btn = new QPushButton("Change Content Color");
        textColor_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(textColor_btn);

        // Apply page styling to all pages button
        applyAllPages_btn = new QPushButton("Apply Styling To All Pages");
        applyAllPages_btn->setStyleSheet("QPushButton { color: white; font-size: 15px; max-width: 250px; border: 1px solid white; border-radius: 2px}");
        layout->addWidget(applyAllPages_btn);

        // Connects background color button to color dialog box (color wheel)
        connect(backgroundColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setBackgroundColorDialog(this);
        });

        // Connects background color button to color dialog box (color wheel)
        connect(textColor_btn, &QPushButton::clicked, this, [this]() {
            ColorUtils::setTextColorDialog(this);
        });

        connect(applyAllPages_btn, &QPushButton::clicked, this, [this]() {
            QStackedWidget* stack = qobject_cast<QStackedWidget*>(parentWidget());
            if (stack) {
                QList<QWidget*> pages;
                for (int i = 0; i < stack->count(); ++i) {
                    pages.append(stack->widget(i));
                }
                ColorUtils::setAllStyles(this, pages);
            }
        });

        processMonitor = new ProcessInfo(this);
        connect(processMonitor,
                &ProcessInfo::processesUpdated,
                this,
                &ProcessWidget::updateProcesses);

        setStyleSheet("QWidget { background-color: #1e1e1e;}");
    }
private slots:
    void updateProcesses(std::vector<ProcessUsage> processes)
    {
        processTable->setRowCount(processes.size());
        for (size_t i = 0; i < processes.size(); ++i) {
            const ProcessUsage &proc = processes[i];
            processTable->setItem(i, 0, new QTableWidgetItem(QString::number(proc.PID)));
            processTable->setItem(i, 1, new QTableWidgetItem(proc.name));
            processTable->setItem(i,
                                  2,
                                  new QTableWidgetItem(QString::number(proc.cpuUsage, 'f', 2)));
            processTable->setItem(i,
                                  3,
                                  new QTableWidgetItem(QString::number(proc.ramUsage, 'f', 2)
                                                       + " MB"));
            processTable->setItem(i,
                                  4,
                                  new QTableWidgetItem(
                                      "Bytes Read: " + QString("%1").arg(proc.bytesRead)
                                      + " Bytes Written: " + QString("%1").arg(proc.bytesWritten)));
        }
    }

private:
    QTableWidget *processTable;
    ProcessInfo *processMonitor;
    QPushButton *backgroundColor_btn;
    QPushButton *textColor_btn;
    QPushButton *applyAllPages_btn;
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
