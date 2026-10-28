// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ATTACHMENTDIALOG_H
#define ATTACHMENTDIALOG_H

#include <QFileDialog>
#include <QListView>
#include <QTreeView>

namespace AttachmentDialog {
inline QStringList files(QWidget* parent, const QString& directory) {
  QFileDialog::Options options;
  // The GUI test harness can only drive Qt dialogs.
  if(!qEnvironmentVariableIsEmpty("LMC_NON_NATIVE_DIALOGS"))
    options |= QFileDialog::DontUseNativeDialog;
  return QFileDialog::getOpenFileNames(parent, QString(), directory, QString(), nullptr, options);
}

inline QStringList folders(QWidget* parent, const QString& directory) {
  // QFileDialog has no native multiple-directory mode. Enable extended
  // selection in both Qt views while retaining directory-only validation.
  QFileDialog dialog(parent);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::ShowDirsOnly);
  dialog.setDirectory(directory);
  if(auto* list = dialog.findChild<QListView*>("listView"))
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  if(auto* tree = dialog.findChild<QTreeView*>("treeView"))
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  return dialog.exec() == QDialog::Accepted ? dialog.selectedFiles() : QStringList();
}
}

#endif // ATTACHMENTDIALOG_H
