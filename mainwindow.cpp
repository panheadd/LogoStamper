#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "ImageList.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600); // width x height
    connect(ui->AddImageButton, &QPushButton::clicked, this, &MainWindow::on_AddImageButton_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AddImageButton_clicked()
{

    QStringList paths = QFileDialog::getOpenFileNames(this, "Select Images", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    for (const QString &path : paths) {
        ui->ImageListW->addItem(path);
    }
}
