#include "ImageList.h"
#include <QImageReader>

ImageList::ImageList(QWidget *parent) : QListWidget(parent){
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setDefaultDropAction(Qt::CopyAction);
}

void ImageList::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }
}

void ImageList::dropEvent(QDropEvent *event){
    const QList<QUrl> urls = event->mimeData()->urls();
    for(const QUrl &url : urls) {
        QString filePath = url.toLocalFile();
        QImageReader imageReader(filePath);
        if(imageReader.canRead()){
            addItem(filePath);
        }
    }
}
