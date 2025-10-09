#ifndef CPUMONITORUSAGE_H
#define CPUMONITORUSAGE_H

#pragma once
#include <QObject>
#include <QString>
#include <QTimer>

class CpuMonitorUsage : public QObject
{
    Q_OBJECT

public:
    explicit CpuMonitorUsage(QObject *parent = nullptr);
    ~CpuMonitorUsage() = default;

    double getCurrentUsage() const { return m_currentUsage; }
    QString getUsageString() const;

signals:
    void usageUpdated(double usage);

private slots:
    void updateCpuUsage();

private:
    QTimer *m_timer;
    double m_currentUsage;
    quint64 m_lastTotal;
    quint64 m_lastIdle;
    bool m_firstRun;
};

#endif // CPUMONITORUSAGE_H
