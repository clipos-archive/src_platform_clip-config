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

#include <QApplication>
#include <QDir>
#include <QDesktopWidget>
#include <QLayout>
#include <QTabWidget>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include "PkgListTab.h"
#include "frmPkgMain.h"
#include "qtcommon.h"

static const QString 
  g_clipOptionalFile("/etc/admin/clip_install/optional.conf.clip");
static const QString 
  g_rmhOptionalFile("/etc/admin/rm_h/clip_install/optional.conf.rm");
static const QString 
  g_rmbOptionalFile("/etc/admin/rm_b/clip_install/optional.conf.rm");

static const QString 
  g_clipOptionalList("/var/pkg/cache/apt/clip_download/clip/optional.cache");
static const QString 
  g_rmhOptionalList("/var/pkg/rm_h/cache/apt/clip_download/rm/optional.cache");
static const QString 
  g_rmbOptionalList("/var/pkg/rm_b/cache/apt/clip_download/rm/optional.cache");


using namespace std;

frmPkgMain::frmPkgMain() 
  : QDialog(0)
{
  setWindowTitle("Gestion des mises à jour");
  setWindowIcon(QIcon(ICONPATH"preferences-update.png"));

  QRect screen = QApplication::desktop()->screenGeometry();

  if (screen.height() < 800) 
    setMinimumHeight(screen.height() - 150);
  else
    setMinimumHeight(550);

  setMinimumWidth(600);

  tabWidget = new QTabWidget(this);

  PkgListTab *tab_rmh = NULL, *tab_rmb = NULL;

  int width = 0;
  if (hasRMH) {
    tab_rmh = new PkgListTab(this, "rm_h", 
                        g_rmhOptionalFile, g_rmhOptionalList);
    tabWidget->addTab(tab_rmh, QIcon(ICONPATH"rmh.png"), "Niveau Haut");
    tabs.push_back(tab_rmh);
    if (tab_rmh->width() > width) 
      width = tab_rmh->width();
  }
  if (hasRMB) {
    tab_rmb = new PkgListTab(this, "rm_b", 
                        g_rmbOptionalFile, g_rmbOptionalList);
    tabWidget->addTab(tab_rmb, QIcon(ICONPATH"rmb.png"), "Niveau Bas");
    tabs.push_back(tab_rmb);
    if (tab_rmb->width() > width) 
      width = tab_rmb->width();
  }

  PkgListTab *tab_clip = 
    new PkgListTab(this, "clip", g_clipOptionalFile, g_clipOptionalList);
  tabWidget->addTab(tab_clip, QIcon(ICONPATH"clip.png"), "Socle CLIP");
  tabs.push_back(tab_clip);
  if (tab_clip->width() > width) 
    width = tab_clip->width();

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(10);
  vbox->addWidget(tabWidget);
  vbox->addSpacing(10);

  QRect g = geometry();
  resize(width, g.height());
}

void 
frmPkgMain::quit(void)
{
  int i = 0;
  for (QLinkedList<PkgListTab *>::const_iterator it = tabs.constBegin();
                  it != tabs.constEnd(); ++it) {
    if (!(*it)->tryQuit()) {
      tabWidget->setCurrentIndex(i);
      if (!(*it)->confirmQuit())
        return;
    }
    i++;
  }

  QApplication::exit(0);
}

// vi:sw=2:ts=2:et:co=80:
