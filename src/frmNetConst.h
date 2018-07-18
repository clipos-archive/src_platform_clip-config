// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmNetConst.h
 * clip-config-netconst main dialog header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMNETCONST_H
#define FRMNETCONST_H

#include <QDialog>
#include "consts.h"

class QCheckBox;
class QPushButton;
class IPField;
class ClipConfigFile;
class QKeyEvent;

class frmNetConst : public QDialog
{
  Q_OBJECT
 
 public:
  frmNetConst(void);

 protected slots:
  void keyPressEvent(QKeyEvent *);

 private slots:
  void update(void);
  void save(void);
  bool write(void);
  void load(void);
  void quit(void);

  void enableUpdateVirt(int);
  void enableAdminVirt(int);
  void enableAuditVirt(int);
  void enableRmhVirt(int);

 private:
  ClipConfigFile *file;

  QPushButton *btnSave;
  QPushButton *btnQuit;

  QCheckBox *chkUpdateVirt;
  QCheckBox *chkAdminVirt;
  QCheckBox *chkAuditVirt;
  QCheckBox *chkRmhVirt;

  IPField *updateAddr;
  IPField *adminAddr;
  IPField *auditAddr;
  IPField *rmhAddr;
};

#endif // FRMNETCONST_H

// vi:sw=2:ts=2:et:co=80:
