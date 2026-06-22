#include "EmptyStateWidget.h"

#include <QMimeData>
#include <qevent.h>


EmptyStateWidget::EmptyStateWidget(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);

    auto *layout = new QVBoxLayout(this);

    // Icon (Using QIcon or a simple SVG)
    QLabel *icon = new QLabel("📁");
    icon->setAlignment(Qt::AlignCenter);

    // Text
    QLabel *title = new QLabel("Your library is empty");
    title->setAlignment(Qt::AlignCenter);

    // CTA Button
    QPushButton *btn = new QPushButton("Add Video Files");

    layout->addStretch();
    layout->addWidget(icon);
    layout->addWidget(title);
    layout->addWidget(btn);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, &EmptyStateWidget::addVideoFile_Clicked);
}

void EmptyStateWidget::addVideoFile_Clicked()
{
    emit requestFileImport();
}

void EmptyStateWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Only accept the drop if it contains URLs (files)
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void EmptyStateWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QStringList filePaths;
    for (const QUrl &url : urls) {
        // Convert file URL to local path
        filePaths.append(url.toLocalFile());
    }

    // Notify the parent (or controller) that files were dropped
    emit filesDropped(filePaths);
}
