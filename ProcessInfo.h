#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <vector>
#include <QDir>

struct ProcessUsage
{
    int PID;
    QString name;
    double cpuUsage;
    double ramUsage;
    long bytesRead;
    long bytesWritten;

};
class ProcessInfo: public QObject
{
    Q_OBJECT
public:
    explicit ProcessInfo(QObject *parent = nullptr);
    ~ProcessInfo() = default;
    QString getProcessName(int pid);
    double getCPUUsage(int pid);
    double getRAMUsage(int pid);
    std::pair<long, long> getDiskInfo(int pid);



signals:
    void cpuUsageUpdated(double cpuUsage);
    //void ramUsageUpdated(double ramUsage);
    //void diskInfoUpdated(long bytesRead, long bytesWritten);
    void processesUpdated(std::vector<ProcessUsage> processes);

private slots:
    void updateProcessInfo();

private:
    QTimer *m_timer;
    struct ProcessCPUData
    {
        double utime;
        double stime;
        double uptime;

    };
    QMap <int, ProcessCPUData> previousCPUData;
    std::vector <QDir> getProcesses();
    bool containsLetters(const QString &word);
    //double cpuUsage;
    //double ramUsage;
    //long bytesRead;
    //long bytesWritten;


};


#endif // PROCESSINFO_H
