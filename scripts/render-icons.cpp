// SPDX-License-Identifier: GPL-3.0-or-later
// Developer tool: render the original SVG artwork to dependency-free runtime assets.
// Build with Qt 6 Core/Gui/Svg, then run from the repository root.
#include <QBuffer>
#include <QDataStream>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QVector>

static QByteArray render(const QString& source, int size) {
  QSvgRenderer svg(source);
  if(!svg.isValid())
    qFatal("Invalid SVG: %s", qPrintable(source));
  QImage image(size, size, QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  QPainter painter(&image);
  svg.render(&painter);
  painter.end();
  QByteArray bytes;
  QBuffer buffer(&bytes);
  if(!image.save(&buffer, "PNG"))
    qFatal("PNG encoding failed");
  return bytes;
}

static void write(const QString& path, const QByteArray& bytes) {
  QFile file(path);
  if(!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size())
    qFatal("Cannot write %s", qPrintable(path));
}

int main() {
  const QDir dir("lmc/src/resources/lite-icons");
  for(const QString& file : dir.entryList({"*.svg"}, QDir::Files)) {
    const QString name = QFileInfo(file).completeBaseName();
    const QString source = dir.filePath(file);
    if(!name.endsWith("-tray"))
      write(dir.filePath(name + ".png"), render(source, name == "application" ? 256 : 64));
    if(name != "application" && !name.endsWith("-tray"))
      continue;
    const QVector<int> sizes{16, 24, 32, 48, 64, 128, 256};
    QVector<QByteArray> images;
    QByteArray ico;
    QDataStream stream(&ico, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << quint16(0) << quint16(1) << quint16(sizes.size());
    quint32 offset = 6 + sizes.size() * 16;
    for(int size : sizes) {
      images.append(render(source, size));
      stream << quint8(size == 256 ? 0 : size) << quint8(size == 256 ? 0 : size)
             << quint8(0) << quint8(0) << quint16(1) << quint16(32)
             << quint32(images.last().size()) << offset;
      offset += images.last().size();
    }
    for(const QByteArray& image : images)
      stream.writeRawData(image.constData(), image.size());
    write(dir.filePath(name + ".ico"), ico);
  }
  QDir().mkpath("artifacts/branding/LanChatLite.iconset");
  for(int size : {16, 32, 128, 256, 512}) {
    for(int scale : {1, 2}) {
      const QString path = QString("artifacts/branding/LanChatLite.iconset/icon_%1x%1%2.png")
                               .arg(size).arg(scale == 2 ? "@2x" : "");
      write(path, render(dir.filePath("application.svg"), size * scale));
    }
  }
}
