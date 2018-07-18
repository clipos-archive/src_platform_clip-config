// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersMain.h
 * clip-config user management main form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMUSERSMAIN_H
#define FRMUSERSMAIN_H

#include <QDialog>

#include <userd/userd.h>

class QLabel;
class QPushButton;
class QTreeWidget;

class frmUsersMain : public QDialog
{
  Q_OBJECT

 public:
  frmUsersMain();

 private:
  diskinfo_t *diskinfo;
  int columnCount;

  QLabel *lblFreeSpace;
  QTreeWidget *lstUsers;
  QPushButton *btnAddUser;
  QPushButton *btnDelUser;
  QPushButton *btnMigrate;
  QPushButton *btnLockUser;
  QPushButton *btnUnlockUser;
  QPushButton *btnQuit;

  QString getConfirmation(const QString& verb);

 private slots:
  bool updateUserList();
  bool updateFreeSpace();
  void updateButtons();

  void addUser();
  void delUser();
  void migrateUser();
  void lockUnlockUser(bool lock);
  void lockUser();
  void unlockUser();
};

#endif // FRMUSERSMAIN_H

// vi:sw=2:ts=2:et:co=80:
