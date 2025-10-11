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

    long getCurrentRamUsage() const {return totalUsedRam;}
    QString getRamUsageString() const;

signals:
    void ramUsageUpdated(double usage);

private slots:
    void updateRamUsage();

private:
    QTimer *m_Timer;

    long totalUsedRam; //  = totalSysRam - freeRam - buffers - cachedRam
    long totalSysRam;
    long freeRAM;

    long buffers;
    long cachedRam;
    bool m_firstRun;



};
#endif // RAMUSAGE_H
