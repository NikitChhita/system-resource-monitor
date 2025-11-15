#include "RamUsage.h"
#include <QFile>
#include <QTextStream>
#include <sys/sysinfo.h>
#include <QRegularExpression>
#include <QDebug>

RamUsage::RamUsage(QObject *parent)
    :QObject(parent)
    , totalUsedRam(0)
    , totalSysRam(0)
    //, freeRAM(0)
    , buffers(0)
    , cachedRam(0)
    , availableRam(0)
{
    m_Timer = new QTimer(this);
    connect(m_Timer, &QTimer::timeout, this, &RamUsage::updateRamUsage);
    m_Timer -> start(1000);

    updateRamUsage();
}

void RamUsage::updateRamUsage()
{
    QFile file("/proc/meminfo");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open /proc/meminfo";
        return ;
    }

    QString allContent = file.readAll();
    file.close();

    QStringList lines = allContent.split('\n', Qt::SkipEmptyParts);
    QRegularExpression whitespaceRegex("\\s+");

    for(int i = 0; i < lines.size() && i < 10; i++)
    {
        QString line = lines[i];
        QStringList values = line.split(whitespaceRegex, Qt::SkipEmptyParts);

        if(values.size() < 2) continue;

        bool ok;
        long value = values[1].toLong(&ok);

        if(!ok)
        {
            qDebug() << "Error parsing: " << values[0];
        }

        if (values[0] == "MemTotal:")
        {
            totalSysRam = value;

        }
        //else if (values[0] == "MemFree:")
        //{
        //    freeRAM = value;
        //}
        else if(values[0] == "MemAvailable:")
        {
            availableRam = value;
        }
        else if (values[0] == "Buffers:")
        {
            buffers = value;
        }
        else if(values[0] == "Cached:")
        {
            cachedRam = value;

        }
    }

    if (totalSysRam > 0)
    {
        totalUsedRam = totalSysRam - availableRam;
        emit ramUsageUpdated(totalUsedRam);
    }
    else
    {
        qDebug() << "Error: Invalid totalSysRam value";
    }

}

QString RamUsage::getRamUsageString() const
{
    double usedGB = totalUsedRam / (1024.0 * 1024.0);
    double totalGB = totalSysRam / (1024.0 * 1024.0);
    double usedPercentage = (totalGB > 0.0) ? (usedGB * 100.0) / totalGB : 0.0;
    double availableGB = (availableRam) / (1024.0 * 1024.0);
    double cachedGB = cachedRam / (1024.0 * 1024.0);


    return QString("Memory Used: %1 GB/ %2 GB (%3%)\n"
                   "Total Memory: %4 GB Available Memory: %5 GB\n"
                   "Cached Memory: %6 GB")
        .arg(usedGB, 0, 'f', 2).arg(totalGB, 0,  'f', 2)
        .arg(usedPercentage, 0, 'f', 2).arg(totalGB, 0, 'f', 2)
        .arg(availableGB, 0, 'f', 2).arg(cachedGB, 0, 'f', 2);

}




