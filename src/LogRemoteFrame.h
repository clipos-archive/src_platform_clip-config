// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file LogRemoteFrame.h
 * clip-audit-config remote logging config frame header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2014 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _LOG_REMOTE_FRAME_H
#define _LOG_REMOTE_FRAME_H

#include <QFrame>

class QCheckBox;
class QLabel;
class QLineEdit;
class QString;
class QSpinBox;

class ClipConfigFile;

class LogRemoteFrame : public QFrame
{
  Q_OBJECT

 public:
  LogRemoteFrame(QWidget *p, const QString &path);
  bool pending();
  void save();
 
 private slots:
  void updateWidgets(void);
  void enableTLS(int state);
  void enableTLSClient(int state);

 private:
  bool changed;
  ClipConfigFile *file;

  QLabel *lblURL;
  QLabel *lblPort;
  QLabel *lblLogs;

  QLineEdit *edtURL;
  QSpinBox *spbPort;

  QCheckBox *chkRemote;
  QCheckBox *chkTLS;
  QCheckBox *chkTLSClient;
  
  QCheckBox *chkLogNet;
  QCheckBox *chkLogUpdate;
  QCheckBox *chkLogSecAlert;
  QCheckBox *chkLogSecWarn;
  QCheckBox *chkLogSecInfo;
  QCheckBox *chkLogOther;

  void read(const QString &path);
  void update();
};

#endif // _LOG_REMOTE_FRAME_H

// vi:sw=2:ts=2:et:co=80:
