// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmUsersMigrate.h
 * clip-config user migrate form header.
 *
 * Copyright (C) 2013 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMUSERSMIGRATE_H
#define FRMUSERSMIGRATE_H

#define QT_THREAD_SUPPORT 1

#include <QDialog>
#include <QString>
#include <QThread>
#include <userd/userd.h>

class QButtonGroup;
class QLabel;
class QLineEdit;
class QPushButton;



class frmUsersMigrate : public QDialog
{
  Q_OBJECT

public:
  frmUsersMigrate(QString user);
  virtual ~frmUsersMigrate();

 private:
  QLineEdit *edtPasswd[3];
  QLabel *lblPwMatch;
  QLabel *lblPinMatch;

  QPushButton *btnMigrate;
  QPushButton *btnCancel;

  QString name;
  bool pwOk;

protected:

private slots:
  void updatePw();
  void commit();
};


class MigrateThread : public QThread
{
 public:
  MigrateThread(const char *user, const char *password, const char *pincode,
                bool& ptrFatal, uint32_t& ptrRet, bool& ptrFinished);
  virtual void run();

 private:
  const char *name;
  const char *passwd;
  const char *pin;
  bool& fatal;
  uint32_t& ret;
  bool& finished;
};



#endif // FRMUSERSMIGRATE_H
