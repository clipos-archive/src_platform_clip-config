// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUserChpw.h
 * clip-config password change form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMUSERSCHPW_H
#define FRMUSERSCHPW_H

#include <QDialog>
#include <QString>
#include <QThread>
#include "userd/userd.h"

class QLabel;
class QLineEdit;
class QPushButton;

class frmUsersChpw : public QDialog
{
  Q_OBJECT

public:
  frmUsersChpw(QWidget *parent);

private:
  QLineEdit *edtOldPw;
  QLineEdit *edtNewPw[2];
  QLabel *lblMatch;
  QPushButton *btnSave;
  QPushButton *btnCancel;
  bool ok;
  bool match;
  bool changed;

private slots:
  void update(void);
  void save(void);
  void cancel(void);
};


class ChangeThread : public QThread
{
 public:
  ChangeThread(const QString &oldpw, const QString &newpw, 
		bool &fatal, uint32_t& ptrRet, bool& ptrFinished);
  virtual void run();
 private:
  const QString oldPw;
  const QString newPw;
  bool& fatal;
  uint32_t& ret;
  bool& finished;
};

#endif /* FRMUSERSCHPW_H */

// vi:sw=2:ts=2:et:co=80:
