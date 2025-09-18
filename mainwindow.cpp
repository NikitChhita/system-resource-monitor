#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      count(0)
{
    ui->setupUi(this);

    //progress bar Styling
    ui->progressBar->setStyleSheet(
        "QProgressBar {"
        "  border: 2px solid grey;"
        "  border-radius: 5px;"
        "  background: #eee;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #3CB371;"
        "  width: 20px;"
        "  margin: 1px;"
        "}"
        );

    //setting progress bar to 0
    ui->progressBar->setValue(0);

    // Create timer
    timer = new QTimer(this);
    timer->setInterval(50); //update 100ms

    //connect button and timer to their functions
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startCount);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCounter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startCount() {
    count = 0;

    ui->progressBar->setValue(0);

    timer->start();
}

void MainWindow::updateCounter() {
    QString baseStyle =
        "QProgressBar {"
        "  border: 2px solid grey;"
        "  border-radius: 5px;"
        "  background: #eee;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: %1;"
        "}";

    if(count < 100) {
        count++;

        ui->progressBar->setValue(count);

        if(count < 33) {
            ui->progressBar->setStyleSheet(baseStyle.arg("red"));
        } else if (count < 66) {
            ui->progressBar->setStyleSheet(baseStyle.arg("yellow"));
        } else {
            ui->progressBar->setStyleSheet(baseStyle.arg("green"));
        }
    } else {
        timer->stop();
    }
}
