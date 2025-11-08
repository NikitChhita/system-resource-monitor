#include "DiskInfo.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <sys/sysinfo.h>
#include<sys/statvfs.h>

DiskInfo::DiskInfo(QObject *parent)
    : QObject(parent)
    , readIOPS(0)
    , writeIOPS(0)
    , readThroughput(0.0)
    , writtenThroughput(0.0)
    , prevSectorsRead(0)
    , prevSectorsWritten(0)
    , prevTime(0)
    , totalDiskSpace(0.0)
    , availDiskSpace(0.0)
    , firstRun(true)

{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DiskInfo::updateDiskInfo);
    m_timer->start(1000);
    updateDiskInfo();
    getDiskSpaceInfo();

}

void DiskInfo::getDiskSpaceInfo()
{
    struct statvfs buf;
    const char *path = "/";
    if(statvfs(path, &buf) == 0)
    {
        totalDiskSpace = (buf.f_blocks * buf.f_bsize) / (1024.0 * 1024.0 * 1024.0);
        availDiskSpace = (buf.f_bavail * buf.f_bsize) / (1024.0 * 1024.0 * 1024.0);

    }


}

void DiskInfo::updateDiskInfo()
{
    QFile file("/proc/diskstats");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open /proc/diskstats";
        return;
    }

    QString allContent = file.readAll();
    file.close();

    QStringList lines = allContent.split('\n');
    QRegularExpression whitespaceRegex("\\s+");

    long currentSectorsRead = 0;
    long currentSectorsWritten = 0;

    for (int i = 0; i < lines.size() - 1; i++) {
        QString line = lines[i];
        QStringList values = line.split(whitespaceRegex, Qt::SkipEmptyParts);
        QChar lastChar = values[2].at(values[2].length()-1);
        //last line was if(values[2] == "vda")
        if (!(lastChar.isDigit())) {
            readIOPS = values[3].toLong();
            writeIOPS = values[7].toLong();
            currentSectorsRead = values[5].toLong();
            currentSectorsWritten = values[9].toLong();
            break;
        }
    }

    long currentTime = QDateTime::currentMSecsSinceEpoch();

    if (firstRun) {
        prevSectorsRead = currentSectorsRead;
        prevSectorsWritten = currentSectorsWritten;
        prevTime = currentTime;
        firstRun = false;

        //emit updateReads(readIOPS);
        //emit updateWrites(writeIOPS);
        return;
    }

    double elapsedTime = (currentTime - prevTime) / 1000.0;

    if (elapsedTime > 0) {
        long readSectorsDelta = currentSectorsRead - prevSectorsRead;
        long writtenSectorsDelta = currentSectorsWritten - prevSectorsWritten;

        readThroughput = (readSectorsDelta * 512) / elapsedTime;
        writtenThroughput = (writtenSectorsDelta * 512) / elapsedTime;

        prevSectorsRead = currentSectorsRead;
        prevSectorsWritten = currentSectorsWritten;
        prevTime = currentTime;

        emit updateReads(readIOPS);
        emit updateWrites(writeIOPS);
        emit updateReadThroughput(readThroughput);
        emit updateWriteThroughput(writtenThroughput);
    }
}



QString DiskInfo::getDiskInfoString()
{
    double readMBps = readThroughput / (1024.0 * 1024.0);
    double writeMBps = writtenThroughput / (1024.0 * 1024.0);
    return QString("Reads Completed: %0 Writes Completed: %1\n"
                   "Read Throughput: %2 MB/s Write Throughput: %3 MB/s \n"
                   "Available Disk Space: %4 GB\n"
                   "Total Disk Space: %5 GB")
        .arg(readIOPS)
        .arg(writeIOPS)
        .arg(readMBps, 0, 'f', 2)
        .arg(writeMBps, 0, 'f', 2)
        .arg(availDiskSpace, 0, 'f', 2)
        .arg(totalDiskSpace,0, 'f', 2 );
}
