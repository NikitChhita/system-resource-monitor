#ifndef RAMUSAGE_H
#define RAMUSAGE_H


#include <QObject>
#include <QString>
#include <QTimer>

class RamUsage : public QObject
{
    Q_OBJECT

public:
    explicit RamUsage(QObject *parent = nullptr);
    ~RamUsage() = default;

    QString getRamUsageString() const;
    long getCurrentRamUsage() const {return totalUsedRam;}
    long getTotalSysRam() const {return totalSysRam;}


signals:
    void ramUsageUpdated(long usedRamKB);

private slots:
    void updateRamUsage();

private:
    QTimer *m_Timer;

    long totalUsedRam;
    long totalSysRam;
    long buffers;
    long cachedRam;
    long availableRam;
    long pagedPool;
    long nonPagedPool;
    bool m_firstRun;


};
#endif // RAMUSAGE_H
