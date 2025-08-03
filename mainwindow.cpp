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
    connect(ui->DeleteLogoButton, &QPushButton::clicked, this, &MainWindow::on_DeleteLogoButton_clicked);
    connect(ui->ApplyButton, &QPushButton::clicked, this, &MainWindow::on_ApplyButton_clicked);
    connect(ui->ImageListW, &QListWidget::currentRowChanged, this, &MainWindow::previewSelectedImage);

    connect(ui->sizeCheckBox, &QCheckBox::toggled, this, [=](bool checked) {
        checkbox1 = checked;
    });
    connect(ui->sizeCheckBox2, &QCheckBox::toggled, this, [=](bool checked) {
        checkbox2 = checked;
    });
    connect(ui->sizeCheckBox3, &QCheckBox::toggled, this, [=](bool checked) {
        checkbox3 = checked;
    });
    connect(ui->sizeCheckBox4, &QCheckBox::toggled, this, [=](bool checked) {
        checkbox4 = checked;
    });
    connect(ui->jpgCheckBox, &QCheckBox::toggled, this, [=](bool checked) {
        jpgCheckBox = checked;
    });
    ui->widthLineEdit->setEnabled(false);
    ui->heightLineEdit->setEnabled(false);

    connect(ui->customCheckBox, &QCheckBox::toggled, this, [=](bool checked) {
        customCheckBox = checked;
        ui->widthLineEdit->setEnabled(checked);
        ui->heightLineEdit->setEnabled(checked);
    });

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index){
        this->logoPosition = getSelectedLogoPosition(index);
        if (ui->ImageListW->count() > 0) {
            previewSelectedImage(ui->ImageListW->currentRow());
        }
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
    if (ui->ImageListW->count() == 1) {
        ui->ImageListW->setCurrentRow(0);
        previewSelectedImage(0);
    }
}

void MainWindow::on_ClearListButton_clicked(){
    ui->ImageListW->clear();
    ui->ResultPreviewLabel->clear();
}

void MainWindow::on_DeleteImageButton_clicked(){
    QList<QListWidgetItem*> selectedItems = ui->ImageListW->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        delete ui->ImageListW->takeItem(ui->ImageListW->row(item));
    }
    if(ui->ImageListW->count() == 0){
        ui->ResultPreviewLabel->clear();
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
            this->cachedLogoImage.release();
        }
        else{
            ui->LogoPreviewLabel->clear();
        }
    }
    if (ui->ImageListW->count() > 0) {
        previewSelectedImage(ui->ImageListW->currentRow());
    }
}

void MainWindow::on_DeleteLogoButton_clicked() {
    this->selectedLogoPath.clear();
    this->cachedLogoImage.release();
    ui->LogoPreviewLabel->clear();
    if (ui->ImageListW->count() > 0) {
        previewSelectedImage(ui->ImageListW->currentRow());
    }
}

void MainWindow::setScaledPixmap(QLabel *label, const QPixmap &pixmap) {
    if (!pixmap.isNull()) {
        label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        label->clear();
    }
}

cv::Mat MainWindow::getResizedLogo(int baseWidth) {
    if (!this->selectedLogoPath.isEmpty() && this->cachedLogoImage.empty()) {
        this->cachedLogoImage = cv::imread(this->selectedLogoPath.toStdString(), cv::IMREAD_UNCHANGED);
    }

    if (this->cachedLogoImage.empty())
        return {};

    int logoWidth = baseWidth / 4;
    int logoHeight = (logoWidth * cachedLogoImage.rows) / cachedLogoImage.cols;

    cv::Mat resizedLogo;
    cv::resize(cachedLogoImage, resizedLogo, cv::Size(logoWidth, logoHeight));
    return resizedLogo;
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

        this->previewLogoOnImage(filePath ,this->selectedLogoPath);
    }
    else{
        ui->ImagePreviewLabel->clear();
    }
}


QPixmap MainWindow::stampLogoOnImage(const QString &imagePath, const QString &logoPath)
{
    cv::Mat mainImg = cv::imread(imagePath.toStdString(), cv::IMREAD_COLOR);
    return stampLogoOnImage(mainImg, logoPath);
}


QPixmap MainWindow::stampLogoOnImage(const cv::Mat &inputImage, const QString & /*logoPath*/) {
    if (inputImage.empty())
        return QPixmap();

    cv::Mat mainImg = inputImage.clone();

    if (this->selectedLogoPath.isEmpty()) {
        cv::cvtColor(mainImg, mainImg, cv::COLOR_BGR2RGB);
        QImage result((uchar *)mainImg.data, mainImg.cols, mainImg.rows, mainImg.step, QImage::Format_RGB888);
        return QPixmap::fromImage(result.copy());
    }

    cv::Mat logoImg = getResizedLogo(mainImg.cols);
    if (logoImg.empty())
        return QPixmap();

    int margin = 10;
    int x = 0, y = 0;

    switch (this->logoPosition) {
    case LogoPosition::BottomRight:
        x = mainImg.cols - logoImg.cols - margin;
        y = mainImg.rows - logoImg.rows - margin;
        break;
    case LogoPosition::BottomLeft:
        x = margin;
        y = mainImg.rows - logoImg.rows - margin;
        break;
    case LogoPosition::TopRight:
        x = mainImg.cols - logoImg.cols - margin;
        y = margin;
        break;
    case LogoPosition::TopLeft:
        x = margin;
        y = margin;
        break;
    case LogoPosition::Center:
        x = (mainImg.cols - logoImg.cols) / 2;
        y = (mainImg.rows - logoImg.rows) / 2;
        break;
    default:
        x = margin;
        y = margin;
        break;
    }

    if (x < 0 || y < 0)
        return QPixmap();

    if (logoImg.channels() == 4) {
        const float opacity = 0.5f;
        for (int i = 0; i < logoImg.rows; ++i) {
            for (int j = 0; j < logoImg.cols; ++j) {
                cv::Vec4b logoPixel = logoImg.at<cv::Vec4b>(i, j);
                uchar alpha = static_cast<uchar>(logoPixel[3] * opacity);
                if (alpha > 0) {
                    cv::Vec3b &mainPixel = mainImg.at<cv::Vec3b>(y + i, x + j);
                    for (int c = 0; c < 3; ++c)
                        mainPixel[c] = (logoPixel[c] * alpha + mainPixel[c] * (255 - alpha)) / 255;
                }
            }
        }
    } else if (logoImg.channels() == 3) {
        for (int i = 0; i < logoImg.rows; ++i) {
            for (int j = 0; j < logoImg.cols; ++j) {
                mainImg.at<cv::Vec3b>(y + i, x + j) = logoImg.at<cv::Vec3b>(i, j);
            }
        }
    }

    cv::cvtColor(mainImg, mainImg, cv::COLOR_BGR2RGB);
    QImage result((uchar *)mainImg.data, mainImg.cols, mainImg.rows, mainImg.step, QImage::Format_RGB888);
    return QPixmap::fromImage(result.copy());
}



void MainWindow::previewLogoOnImage(const QString &path, const QString &logoPath)
{
    if (path.isEmpty()) {

        return;
    }
    if(logoPath.isEmpty()){
        QPixmap result;
        result.load(path);
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
    else{
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
}


void MainWindow::on_ApplyButton_clicked(){
    if(!(this->checkbox1||this->checkbox2||this->checkbox3||this->checkbox4||this->customCheckBox)){
        return;
    }
    QString saveDir = QFileDialog::getExistingDirectory(this, "Select Folder to Save Stamped Images");

    if (saveDir.isEmpty())
        return;

    QDir dir(saveDir);

    QStringList folders = {};
    QList<QSize> sizes = {};

    if(this->checkbox1){
        folders.push_back( "640x426");
        sizes.push_back(QSize(640, 426));
    }
    if(this->checkbox2){
        folders.push_back( "900x600");
        sizes.push_back(QSize(900, 600));
    }
    if(this->checkbox3){
        folders.push_back( "1620x1080");
        sizes.push_back(QSize(1620, 1080));
    }
    if(this->checkbox4){
        folders.push_back( "Original");
        sizes.push_back(QSize(0,0));
    }
    if(this->customCheckBox){
        QString width = ui->widthLineEdit->text();
        QString height = ui->heightLineEdit->text();

        if (!width.isEmpty() && !height.isEmpty()){
            bool widthOk = false;
            bool heightOk = false;

            int widthInt = width.toInt(&widthOk);
            int heightInt = height.toInt(&heightOk);

            if(widthOk && heightOk){
                QString folderName = width+"x"+height;
                folders.push_back(folderName);
                sizes.push_back(QSize(widthInt,heightInt));
            }

        }

    }


    for (const QString &folder : folders) {
        dir.mkdir(folder);
    }

    for (int i = 0; i < ui->ImageListW->count(); ++i)
    {
        QListWidgetItem *item = ui->ImageListW->item(i);
        QString imagePath = item->data(Qt::UserRole).toString();

        QImage originalImage(imagePath);
        if (originalImage.isNull())
            continue;

        QFileInfo fileInfo(imagePath);
        QString baseName = fileInfo.completeBaseName();
        QString extension = fileInfo.suffix();

        for (int j = 0; j < sizes.size(); ++j) {
            QSize targetSize = sizes[j];
            QString folderName = folders[j];

            cv::Mat input = cv::imread(imagePath.toStdString(), cv::IMREAD_COLOR);
            if (input.empty())
                continue;

            cv::Mat resizedInput;
            if(targetSize == QSize(0,0)){
                resizedInput = input;
            }
            else{
                cv::resize(input, resizedInput, cv::Size(targetSize.width(), targetSize.height()), 0, 0, cv::INTER_AREA);
            }

            QPixmap stamped = stampLogoOnImage(resizedInput, this->selectedLogoPath);

            if (stamped.isNull())
                continue;

            if(jpgCheckBox){
                extension = "jpg";
                QString savePath = saveDir + "/" + folderName + "/" + baseName + "_stamped." + extension;
                stamped.save(savePath,"JPG");
            }
            else{
                QString savePath = saveDir + "/" + folderName + "/" + baseName + "_stamped." + extension;
                stamped.save(savePath);
            }

        }
    }
}

MainWindow::LogoPosition MainWindow::getSelectedLogoPosition(int index) {
    switch (index) {
    case 0: return LogoPosition::Center;
    case 1: return LogoPosition::TopLeft;
    case 2: return LogoPosition::TopRight;
    case 3: return LogoPosition::BottomRight;
    case 4: return LogoPosition::BottomLeft;
    default: return LogoPosition::Center;
    }
}



