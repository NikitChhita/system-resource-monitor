#include "CpuMonitorUsage.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

//using QFile for linux /proc/stat //QObject for Qt's signal system
//QRegularExpression used to split whitespace in /proc/stat lines
CpuMonitorUsage::CpuMonitorUsage(QObject *parent)
    : QObject(parent)
    , m_currentUsage(0.0) //current CPU usage to 0.0 to start
    , m_lastTotal(0)      //previous total CPU time (0 for now)
    , m_lastIdle(0)       //previous idle CPU time (0 aswell)
    , m_firstRun(true)    //flag to skip first the reading
{
    m_timer = new QTimer(this); //timer object
    //everytime timer fires, updateCpuUsage gets called to update our usage
    connect(m_timer, &QTimer::timeout, this, &CpuMonitorUsage::updateCpuUsage);

    //timer fires every 1 second
    m_timer->start(1000);

    // we call the update function right away, but skip first reading
    updateCpuUsage();
}

QString CpuMonitorUsage::getUsageString() const
{
    //placeholder with %1 and its changed to the actual amount
    return QString("CPU Usage: %1%").arg(QString::number(m_currentUsage, 'f', 1));
}
//method thats called every second to update usage
void CpuMonitorUsage::updateCpuUsage()
{
    static const QRegularExpression whitespaceRegex("\\s+");

    //creates a file object that points to the Linux system file
    //containg our cpu stats
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly)) { //make sure we can read it
        return;
    }
    //creates a wrapper around the file for easiesr text reading
    QTextStream in(&file);
    QStringList values = in.readLine().split(whitespaceRegex, Qt::SkipEmptyParts);
    file.close(); //good practice to close file
    //values reads first line from /proc/stat and splits the line on whitespaces

    //need atleast 5 elements => "cpu" + user + nice + system + idle to calculate
    if (values.size() < 5) {
        return;
    }

    //in the /proc/stat file [1] is user [2] nice...
    //quint64 handles large numbers
    const quint64 user = values[1].toULongLong();
    const quint64 nice = values[2].toULongLong();
    //Extracts nice (low-priority user-mode) CPU time from position 2
    const quint64 system = values[3].toULongLong();
    //extracts kernel mode cpu time
    const quint64 idle = values[4].toULongLong();
    //extracts idle cpu time
    const quint64 total = user + nice + system + idle;
    //calculate total cpu time by summing

    //this is where we skip the first run because we need previous readings
    if (!m_firstRun) {
        //calculate the idle time since last reading
        const quint64 diffIdle = idle - m_lastIdle;
        //calculate how much total cpu time occured since last reading
        const quint64 diffTotal = total - m_lastTotal;

        if (diffTotal > 0) { //dont want to divide by 0
            m_currentUsage = 100.0 * static_cast<double>(diffTotal - diffIdle)
                             / static_cast<double>(diffTotal);
        } //diffTotal - diddIdle is active CPU time during interval
        //divided by diffTotal is the percent of time CPU was active
        //multiply 100 to convert to a percentage
        //static_cast<double>() is used for to ensure floating point division

        //emits a signal with our CPU usage val, all connected slots are called
        emit usageUpdated(m_currentUsage);
    }

    //for each run we need to store new values and these will update every second
    m_lastTotal = total;
    m_lastIdle = idle;
    m_firstRun = false;
}
