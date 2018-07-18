// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersAdd.h
 * clip-config user add form header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMUSERSADD_H
#define FRMUSERSADD_H

#define QT_THREAD_SUPPORT 1

#include <QDialog>
#include <QString>
#include <QThread>
#include <userd/userd.h>

class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSlider;

extern const char *type_str[];
extern const char *auth_str[];
extern void print_error(uint32_t ret, QString verb, QString name);

class frmUsersAdd : public QDialog
{
  Q_OBJECT

public:
  frmUsersAdd(const diskinfo_t& di);
  virtual ~frmUsersAdd();

 private:
  int numRM;
  QLineEdit *edtName;
  QLabel *lblPasswd;
  QLineEdit *edtPasswd[2];
  QGroupBox *btnGroup;
  QButtonGroup *btnAuthType;
  QButtonGroup *btnType;
  QLabel *lblTotalSize;
  QSlider *sldTotalSize;
  QLabel *lblRMBSize;
  QLabel *lblRMHSize;
  QPushButton *btnReset;
  QSlider *sldRepartition;
  QLabel *lblPwMatch;
  QLabel *lblPinMatch;

  QPushButton *btnAdd;
  QPushButton *btnCancel;

  QRadioButton *btnAuthPw;
  QRadioButton *btnAuthPkcs;

  QRadioButton *btnAdmin;
  QRadioButton *btnAudit;
  QRadioButton *btnUser;
  QRadioButton *btnPrivUser;
  QRadioButton *btnNomUser;

  const diskinfo_t& diskinfo;
  userinfo_t *userData;
  bool pwOk;

protected:

private slots:
  void updatePw();
  void authSelected(int type);
  void updateWidgets();
  void resetRepartition ();
  void giveUserData();
};


class AddThread : public QThread
{
 public:
  AddThread(userinfo_t *data, bool& ptrFatal, 
                uint32_t& ptrRet, bool& ptrFinished);
  virtual void run();
 private:
  userinfo_t *userData;
  bool& fatal;
  uint32_t& ret;
  bool& finished;
};



#endif // FRMUSERSADD_H

// vi:sw=2:ts=2:et:co=80:
