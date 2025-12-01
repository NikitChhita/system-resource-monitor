#ifndef CPUMONITORUSAGE_H
#define CPUMONITORUSAGE_H
#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>

// Simplified struct with only working metrics
struct CpuInfo
{
    double usage;          // Current CPU usage percentage
    QString modelName;     // CPU model name
    int sockets;           // Number of CPU sockets
    int coresPerSocket;    // Physical cores per socket
    int logicalProcessors; // Total logical processors (threads)
    int processes;         // Number of running processes
    int threads;           // Number of running threads
    QString uptime;        // System uptime formatted
};

class CpuMonitorUsage : public QObject
{
    Q_OBJECT
public:
    explicit CpuMonitorUsage(QObject *parent = nullptr);
    ~CpuMonitorUsage() = default;

    double getCurrentUsage() const { return m_currentUsage; }
    QString getUsageString() const;
    CpuInfo getCpuInfo() const { return m_cpuInfo; }
    QVector<double> getUtilizationHistory() const { return m_utilizationHistory; }

signals:
    void usageUpdated(double usage);
    void cpuInfoUpdated(const CpuInfo &info);

private slots:
    void updateCpuUsage();

private:
    QTimer *m_timer;
    double m_currentUsage;
    quint64 m_lastTotal;
    quint64 m_lastIdle;
    bool m_firstRun;

    CpuInfo m_cpuInfo;
    QVector<double> m_utilizationHistory;

    // Helper methods - only what works
    void updateDetailedInfo();
    void updateLscpuInfo();
    int getProcessCount();
    int getThreadCount();
    QString formatUptime();
};

#endif // CPUMONITORUSAGE_H
