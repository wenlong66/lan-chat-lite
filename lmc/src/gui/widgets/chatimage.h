// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CHATIMAGE_H
#define CHATIMAGE_H

#include <QImage>
#include <QObject>
#include <QTextObjectInterface>
#include <QTextFormat>
#include "messaging/xmlmessage.h"

// A custom text object avoids QTextDocument retaining every decoded image.
class ChatImage : public QObject, public QTextObjectInterface {
  Q_OBJECT
  Q_INTERFACES(QTextObjectInterface)
public:
  explicit ChatImage(QObject* parent) : QObject(parent) {}
  enum { ObjectType = QTextFormat::UserObject + 1,
         Path = QTextFormat::UserProperty + 1, Size };
  static bool isImageMessage(XmlMessage* message);
  static QImage thumbnail(const QString& path);
  static QImage original(const QString& path);
  QSizeF intrinsicSize(QTextDocument* doc, int pos, const QTextFormat& format) override;
  void drawObject(QPainter* painter, const QRectF& rect, QTextDocument* doc,
                  int pos, const QTextFormat& format) override;
};

#endif
