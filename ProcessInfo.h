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
    //double getCPUUsage() const{return cpuUsage;}
    //double getRAMUsage() const{return ramUsage;}
   // long getBytesRead() const {return bytesRead;}
   // long getBytesWritten() const{return bytesRead;}



signals:
    //void cpuUsageUpdated(double cpuUsage);
    //void ramUsageUpdated(double ramUsage);
    //void diskInfoUpdated(long bytesRead, long bytesWritten);
    void processesUpdated(std::vector<ProcessInfo> processes);

private slots:
    void updateProcessInfo();

private:
    QTimer *m_timer;
    //double cpuUsage;
    //double ramUsage;
    //long bytesRead;
    //long bytesWritten;

    std::vector <QDir> getProcesses();
    bool containsLetters(QString word);


};


#endif // PROCESSINFO_H
