#include "ProcessInfo.h"
#include <QFile>
#include <QDir>
#include <sys/sysinfo.h>
#include <QRegularExpression>
#include <QDebug>
#include <vector>
#include <QDirIterator>
#include <unistd.h>


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

bool ProcessInfo::containsLetters(const QString &word)
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
        QString dirPath = it.next();
        QString dirName = QFileInfo(dirPath).fileName();

        if(containsLetters(dirName))
        {
            continue;
        }

        //checking if process is kernel process or zombie
        QDir procDir(dirPath);
        QFile cmdFile(procDir.filePath("cmdline"));

        if(!cmdFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            continue;
        }

        QString cmd = cmdFile.readAll().trimmed();
        if(cmd.isEmpty())
        {
            continue;
        }

        processes.push_back(QDir(it.filePath()));
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

double ProcessInfo::getCPUUsage(int pid)
{

    const double SYSTEM_CLOCK_TICKS = sysconf(_SC_CLK_TCK);
    const int SYSTEM_PROCESSORS =  sysconf(_SC_NPROCESSORS_ONLN);

    static const QRegularExpression whitespaceRegex("\\s+");

    QString uptimePath = QString("/proc/uptime");
    QFile uptimeFile(uptimePath);


    if(!uptimeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 0.0;
    }

    QTextStream in(&uptimeFile);
    QStringList values = in.readLine().split(whitespaceRegex, Qt::SkipEmptyParts);
    uptimeFile.close();
    //usage = sum of utime and stime / elapsed time
    //calculating elapsed time
    double currentUptime = values[0].toDouble();

    //getting from /proc/<PID>/stat
    QString statPath = QString("/proc/%1/stat").arg(pid);
    QFile statFile(statPath);

    if(!statFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 0.0;
    }

    QTextStream in1(&statFile);
    QStringList cpuValues = in1.readLine().split(whitespaceRegex, Qt::SkipEmptyParts);
    statFile.close();

    double utime = cpuValues[13].toLong() / SYSTEM_CLOCK_TICKS;
    double stime = cpuValues[14].toLong() / SYSTEM_CLOCK_TICKS;

    if(!previousCPUData.contains(pid))
    {
        ProcessCPUData data;
        data.utime = utime;
        data.stime = stime;
        data.uptime = currentUptime;
        previousCPUData[pid] = data;
        return 0.0;
    }
    ProcessCPUData prevData = previousCPUData[pid];
    double timeDelta = currentUptime - prevData.uptime;
    double cpuTimeDelta = (stime + utime) - (prevData.utime + prevData.stime);
    double cpuUsage = (cpuTimeDelta / (timeDelta * SYSTEM_PROCESSORS))* 100;

    ProcessCPUData newData;
    newData.utime = utime;
    newData.stime = stime;
    newData.uptime = currentUptime;
    previousCPUData[pid] = newData;

    return cpuUsage;

}

double ProcessInfo::getRAMUsage(int pid)
{
    QString statusPath = QString("/proc/%1/status").arg(pid);
    QFile statusFile(statusPath);
    static const QRegularExpression whitespaceRegex("\\s+");
    if(!statusFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 0.0;
    }

    QTextStream in(&statusFile);
    QString allContent = in.readAll();
    statusFile.close();


    QStringList lines = allContent.split("\n");

    for(const QString &line : lines)
    {
        if(line.startsWith("VmRSS:"))
        {

            QStringList parts = line.split(whitespaceRegex, Qt::SkipEmptyParts);
            if(parts.size() >= 2)
            {
                double vmRSS = parts[1].toDouble() / (1024);
                return vmRSS;
            }
        }
    }

    return 0.0;
}


std::pair<long,long> ProcessInfo::getDiskInfo(int pid)
{
    long bytesRead;
    long bytesWritten;

    QString ioPath = QString(("/proc/%1/io")).arg(pid);
    QFile ioFile(ioPath);
    static const QRegularExpression whitespaceRegex("\\s+");

    if(!ioFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return std::make_pair(0, 0);
    }

    QTextStream in(&ioFile);
    QString allContent = in.readAll();
    ioFile.close();

    QStringList lines = allContent.split("\n");
    for(const QString &line: lines)
    {
        if(line.startsWith("read_bytes:"))
        {
            QStringList parts = line.split(whitespaceRegex, Qt::SkipEmptyParts);
            if(parts.size() >= 2)
            {
                bytesRead = parts[1].toLong();
            }
        }

        if(line.startsWith("write_bytes:"))
        {
            QStringList parts = line.split(whitespaceRegex, Qt::SkipEmptyParts);
            if(parts.size() >= 2)
            {
                bytesWritten = parts[1].toLong();
            }
        }
    }
    return std::make_pair(bytesRead, bytesWritten);
}

// for now iterate through the vector, collect pid, assign it to struct val, everything else
// to 0, work to display on screen. If working, rinse and repeat


void ProcessInfo::updateProcessInfo()
{
    std::vector<ProcessUsage> processes;
    std::vector<QDir> processDirs = getProcesses();

    qDebug() << "Found" << processDirs.size() << "process directories";
    for(const QDir &dir : processDirs)
    {
        QString dirName = dir.dirName();
        //bool ok;
        int pid = dirName.toInt();

        //if(ok)
        //{
        ProcessUsage proc;
        proc.PID = pid;
        proc.name = getProcessName(pid);
        proc.cpuUsage = getCPUUsage(pid);
        proc.ramUsage = getRAMUsage(pid);
        std::pair<long, long> diskInfo = getDiskInfo(pid);
        proc.bytesRead = diskInfo.first;
        proc.bytesWritten = diskInfo.second;
        processes.push_back(proc);
        //}
    }

    qDebug() << "Emitting" << processes.size() << "processes";
    emit processesUpdated(processes);
}




