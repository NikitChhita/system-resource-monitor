#ifndef PROCESSUSAGE_H
#define PROCESSUSAGE_H

#endif // PROCESSUSAGE_H
#include <QObject>
#include <QString>
#include <QTimer>
#include <QList>
#include <QFileInfoList>
#include <QDir>

class ProcessUsage : public QObject
{
   Q_OBJECT

public:
    ProcessUsage(QObject *parent = nullptr);
    ~ProcessUsage();
    struct ProcessInfo
    {
        QTimer *timer;
        int processID;
        qint64 utime; //time process has been running in user mode
        qint64 stime; // time process has been running in kernel mode
        qint64 starttime; //the moment the os created and launched the process

        qint64 prevUtime;
        qint64 prevStime;
        qint64 prevTotalTime;

        qint64 sysRam;
        qint64 vmRSS;

        double cpuUsage;
        double ramUsage;
    };

}

#endif
