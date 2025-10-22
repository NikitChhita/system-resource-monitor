#include "ProcessInfo.h"
#include <QFile>
#include <QDir>
#include <sys/sysinfo.h>
#include <QRegularExpression>
#include <QDebug>
#include <vector>
#include <QDirIterator>


ProcessInfo::ProcessInfo(QObject *parent)
    :QObject(parent)
    //,cpuUsage(0.0)
    //,ramUsage(0.0)
    //,bytesRead(0)
    //,bytesWritten(0)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this,&ProcessInfo::updateProcessInfo);
    m_timer->start(1000);
    updateProcessInfo();
}

bool ProcessInfo::containsLetters(QString word)
{
    for(QChar c : word)
    {
        if(c.isLetter())
        {
            return true;
        }
    }
    return false;
}

std::vector<QDir> ProcessInfo::getProcesses()
{
    std::vector<QDir> processes;
    QDirIterator it("/proc", QDir::Dirs | QDir::NoDotAndDotDot);
    while(it.hasNext())
    {
        QString dir = it.next();
        if(containsLetters(dir))
        {
            continue;
        }
        processes.push_back(it.filePath());
    }
    return processes;
}

QString ProcessInfo::getProcessName(int pid)
{
    QString commPath = QString("/proc/%1/comm").arg(pid);
    QFile file(commPath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString name = file.readLine().trimmed();
        file.close();
        return name;
    }

    return "Unknown";
}

// for now iterate through the vector, collect pid, assign it to struct val, everything else
// to 0, work to display on screen. If working, rinse and repeat


void ProcessInfo::updateProcessInfo()
{
    std::vector<ProcessUsage> processes;
    std::vector<QDir> processDirs = getProcesses();
    for(const QDir &dir : processDirs)
    {
        QString dirName = dir.dirName();
        bool ok;
        int pid = dirName.toInt(&ok);

        if(ok)
        {
            ProcessUsage proc;
            proc.PID = pid;
            proc.name = getProcessName(pid);
            proc.cpuUsage = 0.0;
            proc.ramUsage = 0.0;
            proc.bytesRead = 0;
            proc.bytesWritten = 0.0;

            processes.push_back(proc);

            qDebug() << "PID" << proc.PID << "Name:" << proc.name;
        }
    }

    emit processesUpdated(processes);
}




