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
    , freeRAM(0)
    , buffers(0)
    , cachedRam(0)
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
        return;
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

        if (values[0] == "MemTotal:")
        {
            totalSysRam = values[1].toLong();

        }
        else if (values[0] == "MemFree:")
        {
            freeRAM = values[1].toLong();
        }
        else if (values[0] == "Buffers:")
        {
            buffers = values[1].toLong();
        }
        else if(values[0] == "Cached:")
        {
            cachedRam = values[1].toLong();
            break;
        }
    }


    totalUsedRam = totalSysRam - freeRAM - buffers - cachedRam;
    emit ramUsageUpdated(totalUsedRam);

}

QString RamUsage::getRamUsageString() const
{
    double usedGB = totalUsedRam / (1024.0 * 1024.0);
    double totalGB = totalSysRam / (1024.0 * 1024.0);
    return QString("Memory Used: %1 GB/ %2 GB").arg(usedGB, 0, 'f', 1).arg(totalGB, 0,  'f', 1);

}
