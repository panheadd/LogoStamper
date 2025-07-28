#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "ImageList.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1200, 800); // width x height
    connect(ui->AddImageButton, &QPushButton::clicked, this, &MainWindow::on_AddImageButton_clicked);
    connect(ui->ClearListButton, &QPushButton::clicked, this, &MainWindow::on_ClearListButton_clicked);
    connect(ui->DeleteImageButton, &QPushButton::clicked, this, &MainWindow::on_DeleteImageButton_clicked);
    connect(ui->SelectLogoButton, &QPushButton::clicked, this, &MainWindow::on_SelectLogoButton_clicked);


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

void MainWindow::on_ClearListButton_clicked(){
    ui->ImageListW->clear();
}

void MainWindow::on_DeleteImageButton_clicked(){
    QList<QListWidgetItem*> selectedItems = ui->ImageListW->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        delete ui->ImageListW->takeItem(ui->ImageListW->row(item));
    }
}

void MainWindow::on_SelectLogoButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Logo Image",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)"
        );

    if (!filePath.isEmpty()) {
        QPixmap logoPixmap(filePath);
        if (!logoPixmap.isNull()) {
            ui->LogoPreviewLabel->setPixmap(logoPixmap.scaled(
                ui->LogoPreviewLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));

            this->selectedLogoPath = filePath;
        }
    }
}
