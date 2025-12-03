#include "PageCustomization.h"
#include <QRegularExpression>
#include <QDebug>
#include <QTableWidget>
#include "UsageGraph.h"

void ColorUtils::setBackgroundColorDialog(QWidget* widget)
{
    QColorDialog* colorSelector = new QColorDialog(widget);
    colorSelector->setOption(QColorDialog::ShowAlphaChannel);

    // Saves original state before any changes in case user cancels dialog box
    QString ogWidgetStyle = widget->styleSheet();
    QMap<QPushButton*, QString> ogButtonStyles;
    for (QPushButton* button : widget->findChildren<QPushButton*>()) {
        ogButtonStyles[button] = button->styleSheet();
    }

    // Updates background live as user explores colors
    QObject::connect(colorSelector, &QColorDialog::currentColorChanged,[widget](const QColor &color) {
        applyBackgroundColor(widget, color);
    });

    // If user cancels, will restore the original styles
    QObject::connect(colorSelector, &QColorDialog::finished,[widget, ogWidgetStyle, ogButtonStyles](int result) {
        if (result == QDialog::Rejected) {
            widget->setStyleSheet(ogWidgetStyle);
            for (auto i = ogButtonStyles.begin(); i != ogButtonStyles.end(); ++i) {
                i.key()->setStyleSheet(i.value());
            }
        }
    });

    colorSelector->open();
    colorSelector->setAttribute(Qt::WA_DeleteOnClose);
}

void ColorUtils::setTextColorDialog(QWidget* widget)
{
    QColorDialog* colorSelector = new QColorDialog(widget);
    colorSelector->setOption(QColorDialog::ShowAlphaChannel);

    // Saves original states before any changes in case user cancels dialog box
    QMap<QLabel*, QString> ogLabelStyles;
    for (QLabel* label : widget->findChildren<QLabel*>()) {
        ogLabelStyles[label] = label->styleSheet();
    }

    QMap<QTableWidget*, QString> ogTableStyles;
    for (QTableWidget* table : widget->findChildren<QTableWidget*>()) {
        ogTableStyles[table] = table->styleSheet();
    }

    QMap<UsageGraph*, QColor> ogGraphStyles;
    for (UsageGraph* graph : widget->findChildren<UsageGraph*>()) {
        ogGraphStyles[graph] = graph->getTextColor();
    }

    // Updates background live as user explores colors
    QObject::connect(colorSelector, &QColorDialog::currentColorChanged,[widget](const QColor &color) {
        applyTextColor(widget, color);
    });

    // If user cancels, will restore the original styles
    QObject::connect(colorSelector, &QColorDialog::finished,[widget, ogLabelStyles, ogTableStyles, ogGraphStyles](int result) {
        if (result == QDialog::Rejected) {
            for (auto i = ogLabelStyles.begin(); i != ogLabelStyles.end(); ++i) {
                i.key()->setStyleSheet(i.value());
            }
            for (auto i = ogTableStyles.begin(); i != ogTableStyles.end(); ++i) {
                i.key()->setStyleSheet(i.value());
            }
            for (auto i = ogGraphStyles.begin(); i != ogGraphStyles.end(); ++i) {
                i.key()->setTextColor(i.value());
            }
        }
    });

    colorSelector->open();
    colorSelector->setAttribute(Qt::WA_DeleteOnClose);
}

void ColorUtils::applyBackgroundColor(QWidget* widget, const QColor &color)
{
    int brightness = (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
    QString buttonColor = (brightness > 128) ? "black" : "white";

    // Update background's color
    widget->setStyleSheet(QString("QWidget {background-color: %1}").arg(color.name()));

    // Applies buttons' colors
    for (QPushButton* button : widget->findChildren<QPushButton*>()) {
        button->setStyleSheet(QString("QPushButton { color: %1; font-size: 15px; max-width: 250px; border: 1px solid %1; border-radius: 2px}").arg(buttonColor));
    }
}

void ColorUtils::applyTextColor(QWidget* widget, const QColor &color)
{
    // Updates all labels' colors
    for (QLabel* label : widget->findChildren<QLabel*>()) {
        QString style = label->styleSheet();
        if (style.contains(QRegularExpression("color:\\s*[^;]+"))) {

            // If there's already a color property, replaces it
            style.replace(QRegularExpression("color:\\s*[^;]+"), QString("color: %1").arg(color.name()));
        } else {

            // If there isnt, just appends it to the stylesheet
            style += QString(" color: %1;").arg(color.name());
        }
        label->setStyleSheet(style);
    }

    // Updates all table elements' colors
    for (QTableWidget* table : widget->findChildren<QTableWidget*>()) {
        QString style = table->styleSheet();

        // Only replace the color property within QTableWidget rule
        if (style.contains(QRegularExpression("QTableWidget\\s*\\{[^}]*color:"))) {
            style.replace(QRegularExpression("(QTableWidget\\s*\\{[^}]*)color:\\s*[^;]+(;[^}]*\\})"), QString("\\1color: %1\\2").arg(color.name()));
        } else {
            // Add color to existing QTableWidget rule or create new one
            style.replace(QRegularExpression("(QTableWidget\\s*\\{)"), QString("\\1 color: %1;").arg(color.name()));
        }

        if (style.contains(QRegularExpression("QHeaderView::section\\s*\\{[^}]*color:"))) {
            style.replace(QRegularExpression("(QHeaderView::section\\s*\\{[^}]*)color:\\s*[^;]+(;[^}]*\\})"), QString("\\1color: %1\\2").arg(color.name()));
        } else {
            style += QString(" QHeaderView::section { color: %1; }").arg(color.name());
        }

        table->setStyleSheet(style);
    }



    // Updates all UsageGraph colors
    for (UsageGraph* graph : widget->findChildren<UsageGraph*>()) {
        graph->setTextColor(color);
    }
}

void ColorUtils::setAllStyles(QWidget* sourceWidget, QList<QWidget*> allPages)
{
    QString bgStyle = sourceWidget->styleSheet();

    // Extracts the background color from source
    QRegularExpression bgRegex("background-color:\\s*([^;\\}]+)");
    QRegularExpressionMatch bgMatch = bgRegex.match(bgStyle);

    if (!bgMatch.hasMatch()) return;

    QColor bgColor(bgMatch.captured(1).trimmed());

    // Gets text color from first label
    QColor textColor;
    QLabel* firstLabel = sourceWidget->findChild<QLabel*>();
    if (firstLabel) {
        QString labelStyle = firstLabel->styleSheet();
        QRegularExpression textRegex("color:\\s*([^;\\}]+)");
        QRegularExpressionMatch textMatch = textRegex.match(labelStyle);
        if (textMatch.hasMatch()) {
            textColor = QColor(textMatch.captured(1).trimmed());
        }
    }

    // Apply to all pages
    for (QWidget* page : allPages) {
        if (page != sourceWidget) {
            applyBackgroundColor(page, bgColor);
            if (textColor.isValid()) {
                applyTextColor(page, textColor);
            }
        }
    }
}
