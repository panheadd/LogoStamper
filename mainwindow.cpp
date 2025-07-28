#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "ImageList.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>

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
    connect(ui->ImageListW, &QListWidget::currentRowChanged, this, &MainWindow::previewSelectedImage);
    connect(ui->ImageListW, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int index = ui->ImageListW->row(item);
        previewSelectedImage(index);
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AddImageButton_clicked()
{

    QStringList paths = QFileDialog::getOpenFileNames(this, "Select Images", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    for (const QString &path : paths) {
        QListWidgetItem *item = new QListWidgetItem(QFileInfo(path).fileName());
        item->setData(Qt::UserRole, path);
        ui->ImageListW->addItem(item);
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

void MainWindow::previewSelectedImage(int index)
{
    if (index < 0 || index >= ui->ImageListW->count()) {
        ui->ImagePreviewLabel->clear();
        return;
    }

    QListWidgetItem *item = ui->ImageListW->item(index);
    QString filePath = item->data(Qt::UserRole).toString();

    QPixmap img(filePath);
    if (!img.isNull()) {
        ui->ImagePreviewLabel->setPixmap(img.scaled(
            ui->ImagePreviewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    }

    this->previewLogoOnImage(filePath ,this->selectedLogoPath);
}


QPixmap stampLogoOnImage(const QString &imagePath, const QString &logoPath)
{
    cv::Mat mainImg = cv::imread(imagePath.toStdString(), cv::IMREAD_COLOR);
    cv::Mat logoImg = cv::imread(logoPath.toStdString(), cv::IMREAD_UNCHANGED);

    if (mainImg.empty() || logoImg.empty())
        return QPixmap();

    int logoWidth = mainImg.cols / 4;
    int logoHeight = (logoWidth * logoImg.rows) / logoImg.cols;
    cv::resize(logoImg, logoImg, cv::Size(logoWidth, logoHeight));

    int x = mainImg.cols - logoImg.cols - 10;
    int y = mainImg.rows - logoImg.rows - 10;
    if (x < 0 || y < 0)
        return QPixmap();

    if (logoImg.channels() == 4) {
        for (int i = 0; i < logoImg.rows; ++i) {
            for (int j = 0; j < logoImg.cols; ++j) {
                cv::Vec4b logoPixel = logoImg.at<cv::Vec4b>(i, j);
                uchar alpha = logoPixel[3];

                if (alpha > 0) {
                    cv::Vec3b &mainPixel = mainImg.at<cv::Vec3b>(y + i, x + j);
                    for (int c = 0; c < 3; ++c)
                        mainPixel[c] = (logoPixel[c] * alpha + mainPixel[c] * (255 - alpha)) / 255;
                }
            }
        }
    }
    else if (logoImg.channels() == 3) {
        for (int i = 0; i < logoImg.rows; ++i) {
            for (int j = 0; j < logoImg.cols; ++j) {
                mainImg.at<cv::Vec3b>(y + i, x + j) = logoImg.at<cv::Vec3b>(i, j);
            }
        }
    } else {
        return QPixmap();
    }

    cv::cvtColor(mainImg, mainImg, cv::COLOR_BGR2RGB);
    QImage result((uchar *)mainImg.data, mainImg.cols, mainImg.rows, mainImg.step, QImage::Format_RGB888);
    return QPixmap::fromImage(result.copy());
}



void MainWindow::previewLogoOnImage(const QString &path, const QString &logoPath)
{
    if (path.isEmpty() || logoPath.isEmpty()) {

        return;
    }

    QPixmap result = stampLogoOnImage(path, logoPath);
    if (!result.isNull()) {
        ui->ResultPreviewLabel->setPixmap(result.scaled(
            ui->ResultPreviewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    } else {
        return;
    }
}

