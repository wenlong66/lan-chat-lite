/****************************************************************************
**
** This file is part of LAN Messenger.
** 
** Copyright (c) 2010 - 2012 Qualia Digital Solutions.
** 
** Contact:  qualiatech@gmail.com
** 
** LAN Messenger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** LAN Messenger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with LAN Messenger.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#include <QDesktopServices>
#include <QTimer>
#include <QMimeData>
#include <QMessageBox>
#include <QTemporaryDir>
#include <algorithm>
#include "chatwindow.h"
#include "gui/widgets/actionhelper.h"
#include "gui/widgets/attachmentdialog.h"
#include "gui/widgets/clipboardimage.h"
#include "gui/widgets/chatimage.h"

const qint64 pauseTime = 5000;

namespace {
// Application-session lifetime: closing a conversation must not remove a PNG
// while the peer is still accepting/downloading it. Completed screenshots move
// to sent-image storage; the directory removes leftovers at orderly app exit.
QTemporaryDir& clipboardAttachmentDirectory() {
  static QTemporaryDir directory(QDir::tempPath() + QStringLiteral("/lanchat-lite-clipboard-XXXXXX"));
  return directory;
}

bool isClipboardAttachment(const QString& path) {
  return !path.isEmpty() && clipboardAttachmentDirectory().isValid() &&
         QFileInfo(path).absolutePath() == clipboardAttachmentDirectory().path() &&
         QFileInfo(path).fileName().startsWith(QStringLiteral("Screenshot-"));
}
}

lmcChatWindow::lmcChatWindow(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags) {
  ui.setupUi(this);
#ifdef LMC_LITE_BUILD
  resize(620, 560);
  setMinimumSize(440, 360);
  ui.verticalLayout_4->setContentsMargins(12, 8, 12, 12);
  ui.wgtToolBar->setMinimumHeight(44);
  ui.wgtToolBar->setMaximumHeight(QWIDGETSIZE_MAX);
  ui.txtMessage->setMinimumHeight(100);
  ui.msgRowLayout->setSpacing(8);
  ui.btnSend->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  ui.btnSend->setFocusPolicy(Qt::StrongFocus);
  ui.msgRowLayout->setAlignment(ui.btnSend, Qt::AlignBottom);
  ui.lblDividerTop->hide();
  ui.lblDividerBottom->hide();
#endif
  //	Destroy the window when it closes
  setAttribute(Qt::WA_DeleteOnClose, true);
  setAcceptDrops(true);

  pMessageLog = new lmcMessageLog(ui.wgtLog);
  ui.logLayout->addWidget(pMessageLog);
  pMessageLog->setAcceptDrops(true);
  pMessageLog->viewport()->installEventFilter(this);
  ui.txtMessage->viewport()->installEventFilter(this);
  connect(pMessageLog, &lmcMessageLog::messageSent, this, &lmcChatWindow::log_sendMessage);

  int bottomPanelHeight = ui.txtMessage->minimumHeight() + ui.lblDividerBottom->minimumHeight() +
                          ui.lblDividerTop->minimumHeight() + ui.wgtToolBar->minimumHeight();
  QList<int> sizes;
  sizes.append(height() - bottomPanelHeight - ui.splitter->handleWidth());
  sizes.append(bottomPanelHeight);
  ui.splitter->setSizes(sizes);
#ifndef LMC_LITE_BUILD
  ui.splitter->setStyleSheet("QSplitter::handle { image: url(" IDR_VGRIP "); }");
#endif

  ui.lblInfo->setBackgroundRole(QPalette::Base);
  ui.lblInfo->setAutoFillBackground(true);
  ui.lblInfo->setVisible(false);
  pMessageLog->installEventFilter(this);
  ui.txtMessage->installEventFilter(this);
  ui.btnSend->installEventFilter(this);
  connect(ui.btnSend, &QAbstractButton::clicked, this, &lmcChatWindow::btnSend_clicked);
  connect(ui.txtMessage, &lmcMessageEdit::imagePasted, this, &lmcChatWindow::pasteImage);
  connect(ui.txtMessage, &lmcMessageEdit::imagePasteRejected, this, &lmcChatWindow::imagePasteRejected);
  infoFlag = IT_Ok;

  localId = QString();
  localName = QString();
  peerIds.clear();
  peerNames.clear();
  peerStatuses.clear();
  peerCaps.clear();
  threadId = QString();
  groupMode = false;
  dataSaved = false;

  chatState = CS_Blank;
  keyStroke = 0;
}

lmcChatWindow::~lmcChatWindow(void) {
}

void lmcChatWindow::init(User* pLocalUser, User* pRemoteUser, bool connected) {
  localId = pLocalUser->id;
  localName = pLocalUser->name;

  peerId = pRemoteUser->id;
  peerIds.insert(peerId, pRemoteUser->id);
  peerNames.insert(peerId, pRemoteUser->name);
  peerStatuses.insert(peerId, pRemoteUser->status);
  peerCaps.insert(peerId, pRemoteUser->caps);

  this->pLocalUser = pLocalUser;

  pMessageLog->localId = localId;
  pMessageLog->peerId = peerId;
  pMessageLog->peerName = pRemoteUser->name;

  //	get the avatar image for the users
  pMessageLog->participantAvatars.insert(localId, pLocalUser->avatarPath);
  pMessageLog->participantAvatars.insert(peerId, pRemoteUser->avatarPath);

  createSmileyMenu();
  createToolBar();

  bConnected = connected;
  if(!bConnected)
    showStatus(IT_Disconnected, true);

  int index = Helper::statusIndexFromCode(pRemoteUser->status);
  if(index != -1) {
    setWindowIcon(QIcon(bubblePic[index]));
    if(statusType[index] == StatusTypeOffline)
      showStatus(IT_Offline, true);
    else if(statusType[index] == StatusTypeAway)
      showStatus(IT_Away, true);
    else if(statusType[index] == StatusTypeBusy)
      showStatus(IT_Busy, true);
  }

  pSoundPlayer = new lmcSoundPlayer();

  pSettings = new lmcSettings();
  showSmiley = pSettings->value(IDS_EMOTICON, IDS_EMOTICON_VAL).toBool();
  pMessageLog->showSmiley = showSmiley;
  pMessageLog->autoFile = pSettings->value(IDS_AUTOFILE, IDS_AUTOFILE_VAL).toBool();
  pMessageLog->fontSizeVal = pSettings->value(IDS_FONTSIZE, IDS_FONTSIZE_VAL).toInt();
  pMessageLog->messageTime = pSettings->value(IDS_MESSAGETIME, IDS_MESSAGETIME_VAL).toBool();
  pMessageLog->messageDate = pSettings->value(IDS_MESSAGEDATE, IDS_MESSAGEDATE_VAL).toBool();
  pMessageLog->allowLinks = pSettings->value(IDS_ALLOWLINKS, IDS_ALLOWLINKS_VAL).toBool();
  pMessageLog->pathToLink = pSettings->value(IDS_PATHTOLINK, IDS_PATHTOLINK_VAL).toBool();
  pMessageLog->trimMessage = pSettings->value(IDS_TRIMMESSAGE, IDS_TRIMMESSAGE_VAL).toBool();
  QFont font = QApplication::font();
  font.fromString(pSettings->value(IDS_FONT, IDS_FONT_VAL).toString());
  messageColor = QApplication::palette().text().color();
  messageColor.setNamedColor(pSettings->value(IDS_COLOR, IDS_COLOR_VAL).toString());
  sendKeyMod = pSettings->value(IDS_SENDKEYMOD, IDS_SENDKEYMOD_VAL).toBool();
  clearOnClose = pSettings->value(IDS_CLEARONCLOSE, IDS_CLEARONCLOSE_VAL).toBool();
  showSendButton = pSettings->value(IDS_SHOWSENDBUTTON, IDS_SHOWSENDBUTTON_VAL).toBool();
  ui.btnSend->setVisible(showSendButton);

  setUIText();

  setMessageFont(font);
  ui.txtMessage->setStyleSheet("QTextEdit {color: " + messageColor.name() + ";}");
  ui.txtMessage->setFocus();

  QString themePath = pSettings->value(IDS_THEME, IDS_THEME_VAL).toString();
  pMessageLog->initMessageLog(themePath);
  if(!clearOnClose)
    pMessageLog->restoreMessageLog(QDir(StdLocation::cacheDir()).absoluteFilePath("msg_" + peerId + ".tmp"));
}

void lmcChatWindow::stop(void) {
  bool saveHistory = pSettings->value(IDS_HISTORY, IDS_HISTORY_VAL).toBool();
  if(pMessageLog->hasData && saveHistory && !dataSaved) {
    if(clearOnClose) {
      QString szMessageLog = pMessageLog->prepareMessageLogForSave();
      if(!groupMode)
        History::save(peerNames.value(peerId), QDateTime::currentDateTime(), &szMessageLog);
      else
        History::save(tr("Group Conversation"), QDateTime::currentDateTime(), &szMessageLog);
      dataSaved = true;
    } else {
      pMessageLog->saveMessageLog(QDir(StdLocation::cacheDir()).absoluteFilePath("msg_" + peerId + ".tmp"));
    }
  }
}

void lmcChatWindow::receiveMessage(MessageType type, QString* lpszUserId, XmlMessage* pMessage) {
  QString title;
  int statusIndex;

  //	if lpszUserId is NULL, the message was sent locally
  QString senderId = lpszUserId ? *lpszUserId : localId;
  QString senderName = lpszUserId ? peerNames.value(senderId) : localName;
  QString data;

  switch(type) {
  case MT_Message:
    appendMessageLog(type, lpszUserId, &senderName, pMessage);
    if(isHidden() || !isActiveWindow()) {
      pSoundPlayer->play(SE_NewMessage);
      title = tr("%1 says...");
      setWindowTitle(title.arg(senderName));
    }
    break;
  case MT_Broadcast:
    appendMessageLog(type, lpszUserId, &senderName, pMessage);
    if(isHidden() || !isActiveWindow()) {
      pSoundPlayer->play(SE_NewMessage);
      title = tr("Broadcast from %1");
      setWindowTitle(title.arg(senderName));
    }
    break;
  case MT_ChatState:
    appendMessageLog(type, lpszUserId, &senderName, pMessage);
    break;
  case MT_Status:
    data = pMessage->data(XN_STATUS);
    statusIndex = Helper::statusIndexFromCode(data);
    if(statusIndex != -1) {
      setWindowIcon(QIcon(bubblePic[statusIndex]));
      statusType[statusIndex] == StatusTypeOffline ? showStatus(IT_Offline, true) : showStatus(IT_Offline, false);
      statusType[statusIndex] == StatusTypeBusy ? showStatus(IT_Busy, true) : showStatus(IT_Busy, false);
      statusType[statusIndex] == StatusTypeAway ? showStatus(IT_Away, true) : showStatus(IT_Away, false);
      peerStatuses.insert(senderId, data);
    }
    break;
  case MT_Avatar:
    data = pMessage->data(XN_FILEPATH);
    // this message may come with or without user id. NULL user id means avatar change
    // by local user, while non NULL user id means avatar change by a peer.
    pMessageLog->updateAvatar(&senderId, &data);
    break;
  case MT_UserName:
    data = pMessage->data(XN_NAME);
    if(peerNames.contains(senderId)) {
      peerNames.insert(senderId, data);
      pMessageLog->peerName = data;
    }
    pMessageLog->updateUserName(&senderId, &data);
    break;
  case MT_Failed:
    appendMessageLog(type, lpszUserId, &senderName, pMessage);
    break;
  case MT_File:
  case MT_Folder:
    if(pMessage->data(XN_FILEOP) == FileOpNames[FO_Request]) {
      const QString filePath = pMessage->data(XN_FILEPATH);
      if(pMessage->data(XN_MODE) == FileModeNames[FM_Send] && isClipboardAttachment(filePath))
        clipboardTransfers.insert(pMessage->data(XN_FILEID), filePath);
      //	a file request has been received
      appendMessageLog(type, lpszUserId, &senderName, pMessage);
      if(pMessage->data(XN_MODE) == FileModeNames[FM_Receive] && (isHidden() || !isActiveWindow())) {
        pSoundPlayer->play(SE_NewFile);
        if(type == MT_File && ChatImage::isImageMessage(pMessage))
          title = tr("%1 sends an image...");
        else if(type == MT_File)
          title = tr("%1 sends a file...");
        else
          title = tr("%1 sends a folder...");
        setWindowTitle(title.arg(senderName));
      }
    } else {
      // a file message of op other than request has been received
      processFileOp(pMessage);
    }
    break;
  case MT_Depart:
    pMessageLog->abortPendingFileOperations();
    break;
  default:
    break;
  }
}

void lmcChatWindow::connectionStateChanged(bool connected) {
  bConnected = connected;
  bConnected ? showStatus(IT_Disconnected, false) : showStatus(IT_Disconnected, true);
}

void lmcChatWindow::settingsChanged(void) {
  showSmiley = pSettings->value(IDS_EMOTICON, IDS_EMOTICON_VAL).toBool();
  pMessageLog->showSmiley = showSmiley;
  pMessageLog->fontSizeVal = pSettings->value(IDS_FONTSIZE, IDS_FONTSIZE_VAL).toInt();
  pMessageLog->autoFile = pSettings->value(IDS_AUTOFILE, IDS_AUTOFILE_VAL).toBool();
  sendKeyMod = pSettings->value(IDS_SENDKEYMOD, IDS_SENDKEYMOD_VAL).toBool();
  ui.btnSend->setToolTip(tr("Send message") + QStringLiteral(" (%1)").arg(
      QKeySequence(sendKeyMod ? Qt::CTRL | Qt::Key_Return : Qt::Key_Return).toString(QKeySequence::NativeText)));
  clearOnClose = pSettings->value(IDS_CLEARONCLOSE, IDS_CLEARONCLOSE_VAL).toBool();
  showSendButton = pSettings->value(IDS_SHOWSENDBUTTON, IDS_SHOWSENDBUTTON_VAL).toBool();
  ui.btnSend->setVisible(showSendButton);
  pSoundPlayer->settingsChanged();
  if(localName.compare(pLocalUser->name) != 0) {
    localName = pLocalUser->name;
    pMessageLog->updateUserName(&localId, &localName);
  }

  bool msgTime = pSettings->value(IDS_MESSAGETIME, IDS_MESSAGETIME_VAL).toBool();
  bool msgDate = pSettings->value(IDS_MESSAGEDATE, IDS_MESSAGEDATE_VAL).toBool();
  bool allowLinks = pSettings->value(IDS_ALLOWLINKS, IDS_ALLOWLINKS_VAL).toBool();
  bool pathToLink = pSettings->value(IDS_PATHTOLINK, IDS_PATHTOLINK_VAL).toBool();
  bool trim = pSettings->value(IDS_TRIMMESSAGE, IDS_TRIMMESSAGE_VAL).toBool();
  QString theme = pSettings->value(IDS_THEME, IDS_THEME_VAL).toString();
  if(msgTime != pMessageLog->messageTime || msgDate != pMessageLog->messageDate ||
     allowLinks != pMessageLog->allowLinks || pathToLink != pMessageLog->pathToLink ||
     trim != pMessageLog->trimMessage || theme.compare(pMessageLog->themePath) != 0) {
    pMessageLog->messageTime = msgTime;
    pMessageLog->messageDate = msgDate;
    pMessageLog->allowLinks = allowLinks;
    pMessageLog->pathToLink = pathToLink;
    pMessageLog->trimMessage = trim;
    pMessageLog->themePath = theme;
    pMessageLog->reloadMessageLog();
  }
}

bool lmcChatWindow::eventFilter(QObject* pObject, QEvent* pEvent) {
  if((pObject == ui.txtMessage->viewport() || pObject == pMessageLog->viewport()) &&
     (pEvent->type() == QEvent::DragEnter || pEvent->type() == QEvent::DragMove ||
      pEvent->type() == QEvent::Drop) &&
     static_cast<QDropEvent*>(pEvent)->mimeData()->hasUrls()) {
    const auto urls = static_cast<QDropEvent*>(pEvent)->mimeData()->urls();
    if(std::none_of(urls.cbegin(), urls.cend(), [](const QUrl& url) { return url.isLocalFile(); }))
      return false;
    // QTextEdit otherwise inserts file URLs as text instead of sending them.
    if(pEvent->type() == QEvent::DragEnter)
      dragEnterEvent(static_cast<QDragEnterEvent*>(pEvent));
    else if(pEvent->type() == QEvent::DragMove)
      dragMoveEvent(static_cast<QDragMoveEvent*>(pEvent));
    else
      dropEvent(static_cast<QDropEvent*>(pEvent));
    return true;
  }

  if(pEvent->type() == QEvent::KeyPress) {
    QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(pEvent);
    if(pObject == ui.txtMessage) {
      if(pKeyEvent->key() == Qt::Key_Return || pKeyEvent->key() == Qt::Key_Enter) {
        bool keyMod = ((pKeyEvent->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);
        if(keyMod == sendKeyMod) {
          sendMessage();
          setChatState(CS_Active);
          return true;
        }
        // The TextEdit widget does not insert new line when Ctrl+Enter is pressed
        // So we insert a new line manually
        if(keyMod)
          ui.txtMessage->insertPlainText("\n");
      } else if(pKeyEvent->key() == Qt::Key_Escape) {
        close();
        return true;
      }
      keyStroke++;
      setChatState(CS_Composing);
    } else {
      if(pKeyEvent->key() == Qt::Key_Escape) {
        close();
        return true;
      }
    }
  }

  return false;
}

void lmcChatWindow::changeEvent(QEvent* pEvent) {
  switch(pEvent->type()) {
  case QEvent::ActivationChange:
    if(isActiveWindow())
      setWindowTitle(getWindowTitle());
    break;
  case QEvent::LanguageChange:
    setUIText();
    break;
  default:
    break;
  }

  QWidget::changeEvent(pEvent);
}

void lmcChatWindow::closeEvent(QCloseEvent* pEvent) {
  setChatState(CS_Inactive);
  // Call stop() to save history
  stop();
  emit closed(&peerId);

  QWidget::closeEvent(pEvent);
}

void lmcChatWindow::dragEnterEvent(QDragEnterEvent* pEvent) {
  dragMoveEvent(pEvent);
}

void lmcChatWindow::dragMoveEvent(QDragMoveEvent* pEvent) {
  if(pEvent->possibleActions().testFlag(Qt::CopyAction) && !droppedPaths(pEvent->mimeData()).isEmpty()) {
    pEvent->setDropAction(Qt::CopyAction);
    pEvent->accept();
  } else
    pEvent->ignore();
}

QStringList lmcChatWindow::droppedPaths(const QMimeData* mimeData) const {
  QStringList paths;
  if(!bConnected)
    return paths;
  const uint caps = peerCaps.value(peerId);
  for(const QUrl& url : mimeData->urls()) {
    if(!url.isLocalFile())
      continue;
    const QString path = url.toLocalFile();
    const QFileInfo info(path);
    if((info.isFile() && (caps & UC_File)) || (info.isDir() && (caps & UC_Folder)))
      paths.append(path);
  }
  return paths;
}

void lmcChatWindow::dropEvent(QDropEvent* pEvent) {
  const QStringList paths = droppedPaths(pEvent->mimeData());
  if(!pEvent->possibleActions().testFlag(Qt::CopyAction) || paths.isEmpty()) {
    pEvent->ignore();
    return;
  }
  for(QString path : paths) {
    if(QFileInfo(path).isDir())
      sendFolder(&path);
    else
      sendFile(&path);
  }
  pEvent->setDropAction(Qt::CopyAction);
  pEvent->accept();
}

void lmcChatWindow::btnFont_clicked(void) {
  bool ok;
  QFont font = ui.txtMessage->font();
  font.setPointSize(ui.txtMessage->fontPointSize());
  QFont newFont = QFontDialog::getFont(&ok, font, this, tr("Select Font"));
  if(ok)
    setMessageFont(newFont);
}

void lmcChatWindow::btnFontColor_clicked(void) {
  QColor color = QColorDialog::getColor(messageColor, this, tr("Select Color"));
  if(color.isValid()) {
    messageColor = color;
    ui.txtMessage->setStyleSheet("QTextEdit {color: " + messageColor.name() + ";}");
  }
}

void lmcChatWindow::btnFile_clicked(void) {
  QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
  const QStringList paths = AttachmentDialog::files(this, dir);
  if(!paths.isEmpty())
    pSettings->setValue(IDS_OPENPATH, QFileInfo(paths.first()).absolutePath());
  for(QString fileName : paths)
    sendFile(&fileName);
}

void lmcChatWindow::btnFolder_clicked(void) {
  QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
  const QStringList paths = AttachmentDialog::folders(this, dir);
  if(!paths.isEmpty())
    pSettings->setValue(IDS_OPENPATH, QFileInfo(paths.first()).absolutePath());
  for(QString path : paths)
    sendFolder(&path);
}

void lmcChatWindow::btnSave_clicked(void) {
  QString dir = pSettings->value(IDS_SAVEPATH, IDS_SAVEPATH_VAL).toString();
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Conversation"), dir,
                                                  "HTML File (*.html);;Text File (*.txt)");
  if(!fileName.isEmpty()) {
    pSettings->setValue(IDS_SAVEPATH, QFileInfo(fileName).dir().absolutePath());
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return;
    QTextStream stream(&file);
    Helper::setUtf8Stream(stream);
    stream.setGenerateByteOrderMark(true);
    if(fileName.endsWith(".html", Qt::CaseInsensitive))
      stream << pMessageLog->prepareMessageLogForSave();
    else
      stream << pMessageLog->prepareMessageLogForSave(TextFormat);
    file.close();
  }
}

void lmcChatWindow::btnHistory_clicked(void) {
  emit showHistory();
}

void lmcChatWindow::btnTransfers_clicked(void) {
  emit showTransfers();
}

void lmcChatWindow::imagePasteRejected(void) {
  QMessageBox::information(this, tr("Image too large"),
    tr("Clipboard images must be at most 16 megapixels and 64 MiB when decoded. "
       "Save larger images to a file and use Send File instead."));
}

void lmcChatWindow::pasteImage(const QImage& image) {
  if(!ClipboardImage::isWithinLimits(image)) {
    imagePasteRejected();
    return;
  }
  if(groupMode || !bConnected || (infoFlag & IT_Offline) ||
     (peerCaps.value(peerId) & UC_File) != UC_File) {
    QMessageBox::information(this, tr("Cannot send image"),
      tr("Open a one-to-one conversation with an online device that supports file transfers."));
    return;
  }

  QMessageBox confirmation(QMessageBox::NoIcon, tr("Send image?"),
    tr("Send this %1 x %2 image to %3?")
      .arg(image.width()).arg(image.height()).arg(peerNames.value(peerId)),
    QMessageBox::Ok | QMessageBox::Cancel, this);
  confirmation.setTextFormat(Qt::PlainText);
  confirmation.setInformativeText(tr("The image will appear directly in the conversation on updated devices."));
  confirmation.setIconPixmap(QPixmap::fromImage(image.scaled(360, 220, Qt::KeepAspectRatio,
                                                            Qt::SmoothTransformation)));
  confirmation.button(QMessageBox::Ok)->setText(tr("Send image"));
  confirmation.setDefaultButton(QMessageBox::Cancel);
  if(confirmation.exec() != QMessageBox::Ok)
    return;

  // The modal preview processes events: the peer may have gone away meanwhile.
  if(!bConnected || (infoFlag & IT_Offline)) {
    QMessageBox::information(this, tr("Cannot send image"), tr("The other device is now offline."));
    return;
  }

  QTemporaryDir& directory = clipboardAttachmentDirectory();
  if(!directory.isValid()) {
    QMessageBox::warning(this, tr("Cannot prepare image"),
                         tr("The temporary image folder is unavailable."));
    return;
  }
  const QFileInfoList pending = QDir(directory.path()).entryInfoList(QDir::Files);
  qint64 pendingBytes = 0;
  for(const QFileInfo& file : pending)
    pendingBytes += file.size();
  // Bound disk staging as well as decoded image memory. A closed conversation
  // may retain its in-flight file until app exit, rather than break a transfer.
  if(pending.size() >= 64 ||
     pendingBytes + image.sizeInBytes() + 1024 * 1024 > 256 * 1024 * 1024) {
    QMessageBox::warning(this, tr("Cannot prepare image"),
      tr("The temporary image folder is unavailable or full. Finish current image transfers, "
         "or save the image and use Send File."));
    return;
  }

  QString error;
  QString path = ClipboardImage::savePng(image, directory.path(), &error);
  if(path.isEmpty()) {
    QMessageBox::warning(this, tr("Cannot prepare image"), error);
    return;
  }
  sendFile(&path);
}

void lmcChatWindow::btnSend_clicked(void) {
  sendMessage();
  setChatState(CS_Active);
}

void lmcChatWindow::smileyAction_triggered(void) {
  //	nSmiley contains index of smiley
  if(showSmiley) {
    QString htmlPic("<html><head></head><body><img src='" + smileyPic[nSmiley] + "' /></body></html>");
    ui.txtMessage->insertHtml(htmlPic);
  } else
    ui.txtMessage->insertPlainText(smileyCode[nSmiley]);
}

void lmcChatWindow::log_sendMessage(MessageType type, QString* lpszUserId, XmlMessage* pMessage) {
  emit messageSent(type, lpszUserId, pMessage);
}

void lmcChatWindow::checkChatState(void) {
  if(keyStroke > snapKeyStroke) {
    snapKeyStroke = keyStroke;
    QTimer::singleShot(pauseTime, this, &lmcChatWindow::checkChatState);
    return;
  }

  if(ui.txtMessage->document()->isEmpty())
    setChatState(CS_Active);
  else
    setChatState(CS_Paused);
}

void lmcChatWindow::createSmileyMenu(void) {
  pSmileyAction = new lmcImagePickerAction(this, smileyPic, SM_COUNT, 19, 10, &nSmiley);
  connect(pSmileyAction, &QAction::triggered, this, &lmcChatWindow::smileyAction_triggered);

  pSmileyMenu = new QMenu(this);
  pSmileyMenu->addAction(pSmileyAction);
}

void lmcChatWindow::createToolBar(void) {
  QToolBar* pLeftBar = new QToolBar(ui.wgtToolBar);
  pLeftBar->setStyleSheet("QToolBar { border: 0px }");
  pLeftBar->setIconSize(QSize(16, 16));
  ui.toolBarLayout->addWidget(pLeftBar);

  pFontAction = ActionHelper::createAction(pLeftBar, "Change Font...", this, &lmcChatWindow::btnFont_clicked,
                                           QIcon(QPixmap(IDR_FONT, "PNG")));
  pFontColorAction = ActionHelper::createAction(pLeftBar, "Change Color...", this,
                                                &lmcChatWindow::btnFontColor_clicked,
                                                QIcon(QPixmap(IDR_FONTCOLOR, "PNG")));

  pLeftBar->addSeparator();

  pbtnSmiley = new lmcToolButton(pLeftBar);
  pbtnSmiley->setIcon(QIcon(QPixmap(IDR_SMILEY, "PNG")));
  pbtnSmiley->setPopupMode(QToolButton::InstantPopup);
  pbtnSmiley->setMenu(pSmileyMenu);
  QAction* smileyWidgetAction = pLeftBar->addWidget(pbtnSmiley);

  pLeftBar->addSeparator();

  pFileAction = ActionHelper::createAction(pLeftBar, "Send A &File...", this, &lmcChatWindow::btnFile_clicked,
                                           QIcon(QPixmap(IDR_FILE, "PNG")), QKeySequence::Open);
  bool fileCap = ((peerCaps.value(peerId) & UC_File) == UC_File);
  pFileAction->setEnabled(fileCap);
  pFolderAction = ActionHelper::createAction(pLeftBar, "Send A Fol&der...", this, &lmcChatWindow::btnFolder_clicked,
                                             QIcon(QPixmap(IDR_SENDFOLDER, "PNG")));
  bool folderCap = ((peerCaps.value(peerId) & UC_Folder) == UC_Folder);
  pFolderAction->setEnabled(folderCap);

  pLeftBar->addSeparator();

  pSaveAction = ActionHelper::createAction(pLeftBar, "&Save As...", this, &lmcChatWindow::btnSave_clicked,
                                           QIcon(QPixmap(IDR_SAVE, "PNG")), QKeySequence::Save);
  pSaveAction->setEnabled(false);

  pRightBar = new QToolBar(ui.wgtToolBar);
  pRightBar->setStyleSheet("QToolBar { border: 0px }");
  pRightBar->setIconSize(QSize(16, 16));
  pRightBar->setLayoutDirection(Qt::RightToLeft);
  ui.toolBarLayout->addWidget(pRightBar);

  pHistoryAction = ActionHelper::createAction(pRightBar, "&History", this, &lmcChatWindow::btnHistory_clicked,
                                              QIcon(QPixmap(IDR_HISTORY, "PNG")),
                                              QKeySequence(Qt::CTRL | Qt::Key_H));
  pTransferAction = ActionHelper::createAction(pRightBar, "File &Transfers", this,
                                               &lmcChatWindow::btnTransfers_clicked,
                                               QIcon(QPixmap(IDR_TRANSFER, "PNG")),
                                               QKeySequence(Qt::CTRL | Qt::Key_J));

  ui.lblDividerTop->setBackgroundRole(QPalette::Light);
  ui.lblDividerTop->setAutoFillBackground(true);
  ui.lblDividerBottom->setBackgroundRole(QPalette::Dark);
  ui.lblDividerBottom->setAutoFillBackground(true);

#ifdef LMC_LITE_BUILD
  pLeftBar->setIconSize(QSize(20, 20));
  pRightBar->setIconSize(QSize(20, 20));
  pLeftBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  for(QToolBar* bar : {pLeftBar, pRightBar}) {
    for(auto* button : bar->findChildren<QToolButton*>())
      button->setFocusPolicy(Qt::StrongFocus);
  }
  // Keep the conversation toolbar focused on file transfer and history. The
  // actions still exist so language changes and upstream code remain valid.
  pFontAction->setVisible(false);
  pFontColorAction->setVisible(false);
  smileyWidgetAction->setVisible(false);
  pSaveAction->setVisible(false);
  for(QAction* action : pLeftBar->actions()) {
    if(action->isSeparator())
      action->setVisible(false);
  }
#endif
  Q_UNUSED(smileyWidgetAction);
}

void lmcChatWindow::setUIText(void) {
  ui.retranslateUi(this);

  setWindowTitle(getWindowTitle());
  pRightBar->setLayoutDirection((QApplication::layoutDirection() == Qt::LeftToRight ? Qt::RightToLeft : Qt::LeftToRight));

  pbtnSmiley->setToolTip(tr("Insert Smiley"));
  pFileAction->setText(tr("Send A &File..."));
  pFolderAction->setText(tr("Send A Fol&der..."));
  pSaveAction->setText(tr("&Save As..."));
  pHistoryAction->setText(tr("&History"));
  pTransferAction->setText(tr("File &Transfers"));
  if(!groupMode) {
    QString toolTip = tr("Send a file to %1");
    pFileAction->setToolTip(toolTip.arg(peerNames.value(peerId)));
    toolTip = tr("Send a folder to %1");
    pFolderAction->setToolTip(toolTip.arg(peerNames.value(peerId)));
  }
  pSaveAction->setToolTip(tr("Save this conversation"));
  pHistoryAction->setToolTip(tr("View History"));
  pTransferAction->setToolTip(tr("View File Transfers"));
  ui.btnSend->setText(tr("Send"));
  ui.btnSend->setToolTip(tr("Send message"));
  ui.txtMessage->setPlaceholderText(tr("Type a message, paste an image, or drop files/folders..."));
  ui.txtMessage->setAccessibleName(tr("Send message"));
#ifdef LMC_LITE_BUILD
  pFileAction->setIconText(tr("File"));
  pFolderAction->setIconText(tr("Folder"));
  ui.btnSend->setToolTip(tr("Send message") + QStringLiteral(" (%1)").arg(
      QKeySequence(sendKeyMod ? Qt::CTRL | Qt::Key_Return : Qt::Key_Return).toString(QKeySequence::NativeText)));
#endif
  pFontAction->setText(tr("Change Font..."));
  pFontAction->setToolTip(tr("Change message font"));
  pFontColorAction->setText(tr("Change Color..."));
  pFontColorAction->setToolTip(tr("Change message text color"));

  showStatus(IT_Ok, true); //	this will force the info label to retranslate
}

void lmcChatWindow::sendMessage(void) {
  if(ui.txtMessage->document()->isEmpty())
    return;

  if(bConnected) {
    QString szHtmlMessage(ui.txtMessage->toHtml());
    encodeMessage(&szHtmlMessage);
    QTextDocument docMessage;
    docMessage.setHtml(szHtmlMessage);
    QString szMessage = docMessage.toPlainText();

    QFont font = ui.txtMessage->font();
    font.setPointSize(ui.txtMessage->fontPointSize());

    MessageType type = groupMode ? MT_GroupMessage : MT_Message;
    XmlMessage xmlMessage;
    xmlMessage.addHeader(XN_TIME, QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    xmlMessage.addData(XN_FONT, font.toString());
    xmlMessage.addData(XN_COLOR, messageColor.name());
    if(groupMode) {
      xmlMessage.addData(XN_THREAD, threadId);
      xmlMessage.addData(XN_GROUPMESSAGE, szMessage);
    } else
      xmlMessage.addData(XN_MESSAGE, szMessage);

    appendMessageLog(MT_Message, &localId, &localName, &xmlMessage);

    QHash<QString, QString>::const_iterator index = peerIds.constBegin();
    while(index != peerIds.constEnd()) {
      QString userId = index.value();
      emit messageSent(type, &userId, &xmlMessage);
      index++;
    }
  } else
    appendMessageLog(MT_Error, NULL, NULL, NULL);

  ui.txtMessage->clear();
  ui.txtMessage->setFocus();
}

void lmcChatWindow::sendFile(QString* lpszFilePath) {
  sendObject(MT_File, lpszFilePath);
}

void lmcChatWindow::sendFolder(QString* lpszFolderPath) {
  sendObject(MT_Folder, lpszFolderPath);
}

void lmcChatWindow::sendObject(MessageType type, QString* lpszPath) {
  if(bConnected) {
    XmlMessage xmlMessage;
    xmlMessage.addData(XN_FILETYPE, FileTypeNames[FT_Normal]);
    xmlMessage.addData(XN_FILEOP, FileOpNames[FO_Request]);
    xmlMessage.addData(XN_FILEPATH, *lpszPath);
    if(type == MT_File && isClipboardAttachment(*lpszPath))
      xmlMessage.addData(XN_CONTENTTYPE, "image/png");

    QString userId = peerIds.value(peerId);
    emit messageSent(type, &userId, &xmlMessage);
  } else
    appendMessageLog(MT_Error, NULL, NULL, NULL);
}

//	Called before sending message
void lmcChatWindow::encodeMessage(QString* lpszMessage) {
  //	replace all emoticon images with corresponding text code
  ChatHelper::encodeSmileys(lpszMessage);
}

void lmcChatWindow::processFileOp(XmlMessage* pMessage) {
  int fileOp = Helper::indexOf(FileOpNames, FO_Max, pMessage->data(XN_FILEOP));
  int fileMode = Helper::indexOf(FileModeNames, FM_Max, pMessage->data(XN_MODE));
  QString fileId = pMessage->data(XN_FILEID);
  QString filePath = (fileOp == FO_Accept || fileOp == FO_Complete)
    ? pMessage->data(XN_FILEPATH) : QString();

  if(fileMode == FM_Send && fileOp != FO_Accept &&
     (fileOp == FO_Complete || fileOp == FO_Decline || fileOp == FO_Cancel ||
      fileOp == FO_Abort || fileOp == FO_Error)) {
    const QString imagePath = clipboardTransfers.take(fileId);
    if(isClipboardAttachment(imagePath)) {
      if(fileOp == FO_Complete) {
        const QDir sentImages(QDir(StdLocation::fileStorageDir()).filePath("Sent Images"));
        const QString savedPath = sentImages.filePath(QFileInfo(imagePath).fileName());
        if(QDir().mkpath(sentImages.path()) && QFile::rename(imagePath, savedPath))
          filePath = savedPath;
        // On a storage failure keep the staging image until exit for this conversation.
      } else {
        QFile::remove(imagePath);
      }
    }
  }

  switch(fileOp) {
  case FO_Cancel:
  case FO_Accept:
  case FO_Decline:
  case FO_Error:
  case FO_Abort:
  case FO_Complete:
    updateFileMessage((FileMode)fileMode, (FileOp)fileOp, fileId, filePath);
    break;
  default:
    break;
  }
}

void lmcChatWindow::appendMessageLog(MessageType type, QString* lpszUserId, QString* lpszUserName, XmlMessage* pMessage) {
  pMessageLog->appendMessageLog(type, lpszUserId, lpszUserName, pMessage);
  if(!pSaveAction->isEnabled())
    pSaveAction->setEnabled(pMessageLog->hasData);
}

void lmcChatWindow::updateFileMessage(FileMode mode, FileOp op, QString fileId, QString filePath) {
  pMessageLog->updateFileMessage(mode, op, fileId, filePath);
}

void lmcChatWindow::showStatus(int flag, bool add) {
  infoFlag = add ? infoFlag | flag : infoFlag & ~flag;

  // TODO
  //	int relScrollPos = pMessageLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) -
  //			pMessageLog->page()->mainFrame()->scrollBarValue(Qt::Vertical);

  //ui.lblInfo->setStyleSheet("QLabel { background-color:white; }");
  if(infoFlag & IT_Disconnected) {
    ui.lblInfo->setText("<span style='color:rgb(96,96,96);'>" + tr("You are no longer connected.") + "</span>");
    ui.lblInfo->setVisible(true);
  } else if(!groupMode && (infoFlag & IT_Offline)) {
    QString msg = tr("%1 is offline.");
    ui.lblInfo->setText("<span style='color:rgb(96,96,96);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
    ui.lblInfo->setVisible(true);
  } else if(!groupMode && (infoFlag & IT_Away)) {
    QString msg = tr("%1 is away.");
    ui.lblInfo->setText("<span style='color:rgb(255,115,0);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
    ui.lblInfo->setVisible(true);
  } else if(!groupMode && (infoFlag & IT_Busy)) {
    QString msg = tr("%1 is busy. You may be interrupting.");
    ui.lblInfo->setText("<span style='color:rgb(192,0,0);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
    ui.lblInfo->setVisible(true);
  } else {
    ui.lblInfo->setText(QString());
    ui.lblInfo->setVisible(false);
  }

  // TODO
  //	int scrollPos = pMessageLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) - relScrollPos;
  //	pMessageLog->page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollPos);
}

QString lmcChatWindow::getWindowTitle(void) {
  QString title = QString();

  QHash<QString, QString>::const_iterator index = peerNames.constBegin();
  while(index != peerNames.constEnd()) {
    title.append(index.value() + ", ");
    index++;
  }

  //	remove the final comma and space
  title.remove(title.length() - 2, 2);
  title.append(" - ");
  title.append(tr("Conversation"));
  return title;
}

void lmcChatWindow::setMessageFont(QFont& font) {
  ui.txtMessage->setFont(font);
  ui.txtMessage->setFontPointSize(font.pointSize());
}

void lmcChatWindow::setChatState(ChatState newChatState) {
  if(chatState == newChatState)
    return;

  bool bNotify = false;

  switch(newChatState) {
  case CS_Active:
  case CS_Inactive:
    chatState = newChatState;
    bNotify = true;
    break;
  case CS_Composing:
    chatState = newChatState;
    bNotify = true;
    snapKeyStroke = keyStroke;
    QTimer::singleShot(pauseTime, this, &lmcChatWindow::checkChatState);
    break;
  case CS_Paused:
    if(chatState != CS_Inactive) {
      chatState = newChatState;
      bNotify = true;
    }
    break;
  default:
    break;
  }

  // send a chat state message
  if(bNotify && bConnected) {
    XmlMessage xmlMessage;
    if(groupMode)
      xmlMessage.addData(XN_THREAD, threadId);
    xmlMessage.addData(XN_CHATSTATE, ChatStateNames[chatState]);

    QHash<QString, QString>::const_iterator index = peerIds.constBegin();
    while(index != peerIds.constEnd()) {
      QString userId = index.value();
      emit messageSent(MT_ChatState, &userId, &xmlMessage);
      index++;
    }
  }
}
