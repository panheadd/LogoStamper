#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void previewSelectedImage(int index);
    void previewLogoOnImage(const QString &logoPath);

private:
    QString selectedLogoPath;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
