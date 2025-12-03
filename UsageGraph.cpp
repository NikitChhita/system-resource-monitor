#include "UsageGraph.h"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <utility>

UsageGraph::UsageGraph(QString title,
                       bool displayLabels,
                       double minValue,
                       double maxValue,
                       QString minValueLabel,
                       QString maxValueLabel,
                       QString unit,
                       QWidget *parent)
    : QWidget(parent)
    , m_title(std::move(title))
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_minValueLabel(std::move(minValueLabel))
    , m_maxValueLabel(std::move(maxValueLabel))
    , m_unit(std::move(unit))
    , m_maxPoints(60)
    , m_displayLabels(displayLabels)
{
    m_history.fill(0, m_maxPoints);
    m_rawHistory.fill(minValue, m_maxPoints);
    m_accentColor = QApplication::palette().highlight().color();
    setMinimumHeight(displayLabels ? 180 : 40);
    setMinimumWidth(displayLabels ? 220 : 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

UsageGraph::UsageGraph(bool displayLabels, QWidget *parent)
    : UsageGraph("", displayLabels, 0.0, 100.0, "0", "100", "%", parent)
{}

UsageGraph::UsageGraph(QString title, double minValue, double maxValue, QString unit, QWidget *parent)
    : UsageGraph(std::move(title),
                 true,
                 minValue,
                 maxValue,
                 QString::number(minValue),
                 QString::number(maxValue),
                 std::move(unit),
                 parent)
{}

void UsageGraph::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void UsageGraph::setRange(double minValue, double maxValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;

    // renormalizes history every time range is changed
    m_history.clear();
    for (double rawValue : m_rawHistory) {
        double clamped = qBound(m_minValue, rawValue, m_maxValue);
        double normalized = ((clamped - m_minValue) / (m_maxValue - m_minValue)) * 100.0;
        m_history.append(normalized);
    }

    update();
}

void UsageGraph::setRangeLabels(QString minValue, QString maxValue)
{
    m_minValueLabel = std::move(minValue);
    m_maxValueLabel = std::move(maxValue);
    update();
}

void UsageGraph::setUnit(const QString &unit)
{
    m_unit = unit;
    update();
}

void UsageGraph::addUtilizationValue(double value)
{
    if (m_rawHistory.size() >= m_maxPoints)
        m_rawHistory.removeFirst();
    m_rawHistory.append(value);

    value = qBound(m_minValue, value, m_maxValue);
    double normalized = ((value - m_minValue) / (m_maxValue - m_minValue)) * 100.0;

    if (m_history.size() >= m_maxPoints)
        m_history.removeFirst();
    m_history.append(normalized);
    update();
}

void UsageGraph::setTextColor(const QColor &color){
    m_textColor = color;
    update();
}

QColor UsageGraph::getTextColor(){
    return m_textColor;
}

void UsageGraph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    int titleHeight = m_displayLabels ? 18 : 0;
    int leftPadding = m_displayLabels ? 5 : 0;
    int rightPadding = m_displayLabels ? 5 : 0;
    int topPadding = m_displayLabels ? titleHeight + qMin(15, qMax(5, h / 40)) : 0;
    int bottomPadding = m_displayLabels ? qMax(20, h / 14) : 0;

    int graphHeight = h - (topPadding + bottomPadding);
    int graphWidth = w - (leftPadding + rightPadding);

    if (m_displayLabels) {
        QColor textColor = m_textColor;
        textColor.setAlpha(200);
        QFont titleFont = font();
        titleFont.setPointSize(10);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.setPen(textColor);
        QRect titleRect(leftPadding, 5, w - 70, titleHeight);
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_title);

        QFont labelFont = font();
        labelFont.setPointSize(8);
        painter.setFont(labelFont);

        QString maxLabel = m_maxValueLabel + m_unit;
        QRect maxLabelRect(w - 65, 5, 60, titleHeight);
        painter.drawText(maxLabelRect, Qt::AlignRight | Qt::AlignVCenter, maxLabel);

        QRect timeRect(leftPadding, h - bottomPadding + 5, w - 70, 20);
        painter.drawText(timeRect, Qt::AlignLeft | Qt::AlignVCenter, "60 seconds");

        QString minLabel = m_minValueLabel + m_unit;
        QRect minLabelRect(w - 65, h - bottomPadding + 5, 60, 20);
        painter.drawText(minLabelRect, Qt::AlignRight | Qt::AlignVCenter, minLabel);
    }

    double xStep = static_cast<double>(graphWidth) / (m_maxPoints - 1);

    if (m_displayLabels) {
        QPen gridPen(QColor(m_textColor.red(), m_textColor.green(), m_textColor.blue(), 30));
        gridPen.setWidth(0.5);
        gridPen.setStyle(Qt::SolidLine);
        painter.setPen(gridPen);

        for (int i = 0; i <= 10; i++) {
            int y = topPadding + graphHeight - (i * graphHeight / 10);
            painter.drawLine(leftPadding, y, w - rightPadding, y);
        }

        for (int i = 0; i <= m_maxPoints; i += 5) {
            int x = static_cast<int>(leftPadding + i * xStep);
            painter.drawLine(x, topPadding, x, h - bottomPadding);
        }
    }

    if (!m_history.isEmpty()) {
        QPainterPath path;
        QPainterPath fillPath;

        fillPath.moveTo(leftPadding, h - bottomPadding);
        double initialX = leftPadding;
        double initialY = topPadding + graphHeight - (m_history.first() * graphHeight / 100.0);
        path.moveTo(initialX, initialY);
        fillPath.lineTo(initialX, initialY);

        for (int i = 1; i < m_history.size(); ++i) {
            double x1 = leftPadding + (i - 1) * xStep;
            double y1 = topPadding + graphHeight - (m_history.at(i - 1) * graphHeight / 100.0);
            double x2 = leftPadding + i * xStep;
            double y2 = topPadding + graphHeight - (m_history.at(i) * graphHeight / 100.0);
            double cx = (x1 + x2) / 2;
            double cy = (y1 + y2) / 2;
            path.quadTo(x1, y1, cx, cy);
            fillPath.quadTo(x1, y1, cx, cy);
        }

        fillPath.lineTo(leftPadding + static_cast<double>(m_history.size() - 1) * xStep,
                        h - bottomPadding);
        fillPath.lineTo(leftPadding, h - bottomPadding);

        QLinearGradient gradient(leftPadding, 0, w - rightPadding, 0);
        gradient.setColorAt(0.0, QColor(m_textColor.red(), m_textColor.green(), m_textColor.blue(), 80));
        gradient.setColorAt(1.0, QColor(m_textColor.red(), m_textColor.green(), m_textColor.blue(), 10));
        painter.fillPath(fillPath, gradient);

        QPen linePen(m_textColor);
        linePen.setWidth(2);
        painter.setPen(linePen);
        painter.drawPath(path);

        QColor borderColor = QColor(m_textColor.red(), m_textColor.green(), m_textColor.blue(), 150);
        QPen borderPen(borderColor);
        borderPen.setWidth(1);
        borderPen.setStyle(Qt::SolidLine);
        painter.setPen(borderPen);
        painter.drawRect(leftPadding, topPadding, graphWidth, graphHeight);
    }
}

void UsageGraph::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
