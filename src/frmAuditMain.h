// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmAuditMain.h
 * clip-audit-config main dialog header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMAUDITMAIN_H
#define FRMAUDITMAIN_H

#include <QDialog>

class LogRotateFrame;
class LogRemoteFrame;
class QPushButton;
class QKeyEvent;

class frmAuditMain : public QDialog
{
  Q_OBJECT
 
 public:
  frmAuditMain(QWidget *parent, const QString &path);

 protected slots:
  void keyPressEvent(QKeyEvent *);

 private slots:
  bool save();
  void quit();

 private:
  bool pending();

  QPushButton *btnSave;
  QPushButton *btnQuit;
  
  LogRotateFrame *frmRotate;
  LogRemoteFrame *frmRemote;
};

#endif // FRMAUDITMAIN_H

// vi:sw=2:ts=2:et:co=80:
