// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2009-2018 ANSSI. All Rights Reserved.
/**
 * @file frmPkgMain.cpp
 * clip-config main package management form.
 * @author Olivier Levillain <clipos@ssi.gouv.fr>
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN/DCSSI
 * Copyright (C) 2010 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMPKGMAIN_H
#define FRMPKGMAIN_H

#define QT_THREAD_SUPPORT 1

#include <QDialog>
#include <QLinkedList>

class PkgListTab;
class QTabWidget;

class frmPkgMain : public QDialog
{
  Q_OBJECT

 public:
  frmPkgMain ();
 
 public slots:
  void quit(void);

 private:
  QLinkedList<PkgListTab *> tabs;
  QTabWidget *tabWidget;
};

#endif // FRMPKGMAIN_H

// vi:sw=2:ts=2:et:co=80:
