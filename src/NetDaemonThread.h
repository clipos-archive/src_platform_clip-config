// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file NetDaemonThread.h
 * clip-config net daemon communication thread header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef NETDAEMONTHREAD_H
#define NETDAEMONTHREAD_H

#include <QThread>
#include <QMessageBox>

extern void activation_warning(char c, const QString &pname);
extern void scan_warning(char c);

class NetDaemonThread : public QThread
{
 public:
  NetDaemonThread(char action, const QString& profileName, 
                        char& ptrResult, bool& ptrFinished);
  virtual void run();
 private:
  char action;
  const QString& profileName;
  char& ptrResult;
  bool& ptrFinished;
};

#endif // NETDAEMONTHREAD_H

// vi:sw=2:ts=2:et:co=80:
