#ifndef USAGEGRAPH_H
#define USAGEGRAPH_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

class UsageGraph : public QWidget
{
    Q_OBJECT

public:
    explicit UsageGraph(QString title,
                        bool displayLabels,
                        double minValue,
                        double maxValue,
                        QString minValueLabel,
                        QString maxValueLabel,
                        QString unit,
                        QWidget *parent = nullptr);

    explicit UsageGraph(bool displayLabels = true, QWidget *parent = nullptr);
    explicit UsageGraph(QString title, double minValue, double maxValue, QString unit, QWidget *parent = nullptr);
    explicit UsageGraph(QString title, double minValue, double maxValue,
                        QString minValueLabel, QString maxValueLabel, QString unit, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setRange(double minValue, double maxValue);
    void setRangeLabels(QString minValue, QString maxValue);
    void setUnit(const QString &unit);
    void addUtilizationValue(double value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QString m_title;
    double m_minValue;
    double m_maxValue;
    QString m_minValueLabel;
    QString m_maxValueLabel;
    QString m_unit;
    int m_maxPoints;
    bool m_displayLabels;
    QVector<double> m_history;
    QColor m_accentColor;
};

#endif // USAGEGRAPH_H

