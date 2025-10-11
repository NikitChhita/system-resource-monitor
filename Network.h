#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QString>
#include <QTimer>

class networkStats : public QObject
{
    Q_OBJECT

public:
    explicit networkStats(QObject *parent = nullptr);
    ~networkStats() = default;

    QString getNetSend() const { return sendSpeed; }
    QString getNetReceive() const { return receiveSpeed; }
    void getIfaceData(QString interface);

signals:
    void updatedThroughput(QString received, QString sent);
    void updateIfaceData(QString name, QString type, QString ipv6, QString ipv4);

private slots:
    void updateNetStats(QString interface);

private:
    QTimer *m_timer;
    QString receiveSpeed;
    QString sendSpeed;
    quint64 current_rxBytes;
    quint64 current_txBytes;
    quint64 last_rxBytes;
    quint64 last_txBytes;
    QString ifaceName;
    QString ifaceType;
    QString ipv4Addr;
    QString ipv6Addr;
    bool m_firstRun;
};
#endif // NETWORK_H
