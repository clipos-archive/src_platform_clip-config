// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file PkgListTab.h
 * clip-config scrollable package list header.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef _PKG_LIST_TAB_H
#define _PKG_LIST_TAB_H

#define QT_THREAD_SUPPORT 1

#include <QFrame>
#include <QThread>

class PkgTable;
class frmPkgMain;

class PkgListTab : public QFrame
{
  Q_OBJECT

 public:
  PkgListTab(frmPkgMain *parent, const QString &jail, 
                  const QString &output, const QString &input);

 public slots:
  bool tryQuit(void);
  bool confirmQuit(void);
  bool save(void);
  bool download(void);
  int width() const { return mwidth; };

 private:
  PkgTable *tb;
  QString jail;
  int mwidth;
};

class DownloadThread : public QThread
{

 public:
  DownloadThread(const QString& jail, char& ptrResult, bool& ptrFinished);
  virtual void run(void);

 private:
  char& ptrResult;
  const QString& jail;
  bool& ptrFinished;
};

#endif

// vi:sw=2:ts=2:et:co=80:
