#ifndef DISKINFO_H
#define DISKINFO_H

#include <QObject>
#include <QString>
#include <QTimer>

class DiskInfo : public QObject
{
    Q_OBJECT

public:
    explicit DiskInfo(QObject *parent = nullptr);
    ~DiskInfo() = default;

    QString getDiskInfoString() const;

    //long getReadIOPS() const {return readIOPS;}
    //long getWriteIOPS() const {return writeIOPS;} // indicate writes and reads completed

    //throughput = amount of data moved per second
    //double getReadThroughput() const {return readThroughput;}
    //double getWriteThroughput() const {return writtenThroughput;}

signals:
    void updateReads(long readIOPS);
    void updateWrites(long writeIOPS);
    void updateReadThroughput(double readThroughput);
    void updateWriteThroughput(double writeThroughput);

private slots:
    void updateDiskInfo();

private:
    QTimer *m_timer;

    long readIOPS;
    long writeIOPS;

    double readThroughput;
    double writtenThroughput;

    long prevSectorsRead;
    long prevSectorsWritten;

    long prevTime;
    bool firstRun;


};

#endif // DISKINFO_H
