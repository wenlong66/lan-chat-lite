// SPDX-License-Identifier: GPL-3.0-or-later
#include "chatimage.h"
#include "clipboardimage.h"
#include <QCache>
#include <QFileInfo>
#include <QImageReader>
#include <QPainter>
#include <QTextDocument>

namespace {
constexpr qint64 MaxEncodedBytes = ClipboardImage::MaxDecodedBytes + 1024 * 1024;
}

bool ChatImage::isImageMessage(XmlMessage* message) {
  bool validSize = false;
  const qint64 bytes = message->data(XN_FILESIZE).toLongLong(&validSize);
  return message->data(XN_CONTENTTYPE) == "image/png" &&
         message->data(XN_FILETYPE) == "normal" &&
         message->data(XN_FILENAME).endsWith(".png", Qt::CaseInsensitive) &&
         validSize && bytes > 0 && bytes <= MaxEncodedBytes;
}

QImage ChatImage::thumbnail(const QString& path) {
  // Shared across conversations; only scaled ARGB32 thumbnails are cached.
  static QCache<QString, QImage> cache(8 * 1024 * 1024);
  const QFileInfo file(path);
  if(!file.isFile() || file.size() <= 0 || file.size() > MaxEncodedBytes)
    return QImage();
  const QString key = file.absoluteFilePath() + ':' + QString::number(file.size()) + ':' +
                      QString::number(file.lastModified().toMSecsSinceEpoch());
  if(const QImage* cached = cache.object(key))
    return *cached;

  QImageReader reader(path, "png");
  reader.setAutoDetectImageFormat(false);
  const QSize size = reader.size();
  const qint64 pixels = qint64(size.width()) * size.height();
  const int depth = QImage(1, 1, reader.imageFormat()).depth();
  if(!size.isValid() || pixels <= 0 || pixels > ClipboardImage::MaxPixels || depth <= 0 ||
     pixels * qMax(4, depth / 8) > ClipboardImage::MaxDecodedBytes)
    return QImage();
  reader.setScaledSize(size.boundedTo(size.scaled(320, 240, Qt::KeepAspectRatio)));
  QImage image = reader.read();
  if(!ClipboardImage::isWithinLimits(image))
    return QImage();
  image = image.scaled(image.size().boundedTo(QSize(320, 240)),
                       Qt::KeepAspectRatio, Qt::SmoothTransformation)
            .convertToFormat(QImage::Format_ARGB32);
  cache.insert(key, new QImage(image), int(image.sizeInBytes()));
  return image;
}

QSizeF ChatImage::intrinsicSize(QTextDocument* doc, int, const QTextFormat& format) {
  QSizeF size = format.property(Size).toSizeF();
  const qreal width = qMax(1.0, doc->textWidth() - 64.0);
  if(size.width() > width)
    size *= width / size.width();
  return size;
}

void ChatImage::drawObject(QPainter* painter, const QRectF& rect, QTextDocument*,
                           int, const QTextFormat& format) {
  const QImage image = thumbnail(format.property(Path).toString());
  if(image.isNull()) {
    painter->drawText(rect, Qt::AlignCenter, tr("Image unavailable"));
    return;
  }
  painter->save();
  painter->setRenderHint(QPainter::SmoothPixmapTransform);
  painter->drawImage(rect, image);
  painter->restore();
}
