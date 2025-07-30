#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_AddImageButton_clicked();
    void on_ClearListButton_clicked();
    void on_DeleteImageButton_clicked();
    void on_SelectLogoButton_clicked();
    void on_ApplyButton_clicked();
    QPixmap stampLogoOnImage(const QString &imagePath, const QString &logoPath);
    QPixmap stampLogoOnImage(const cv::Mat &inputImage, const QString &logoPath);
    void previewSelectedImage(int index);
    void previewLogoOnImage(const QString &path,const QString &logoPath);
    void onCustomCheckboxToggled(bool checked);
    void on_DeleteLogoButton_clicked();

private:
    QString selectedLogoPath;
    Ui::MainWindow *ui;
    bool checkbox1 = false;
    bool checkbox2 = false;
    bool checkbox3 = false;
    bool checkbox4 = false;
    bool customCheckBox = false;
};
#endif // MAINWINDOW_H
