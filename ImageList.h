#ifndef IMAGELIST_H
#define IMAGELIST_H
#include <QListWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

class ImageList : public QListWidget {
    Q_OBJECT

public:
    explicit ImageList(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // IMAGELIST_H
