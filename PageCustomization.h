#ifndef PAGECUSTOMIZATION_H
#define PAGECUSTOMIZATION_H

#include <QWidget>
#include <QColorDialog>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <QColor>

class ColorUtils
{
public:
    static void setBackgroundColorDialog(QWidget* widget);
    static void setTextColorDialog(QWidget* widget);
    static void setAllStyles(QWidget* sourceWidget, QList<QWidget*> allPages);

private:
    static void applyBackgroundColor(QWidget* widget, const QColor &color);
    static void applyTextColor(QWidget* widget, const QColor &color);
};

#endif // PAGECUSTOMIZATION_H
