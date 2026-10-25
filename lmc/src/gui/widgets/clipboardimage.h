// SPDX-License-Identifier: GPL-3.0-or-later
// Clipboard images are attachments, never embedded full-size in the editor.
#ifndef CLIPBOARDIMAGE_H
#define CLIPBOARDIMAGE_H

#include <QDir>
#include <QImage>
#include <QImageWriter>
#include <QSaveFile>
#include <QUuid>

namespace ClipboardImage {

constexpr qint64 MaxPixels = 16 * 1024 * 1024;
constexpr qint64 MaxDecodedBytes = 64 * 1024 * 1024;

inline bool isWithinLimits(const QImage& image) {
  return !image.isNull() &&
         qint64(image.width()) * image.height() <= MaxPixels &&
         image.sizeInBytes() <= MaxDecodedBytes;
}

// The caller owns the directory and decides when in-flight files can be removed.
// QSaveFile avoids offering an incomplete PNG when encoding/disk writes fail.
inline QString savePng(const QImage& image, const QString& directory, QString* error = nullptr) {
  if(!isWithinLimits(image)) {
    if(error)
      *error = QStringLiteral("The image exceeds the clipboard attachment limit.");
    return QString();
  }
  if(!QDir(directory).exists()) {
    if(error)
      *error = QStringLiteral("The temporary image folder is unavailable.");
    return QString();
  }
  const QString name = QStringLiteral("Screenshot-%1.png")
                         .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
  const QString path = QDir(directory).filePath(name);
  QSaveFile file(path);
  if(!file.open(QIODevice::WriteOnly)) {
    if(error)
      *error = file.errorString();
    return QString();
  }
  QImageWriter writer(&file, "png");
  writer.setCompression(1);
  if(!writer.write(image)) {
    if(error)
      *error = writer.errorString();
    return QString();
  }
  if(!file.commit()) {
    if(error)
      *error = file.errorString();
    return QString();
  }
  return path;
}

} // namespace ClipboardImage

#endif // CLIPBOARDIMAGE_H
