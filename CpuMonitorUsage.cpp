#include "CpuMonitorUsage.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <sstream>

CpuMonitorUsage::CpuMonitorUsage(QObject *parent)
    : QObject(parent)
    , m_currentUsage(0.0)
    , m_lastTotal(0)
    , m_lastIdle(0)
    , m_firstRun(true)
{
    // Initialize history with 60 zeros for smooth graph start
    m_utilizationHistory.fill(0, 60);

    // Initialize CPU info struct - only working fields
    m_cpuInfo.usage = 0.0;
    m_cpuInfo.sockets = 0;
    m_cpuInfo.coresPerSocket = 0;
    m_cpuInfo.logicalProcessors = 0;
    m_cpuInfo.processes = 0;
    m_cpuInfo.threads = 0;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CpuMonitorUsage::updateCpuUsage);
    m_timer->start(1000);

    // Get initial detailed info immediately
    updateDetailedInfo();
    emit cpuInfoUpdated(m_cpuInfo);

    // Start CPU usage updates
    QTimer::singleShot(100, this, &CpuMonitorUsage::updateCpuUsage);
}

QString CpuMonitorUsage::getUsageString() const
{
    return QString("CPU Usage: %1%").arg(QString::number(m_currentUsage, 'f', 1));
}

void CpuMonitorUsage::updateCpuUsage()
{
    static const QRegularExpression whitespaceRegex("\\s+");

    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream in(&file);
    QStringList values = in.readLine().split(whitespaceRegex, Qt::SkipEmptyParts);
    file.close();

    if (values.size() < 5) {
        return;
    }

    const quint64 user = values[1].toULongLong();
    const quint64 nice = values[2].toULongLong();
    const quint64 system = values[3].toULongLong();
    const quint64 idle = values[4].toULongLong();
    const quint64 total = user + nice + system + idle;

    if (!m_firstRun) {
        const quint64 diffIdle = idle - m_lastIdle;
        const quint64 diffTotal = total - m_lastTotal;

        if (diffTotal > 0) {
            m_currentUsage = 100.0 * static_cast<double>(diffTotal - diffIdle)
            / static_cast<double>(diffTotal);
        }

        // Update CPU info struct
        m_cpuInfo.usage = m_currentUsage;

        // Update utilization history (keep last 60 values)
        if (m_utilizationHistory.size() >= 60) {
            m_utilizationHistory.removeFirst();
        }
        m_utilizationHistory.append(m_currentUsage);

        emit usageUpdated(m_currentUsage);
    } else {
        m_cpuInfo.usage = 0.0;
    }

    // Update detailed CPU information every second
    updateDetailedInfo();

    // Emit detailed info signal
    emit cpuInfoUpdated(m_cpuInfo);

    m_lastTotal = total;
    m_lastIdle = idle;
    m_firstRun = false;
}

void CpuMonitorUsage::updateDetailedInfo()
{
    // Update only working metrics
    updateLscpuInfo();
    m_cpuInfo.processes = getProcessCount();
    m_cpuInfo.threads = getThreadCount();
    m_cpuInfo.uptime = formatUptime();
}

void CpuMonitorUsage::updateLscpuInfo()
{
    // Run lscpu command
    QProcess lscpu;
    lscpu.start("lscpu");
    lscpu.waitForFinished();
    QString output = lscpu.readAll();

    static const QRegularExpression whitespaceRegex("\\s+");
    std::istringstream iss(output.toStdString());

    int cores = 0;
    int threadsPerCore = 0;

    for (std::string line; std::getline(iss, line); )
    {
        QString qLine = QString::fromStdString(line);

        // Get CPU model name
        if (qLine.startsWith("model name:", Qt::CaseInsensitive))
        {
            m_cpuInfo.modelName = qLine.mid(qLine.indexOf(':') + 1).trimmed();
        }
        // Get sockets
        else if (qLine.startsWith("socket", Qt::CaseInsensitive))
        {
            QStringList parts = qLine.split(whitespaceRegex, Qt::SkipEmptyParts);
            if (parts.size() > 1) {
                m_cpuInfo.sockets = parts[1].toInt();
            }
        }
        // Get cores per socket
        else if (qLine.startsWith("core(s) per socket", Qt::CaseInsensitive))
        {
            QStringList parts = qLine.split(whitespaceRegex, Qt::SkipEmptyParts);
            if (parts.size() > 0) {
                cores = parts.last().toInt();
                m_cpuInfo.coresPerSocket = cores;
            }
        }
        // Get threads per core to calculate logical processors
        else if (qLine.startsWith("thread(s) per core", Qt::CaseInsensitive))
        {
            QStringList parts = qLine.split(whitespaceRegex, Qt::SkipEmptyParts);
            if (parts.size() > 0) {
                threadsPerCore = parts.last().toInt();
                if (cores > 0) {
                    m_cpuInfo.logicalProcessors = cores * threadsPerCore;
                }
            }
        }
    }
}

int CpuMonitorUsage::getProcessCount()
{
    // Count directories in /proc that are numeric (PIDs)
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    int count = 0;
    for (const QString &entry : entries) {
        bool ok;
        entry.toInt(&ok);
        if (ok) {
            count++;
        }
    }

    return count;
}

int CpuMonitorUsage::getThreadCount()
{
    // Count all threads by checking /proc/[pid]/task directories
    QDir procDir("/proc");
    QStringList processes = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    int totalThreads = 0;
    for (const QString &entry : processes) {
        bool ok;
        entry.toInt(&ok);
        if (ok) {
            QString taskPath = "/proc/" + entry + "/task";
            QDir taskDir(taskPath);
            if (taskDir.exists()) {
                QStringList taskEntries = taskDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                totalThreads += taskEntries.count();
            }
        }
    }

    return totalThreads;
}

QString CpuMonitorUsage::formatUptime()
{
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly)) {
        return "N/A";
    }

    QTextStream in(&file);
    double seconds;
    in >> seconds;
    file.close();

    int h = static_cast<int>(seconds) / 3600;
    int m = static_cast<int>(seconds) % 3600 / 60;
    int s = static_cast<int>(seconds) % 60;

    return QString("%1:%2:%3")
        .arg(h, 2, 10, QLatin1Char('0'))
        .arg(m, 2, 10, QLatin1Char('0'))
        .arg(s, 2, 10, QLatin1Char('0'));
}
