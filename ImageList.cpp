#include "ImageList.h"
#include <QImageReader>

ImageList::ImageList(QWidget *parent) : QListWidget(parent){
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setDefaultDropAction(Qt::CopyAction);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

}

void ImageList::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasUrls()){
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();

    }
}

void ImageList::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageList::dropEvent(QDropEvent *event){

        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            QString filePath = url.toLocalFile();

            QImageReader reader(filePath);
            if (reader.canRead()) {
                addItem(filePath);
            }
        }

        event->acceptProposedAction();
    }
