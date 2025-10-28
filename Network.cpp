#include "Network.h"
#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTextStream>

networkStats::networkStats(QObject *parent)
    : QObject(parent)
    , current_rxBytes(0) //current CPU usage to 0.0 to start
    , current_txBytes(0) //previous total CPU time (0 for now)
    , m_firstRun(true)   //flag to skip first the reading
{
    m_timer = new QTimer(this); //timer object
    //everytime timer fires, updateNetStats and getIfaceData retrieve and emit their data.
    QString iface = "lo"; // defaults to loopback interface

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        // Skips loopback interfaces
        if (!interface.humanReadableName().contains("lo", Qt::CaseInsensitive)) {
            iface = interface.humanReadableName();
            break;
        }
    }

    connect(m_timer, &QTimer::timeout, this, [this, iface]() {
        updateNetStats(iface);
        getIfaceData(iface);
    });

    //timer fires every 1 second
    m_timer->start(1000);

    // calls update function immediately to create baseline data to compare to current data
    updateNetStats(iface);
}

void networkStats::getIfaceData(QString interface)
{
    //Just returns the interface that was provided, as thats its name
    ifaceName = interface;

    //Determines the connection type based on the characters in the interface (unique to different connection types)
    if (interface.contains("enp")) {
        ifaceType = "Ethernet (PCIE)";
    } else if (interface.contains("ens")) {
        ifaceType = "Ethernet (Hot-plug)";
    } else if (interface.contains("eno")) {
        ifaceType = "Ethernet (Onboard)";
    } else if (interface.contains("wl")) {
        ifaceType = "Wireless";
    } else if (interface.contains("ww")) {
        ifaceType = "Cellular";
    } else if (interface.contains("en")) {
        ifaceType = "Ethernet";
    }

    //Obtains IPv6 and IPv4 info from current interface
    //Loops through all interfaces using QNetworkInterface and stopping when it finds the current one
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        if (iface.name() == interface) {
            //Loops through all addresses for the found interface
            for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
                //QHostAddress converts the data into a more readable format
                QHostAddress addr = entry.ip();
                if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                    ipv4Addr = addr.toString();
                } else if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
                    //Removes the %[interface name here] from the end of the ipv6 address
                    QString ipv6Str = addr.toString();

                    int percentIndex = ipv6Str.indexOf('%');
                    if (percentIndex != -1) {
                        ipv6Str = ipv6Str.left(percentIndex);
                    }

                    ipv6Addr = ipv6Str;
                }
            }
        }
    }

    //Signals for the page to update the interface data
    emit updateIfaceData(ifaceName, ifaceType, ipv6Addr, ipv4Addr);
}

void networkStats::updateNetStats(QString interface)
{
    //Obtains and reads in the receive and send files
    QFile rxFile(QString("/sys/class/net/%1/statistics/rx_bytes").arg(interface));
    QFile txFile(QString("/sys/class/net/%1/statistics/tx_bytes").arg(interface));

    //Makes sure they are able to be opened
    if (rxFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&rxFile);
        in >> current_rxBytes;
    }
    if (txFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&txFile);
        in >> current_txBytes;
    }

    // Defaults to byte size in kilobytes
    QString rxSize = "KB";
    QString txSize = "KB";

    //Skips first run to get a baseline for the next run
    if (!m_firstRun) {
        //Download speed
        double rxSpeed = (current_rxBytes - last_rxBytes) * 1000.0 / 1000/ 1000; // changes from bytes/elapsed time to bytes/sec, then to kb/sec
        if (rxSpeed >= 1000 && rxSpeed < 1000000) {
            rxSpeed = rxSpeed / 1000;
            rxSize = "MB";
        } else if (rxSpeed >= 1000000) {
            rxSpeed = rxSpeed / 1000000;
            rxSize = "GB";
        }

        //Upload speed
        double txSpeed = (current_txBytes - last_txBytes) * 1000.0 / 1000 / 1000;
        if (txSpeed >= 1000 && txSpeed < 1000000) {
            txSpeed = txSpeed / 1000;
            txSize = "MB";
        } else if (txSpeed >= 1000000) {
            txSpeed = txSpeed / 1000000;
            txSize = "GB";
        }

        //Emits signal with receive and send speeds for throughput (QStrings)
        emit updatedThroughput(rxSpeed, rxSize, txSpeed, txSize);
    }

    //Updates existing throughput info with current info
    last_txBytes = current_txBytes;
    last_rxBytes = current_rxBytes;
    m_firstRun = false;
}
