// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmDateTime.h
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011 SGDSN/SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMDATETIME_H
#define FRMDATETIME_H


#include <QDialog>

class QCalendarWidget;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTimeEdit;

class ClipConfigFile;

class frmDateTime : public QDialog
{
  Q_OBJECT

 public:
  frmDateTime(void);
 
 private slots:
  void quit(void);
  bool apply(void);
  void load(void);
  void update(void);
  void setPending(void);
  void enableRemoteSync(int);
  void updateDay(void);

 private:
  bool pending;
  ClipConfigFile *file;

  QPushButton *btnQuit;
  QPushButton *btnApply;

  QCalendarWidget *edtDate;
  QTimeEdit *edtTime;
  QCheckBox *chkRemoteSync;
  QLabel *lblRemoteUrl;
  QLineEdit *edtRemoteUrl;
};

#endif // FRMDATETIME_H

// vi:sw=2:ts=2:et:co=80:
