#ifndef MAINWINDOW_H

#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;

class QStackedWidget;

class QHBoxLayout;

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:

    void onPerformanceTabChanged(int index);

private:
    void setupUI();
    void setupPerformanceSidebar();
    void setupContentStack();
    void setupLayout();
    void applyStyles();
    void connectSignals();

    // UI components

    QWidget *centralWidget;
    QListWidget *performanceSidebar;
    QStackedWidget *contentStack;
    QHBoxLayout *mainLayout;
};

#endif // MAINWINDOW_H
